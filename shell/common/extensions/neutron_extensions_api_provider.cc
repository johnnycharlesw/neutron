// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "shell/common/extensions/neutron_extensions_api_provider.h"

#include <memory>
#include <string>

#include "base/containers/span.h"
#include "chrome/common/extensions/manifest_handlers/minimum_chrome_version_checker.h"  // nogncheck
#include "neutron/buildflags/buildflags.h"
#include "neutron/shell/common/extensions/api/generated_schemas.h"
#include "extensions/common/alias.h"
#include "extensions/common/features/feature_provider.h"
#include "extensions/common/features/json_feature_provider_source.h"
#include "extensions/common/features/simple_feature.h"
#include "extensions/common/manifest_constants.h"
#include "extensions/common/manifest_handler.h"
#include "extensions/common/manifest_handler_registry.h"
#include "extensions/common/manifest_handlers/chrome_url_overrides_handler.h"
#include "extensions/common/manifest_handlers/permissions_parser.h"
#include "extensions/common/manifest_url_handlers.h"
#include "extensions/common/permissions/permissions_info.h"
#include "shell/common/extensions/api/api_features.h"
#include "shell/common/extensions/api/manifest_features.h"
#include "shell/common/extensions/api/permission_features.h"

namespace extensions {
namespace {

constexpr APIPermissionInfo::InitInfo permissions_to_register[] = {
    {mojom::APIPermissionID::kResourcesPrivate, "resourcesPrivate",
     APIPermissionInfo::kFlagCannotBeOptional},
#if BUILDFLAG(ENABLE_PDF_VIEWER)
    {mojom::APIPermissionID::kPdfViewerPrivate, "pdfViewerPrivate"},
#endif
    {mojom::APIPermissionID::kManagement, "management"},
    {mojom::APIPermissionID::kTab, "tabs",
     APIPermissionInfo::kFlagRequiresManagementUIWarning},
    {mojom::APIPermissionID::kScripting, "scripting",
     APIPermissionInfo::kFlagRequiresManagementUIWarning},
};
base::span<const APIPermissionInfo::InitInfo> GetPermissionInfos() {
  return base::span(permissions_to_register);
}
base::span<const Alias> GetPermissionAliases() {
  return {};
}

}  // namespace
}  // namespace extensions

namespace neutron {

NeutronExtensionsAPIProvider::NeutronExtensionsAPIProvider() = default;
NeutronExtensionsAPIProvider::~NeutronExtensionsAPIProvider() = default;

void NeutronExtensionsAPIProvider::AddAPIFeatures(
    extensions::FeatureProvider* provider) {
  extensions::AddNeutronAPIFeatures(provider);
}

void NeutronExtensionsAPIProvider::AddManifestFeatures(
    extensions::FeatureProvider* provider) {
  extensions::AddNeutronManifestFeatures(provider);
}

void NeutronExtensionsAPIProvider::AddPermissionFeatures(
    extensions::FeatureProvider* provider) {
  extensions::AddNeutronPermissionFeatures(provider);
}

void NeutronExtensionsAPIProvider::AddBehaviorFeatures(
    extensions::FeatureProvider* provider) {
  // No shell-specific behavior features.
}

void NeutronExtensionsAPIProvider::AddAPIJSONSources(
    extensions::JSONFeatureProviderSource* json_source) {
  // json_source->LoadJSON(IDR_SHELL_EXTENSION_API_FEATURES);
}

bool NeutronExtensionsAPIProvider::IsAPISchemaGenerated(
    const std::string& name) {
  return extensions::api::NeutronGeneratedSchemas::IsGenerated(name);
}

std::string_view NeutronExtensionsAPIProvider::GetAPISchema(
    const std::string& name) {
  return extensions::api::NeutronGeneratedSchemas::Get(name);
}

void NeutronExtensionsAPIProvider::RegisterPermissions(
    extensions::PermissionsInfo* permissions_info) {
  permissions_info->RegisterPermissions(extensions::GetPermissionInfos(),
                                        extensions::GetPermissionAliases());
}

void NeutronExtensionsAPIProvider::RegisterManifestHandlers() {
  extensions::ManifestHandlerRegistry* registry =
      extensions::ManifestHandlerRegistry::Get();
  registry->RegisterHandler(
      std::make_unique<extensions::MinimumChromeVersionChecker>());
}

}  // namespace neutron
