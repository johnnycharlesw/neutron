// Copyright (c) 2019 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_ELECTRON_GPU_CLIENT_H_
#define ELECTRON_SHELL_BROWSER_ELECTRON_GPU_CLIENT_H_

#include "content/public/gpu/content_gpu_client.h"

namespace neutron {

class NeutronGpuClient : public content::ContentGpuClient {
 public:
  NeutronGpuClient();

  // disable copy
  NeutronGpuClient(const NeutronGpuClient&) = delete;
  NeutronGpuClient& operator=(const NeutronGpuClient&) = delete;

  // content::ContentGpuClient:
  void PreCreateMessageLoop() override;
};

}  // namespace neutron

#endif  // ELECTRON_SHELL_BROWSER_ELECTRON_GPU_CLIENT_H_
