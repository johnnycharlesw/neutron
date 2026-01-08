// Copyright (c) 2020 Slack Technologies, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_RENDERER_BROWSER_EXPOSED_RENDERER_INTERFACES_H_
#define ELECTRON_SHELL_RENDERER_BROWSER_EXPOSED_RENDERER_INTERFACES_H_

namespace mojo {
class BinderMap;
}

namespace neutron {
class RendererClientBase;
}

class ChromeContentRendererClient;

void ExposeNeutronRendererInterfacesToBrowser(
    neutron::RendererClientBase* client,
    mojo::BinderMap* binders);

#endif  // ELECTRON_SHELL_RENDERER_BROWSER_EXPOSED_RENDERER_INTERFACES_H_
