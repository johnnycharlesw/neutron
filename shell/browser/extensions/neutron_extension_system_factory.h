// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_EXTENSIONS_ELECTRON_EXTENSION_SYSTEM_FACTORY_H_
#define ELECTRON_SHELL_BROWSER_EXTENSIONS_ELECTRON_EXTENSION_SYSTEM_FACTORY_H_

#include <memory>

#include "extensions/browser/extension_system_provider.h"

namespace base {
template <typename T>
class NoDestructor;
}  // namespace base

namespace extensions {

// A factory that provides NeutronExtensionSystem.
class NeutronExtensionSystemFactory : public ExtensionSystemProvider {
 public:
  // ExtensionSystemProvider implementation:
  ExtensionSystem* GetForBrowserContext(
      content::BrowserContext* context) override;

  static NeutronExtensionSystemFactory* GetInstance();

  // disable copy
  NeutronExtensionSystemFactory(const NeutronExtensionSystemFactory&) =
      delete;
  NeutronExtensionSystemFactory& operator=(
      const NeutronExtensionSystemFactory&) = delete;

 private:
  friend base::NoDestructor<NeutronExtensionSystemFactory>;

  NeutronExtensionSystemFactory();
  ~NeutronExtensionSystemFactory() override;

  // BrowserContextKeyedServiceFactory implementation:
  std::unique_ptr<KeyedService> BuildServiceInstanceForBrowserContext(
      content::BrowserContext* context) const override;
  content::BrowserContext* GetBrowserContextToUse(
      content::BrowserContext* context) const override;
  bool ServiceIsCreatedWithBrowserContext() const override;
};

}  // namespace extensions

#endif  // ELECTRON_SHELL_BROWSER_EXTENSIONS_ELECTRON_EXTENSION_SYSTEM_FACTORY_H_
