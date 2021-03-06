// Copyright (C) 2019-2020 Zilliz. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied. See the License for the specific language governing permissions and limitations under the License.
#ifdef MILVUS_GPU_VERSION
#include "scheduler/optimizer/handler/GpuSearchResHandler.h"

#include <string>
#include <vector>

#include "server/Config.h"

namespace milvus {
namespace scheduler {

GpuSearchResHandler::GpuSearchResHandler() {
    server::Config& config = server::Config::GetInstance();

    Status s = config.GetEngineConfigGpuSearchThreshold(threshold_);
    if (!s.ok()) {
        threshold_ = std::numeric_limits<int32_t>::max();
    }

    config.GetGpuResourceConfigSearchResources(search_gpus_);
}

GpuSearchResHandler::~GpuSearchResHandler() {
    server::Config& config = server::Config::GetInstance();
    config.CancelCallBack(server::CONFIG_ENGINE, server::CONFIG_ENGINE_GPU_SEARCH_THRESHOLD, identity_);
    config.CancelCallBack(server::CONFIG_GPU_RESOURCE, server::CONFIG_GPU_RESOURCE_SEARCH_RESOURCES, identity_);
}

////////////////////////////////////////////////////////////////////////
void
GpuSearchResHandler::OnGpuSearchThresholdChanged(int64_t threshold) {
    threshold_ = threshold;
}

void
GpuSearchResHandler::OnGpuSearchResChanged(const std::vector<int64_t>& gpus) {
    search_gpus_ = gpus;
}

void
GpuSearchResHandler::AddGpuSearchThresholdListener() {
    server::Config& config = server::Config::GetInstance();

    server::ConfigCallBackF lambda_gpu_threshold = [this](const std::string& value) -> Status {
        server::Config& config = server::Config::GetInstance();
        int64_t threshold;
        auto status = config.GetEngineConfigGpuSearchThreshold(threshold);
        if (status.ok()) {
            OnGpuSearchThresholdChanged(threshold);
        }

        return status;
    };
    config.RegisterCallBack(server::CONFIG_ENGINE, server::CONFIG_ENGINE_GPU_SEARCH_THRESHOLD, identity_,
                            lambda_gpu_threshold);
}

void
GpuSearchResHandler::AddGpuSearchResListener() {
    server::Config& config = server::Config::GetInstance();

    server::ConfigCallBackF lambda_gpu_search_res = [this](const std::string& value) -> Status {
        server::Config& config = server::Config::GetInstance();
        std::vector<int64_t> gpu_ids;
        auto status = config.GetGpuResourceConfigSearchResources(gpu_ids);
        if (status.ok()) {
            OnGpuSearchResChanged(gpu_ids);
        }

        return status;
    };
    config.RegisterCallBack(server::CONFIG_GPU_RESOURCE, server::CONFIG_GPU_RESOURCE_SEARCH_RESOURCES, identity_,
                            lambda_gpu_search_res);
}

}  // namespace scheduler
}  // namespace milvus
#endif
