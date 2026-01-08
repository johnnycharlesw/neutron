// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "shell/common/extensions/neutron_extensions_client.h"

#include <memory>
#include <string>

#include "base/no_destructor.h"
#include "base/notimplemented.h"
#include "components/version_info/version_info.h"
#include "extensions/common/core_extensions_api_provider.h"
#include "extensions/common/extension_urls.h"
#include "extensions/common/features/simple_feature.h"
#include "extensions/common/permissions/permission_message_provider.h"
#include "extensions/common/url_pattern_set.h"
#include "shell/common/extensions/neutron_extensions_api_provider.h"

using extensions::ExtensionsClient;

namespace neutron {

namespace {

// TODO(jamescook): Refactor ChromePermissionsMessageProvider so we can share
// code. For now, this implementation does nothing.
class NeutronPermissionMessageProvider
    : public extensions::PermissionMessageProvider {
 public:
  NeutronPermissionMessageProvider() = default;
  ~NeutronPermissionMessageProvider() override = default;

  // disable copy
  NeutronPermissionMessageProvider(const NeutronPermissionMessageProvider&) =
      delete;
  NeutronPermissionMessageProvider& operator=(
      const NeutronPermissionMessageProvider&) = delete;

  // PermissionMessageProvider implementation.
  [[nodiscard]] extensions::PermissionMessages GetPermissionMessages(
      const extensions::PermissionIDSet& permissions) const override {
    return extensions::PermissionMessages();
  }

  [[nodiscard]] bool IsPrivilegeIncrease(
      const extensions::PermissionSet& granted_permissions,
      const extensions::PermissionSet& requested_permissions,
      extensions::Manifest::Type extension_type) const override {
    // Ensure we implement this before shipping.
    NOTREACHED();
  }

  [[nodiscard]] extensions::PermissionIDSet GetAllPermissionIDs(
      const extensions::PermissionSet& permissions,
      extensions::Manifest::Type extension_type) const override {
    return {};
  }
};

}  // namespace

NeutronExtensionsClient::NeutronExtensionsClient()
    : webstore_base_url_(extension_urls::kChromeWebstoreBaseURL),
      new_webstore_base_url_(extension_urls::kNewChromeWebstoreBaseURL),
      webstore_update_url_(extension_urls::kChromeWebstoreUpdateURL) {
  AddAPIProvider(std::make_unique<extensions::CoreExtensionsAPIProvider>());
  AddAPIProvider(std::make_unique<NeutronExtensionsAPIProvider>());
}

NeutronExtensionsClient::~NeutronExtensionsClient() = default;

void NeutronExtensionsClient::Initialize() {
  // TODO(jamescook): Do we need to whitelist any extensions?
}

void NeutronExtensionsClient::InitializeWebStoreUrls(
    base::CommandLine* command_line) {}

const extensions::PermissionMessageProvider&
NeutronExtensionsClient::GetPermissionMessageProvider() const {
  NOTIMPLEMENTED();

  static base::NoDestructor<NeutronPermissionMessageProvider> instance;
  return *instance;
}

const std::string NeutronExtensionsClient::GetProductName() {
  // TODO(samuelmaddock):
  return "app_shell";
}

void NeutronExtensionsClient::FilterHostPermissions(
    const extensions::URLPatternSet& hosts,
    extensions::URLPatternSet* new_hosts,
    extensions::PermissionIDSet* permissions) const {
  NOTIMPLEMENTED();
}

void NeutronExtensionsClient::SetScriptingAllowlist(
    const ExtensionsClient::ScriptingAllowlist& allowlist) {
  scripting_allowlist_ = allowlist;
}

const ExtensionsClient::ScriptingAllowlist&
NeutronExtensionsClient::GetScriptingAllowlist() const {
  // TODO(jamescook): Real whitelist.
  return scripting_allowlist_;
}

extensions::URLPatternSet
NeutronExtensionsClient::GetPermittedChromeSchemeHosts(
    const extensions::Extension* extension,
    const extensions::APIPermissionSet& api_permissions) const {
  return {};
}

bool NeutronExtensionsClient::IsScriptableURL(const GURL& url,
                                               std::string* error) const {
  // No restrictions on URLs.
  return true;
}

const GURL& NeutronExtensionsClient::GetWebstoreBaseURL() const {
  return webstore_base_url_;
}

const GURL& NeutronExtensionsClient::GetNewWebstoreBaseURL() const {
  return new_webstore_base_url_;
}

const GURL& NeutronExtensionsClient::GetWebstoreUpdateURL() const {
  return webstore_update_url_;
}

bool NeutronExtensionsClient::IsBlocklistUpdateURL(const GURL& url) const {
  return false;
}

}  // namespace neutron
