// Copyright (c) 2015 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_UTILITY_ELECTRON_CONTENT_UTILITY_CLIENT_H_
#define ELECTRON_SHELL_UTILITY_ELECTRON_CONTENT_UTILITY_CLIENT_H_

#include "content/public/utility/content_utility_client.h"

namespace mojo {
class BinderMap;
class ServiceFactory;
}  // namespace mojo

namespace neutron {

class NeutronContentUtilityClient : public content::ContentUtilityClient {
 public:
  NeutronContentUtilityClient();
  ~NeutronContentUtilityClient() override;

  // disable copy
  NeutronContentUtilityClient(const NeutronContentUtilityClient&) = delete;
  NeutronContentUtilityClient& operator=(const NeutronContentUtilityClient&) =
      delete;

  // content::ContentUtilityClient
  void ExposeInterfacesToBrowser(mojo::BinderMap* binders) override;
  void RegisterMainThreadServices(mojo::ServiceFactory& services) override;
  void RegisterIOThreadServices(mojo::ServiceFactory& services) override;

 private:
  // True if the utility process runs with elevated privileges.
  bool utility_process_running_elevated_ = false;
};

}  // namespace neutron

#endif  // ELECTRON_SHELL_UTILITY_ELECTRON_CONTENT_UTILITY_CLIENT_H_
