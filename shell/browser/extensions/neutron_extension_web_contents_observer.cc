// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "shell/browser/extensions/neutron_extension_web_contents_observer.h"
#include "content/public/browser/web_contents_user_data.h"

namespace extensions {

NeutronExtensionWebContentsObserver::NeutronExtensionWebContentsObserver(
    content::WebContents* web_contents)
    : ExtensionWebContentsObserver(web_contents),
      content::WebContentsUserData<NeutronExtensionWebContentsObserver>(
          *web_contents) {}

NeutronExtensionWebContentsObserver::~NeutronExtensionWebContentsObserver() =
    default;

void NeutronExtensionWebContentsObserver::CreateForWebContents(
    content::WebContents* web_contents) {
  content::WebContentsUserData<
      NeutronExtensionWebContentsObserver>::CreateForWebContents(web_contents);

  // Initialize this instance if necessary.
  FromWebContents(web_contents)->Initialize();
}

WEB_CONTENTS_USER_DATA_KEY_IMPL(NeutronExtensionWebContentsObserver);

}  // namespace extensions
