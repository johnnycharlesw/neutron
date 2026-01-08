// Copyright (c) 2019 Slack Technologies, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/extensions/neutron_extensions_browser_api_provider.h"

#include "extensions/browser/api/i18n/i18n_api.h"
#include "extensions/browser/extension_function_registry.h"
#include "shell/browser/extensions/api/extension_action/extension_action_api.h"
#include "shell/browser/extensions/api/generated_api_registration.h"
#include "shell/browser/extensions/api/scripting/scripting_api.h"
#include "shell/browser/extensions/api/tabs/tabs_api.h"

namespace extensions {

NeutronExtensionsBrowserAPIProvider::NeutronExtensionsBrowserAPIProvider() =
    default;
NeutronExtensionsBrowserAPIProvider::~NeutronExtensionsBrowserAPIProvider() =
    default;

void NeutronExtensionsBrowserAPIProvider::RegisterExtensionFunctions(
    ExtensionFunctionRegistry* registry) {
  // Generated APIs from Neutron.
  api::NeutronGeneratedFunctionRegistry::RegisterAll(registry);
}

}  // namespace extensions
