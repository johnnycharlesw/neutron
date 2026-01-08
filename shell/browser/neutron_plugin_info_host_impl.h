// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SHELL_BROWSER_ELECTRON_PLUGIN_INFO_HOST_IMPL_H_
#define SHELL_BROWSER_ELECTRON_PLUGIN_INFO_HOST_IMPL_H_

#include <string>

#include "shell/common/plugin.mojom.h"

class GURL;

namespace url {
class Origin;
}

namespace neutron {

// Implements NeutronPluginInfoHost interface.
class NeutronPluginInfoHostImpl : public mojom::NeutronPluginInfoHost {
 public:
  NeutronPluginInfoHostImpl();

  NeutronPluginInfoHostImpl(const NeutronPluginInfoHostImpl&) = delete;
  NeutronPluginInfoHostImpl& operator=(const NeutronPluginInfoHostImpl&) =
      delete;

  ~NeutronPluginInfoHostImpl() override;

  // mojom::NeutronPluginInfoHost
  void GetPluginInfo(const GURL& url,
                     const url::Origin& origin,
                     const std::string& mime_type,
                     GetPluginInfoCallback callback) override;
};

}  // namespace neutron

#endif  // SHELL_BROWSER_ELECTRON_PLUGIN_INFO_HOST_IMPL_H_
