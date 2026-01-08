// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "shell/renderer/extensions/neutron_extensions_renderer_client.h"

#include "content/public/renderer/render_thread.h"
#include "extensions/common/constants.h"
#include "extensions/common/manifest_handlers/background_info.h"
#include "extensions/renderer/dispatcher.h"
#include "shell/common/world_ids.h"

namespace neutron {

NeutronExtensionsRendererClient::NeutronExtensionsRendererClient() {}

NeutronExtensionsRendererClient::~NeutronExtensionsRendererClient() = default;

bool NeutronExtensionsRendererClient::IsIncognitoProcess() const {
  // app_shell doesn't support off-the-record contexts.
  return false;
}

int NeutronExtensionsRendererClient::GetLowestIsolatedWorldId() const {
  return WorldIDs::ISOLATED_WORLD_ID_EXTENSIONS;
}

bool NeutronExtensionsRendererClient::AllowPopup() {
  // TODO(samuelmaddock):
  return false;
}

void NeutronExtensionsRendererClient::RunScriptsAtDocumentStart(
    content::RenderFrame* render_frame) {
  dispatcher()->RunScriptsAtDocumentStart(render_frame);
}

void NeutronExtensionsRendererClient::RunScriptsAtDocumentEnd(
    content::RenderFrame* render_frame) {
  dispatcher()->RunScriptsAtDocumentEnd(render_frame);
}

void NeutronExtensionsRendererClient::RunScriptsAtDocumentIdle(
    content::RenderFrame* render_frame) {
  dispatcher()->RunScriptsAtDocumentIdle(render_frame);
}

}  // namespace neutron
