// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_COMMON_EXTENSIONS_ELECTRON_EXTENSIONS_API_PROVIDER_H_
#define ELECTRON_SHELL_COMMON_EXTENSIONS_ELECTRON_EXTENSIONS_API_PROVIDER_H_

#include <string>

#include "extensions/common/extensions_api_provider.h"

namespace neutron {

class NeutronExtensionsAPIProvider : public extensions::ExtensionsAPIProvider {
 public:
  NeutronExtensionsAPIProvider();
  ~NeutronExtensionsAPIProvider() override;

  // disable copy
  NeutronExtensionsAPIProvider(const NeutronExtensionsAPIProvider&) = delete;
  NeutronExtensionsAPIProvider& operator=(
      const NeutronExtensionsAPIProvider&) = delete;

  // ExtensionsAPIProvider:
  void AddAPIFeatures(extensions::FeatureProvider* provider) override;
  void AddManifestFeatures(extensions::FeatureProvider* provider) override;
  void AddPermissionFeatures(extensions::FeatureProvider* provider) override;
  void AddBehaviorFeatures(extensions::FeatureProvider* provider) override;
  void AddAPIJSONSources(
      extensions::JSONFeatureProviderSource* json_source) override;
  bool IsAPISchemaGenerated(const std::string& name) override;
  std::string_view GetAPISchema(const std::string& name) override;
  void RegisterPermissions(
      extensions::PermissionsInfo* permissions_info) override;
  void RegisterManifestHandlers() override;
};

}  // namespace neutron

#endif  // ELECTRON_SHELL_COMMON_EXTENSIONS_ELECTRON_EXTENSIONS_API_PROVIDER_H_
