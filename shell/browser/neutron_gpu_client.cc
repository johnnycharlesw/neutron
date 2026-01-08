// Copyright (c) 2019 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/neutron_gpu_client.h"

#include "base/environment.h"

#if BUILDFLAG(IS_WIN)
#include <windows.h>
#endif

namespace neutron {

NeutronGpuClient::NeutronGpuClient() = default;

void NeutronGpuClient::PreCreateMessageLoop() {
#if BUILDFLAG(IS_WIN)
  auto env = base::Environment::Create();
  if (env->HasVar("ELECTRON_DEFAULT_ERROR_MODE"))
    SetErrorMode(GetErrorMode() & ~SEM_NOGPFAULTERRORBOX);
#endif
}

}  // namespace neutron
