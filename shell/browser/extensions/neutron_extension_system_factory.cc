// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "shell/browser/extensions/neutron_extension_system_factory.h"

#include "base/no_destructor.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"
#include "extensions/browser/extension_prefs_factory.h"
#include "extensions/browser/extension_registry_factory.h"
#include "shell/browser/extensions/neutron_extension_system.h"

using content::BrowserContext;

namespace extensions {

ExtensionSystem* NeutronExtensionSystemFactory::GetForBrowserContext(
    BrowserContext* context) {
  return static_cast<NeutronExtensionSystem*>(
      GetInstance()->GetServiceForBrowserContext(context, true));
}

// static
NeutronExtensionSystemFactory* NeutronExtensionSystemFactory::GetInstance() {
  static base::NoDestructor<NeutronExtensionSystemFactory> instance;
  return instance.get();
}

NeutronExtensionSystemFactory::NeutronExtensionSystemFactory()
    : ExtensionSystemProvider("NeutronExtensionSystem",
                              BrowserContextDependencyManager::GetInstance()) {
  DependsOn(ExtensionPrefsFactory::GetInstance());
  DependsOn(ExtensionRegistryFactory::GetInstance());
}

NeutronExtensionSystemFactory::~NeutronExtensionSystemFactory() = default;

std::unique_ptr<KeyedService>
NeutronExtensionSystemFactory::BuildServiceInstanceForBrowserContext(
    BrowserContext* context) const {
  return std::make_unique<NeutronExtensionSystem>(context);
}

BrowserContext* NeutronExtensionSystemFactory::GetBrowserContextToUse(
    BrowserContext* context) const {
  // Use a separate instance for incognito.
  return context;
}

bool NeutronExtensionSystemFactory::ServiceIsCreatedWithBrowserContext()
    const {
  return true;
}

}  // namespace extensions
