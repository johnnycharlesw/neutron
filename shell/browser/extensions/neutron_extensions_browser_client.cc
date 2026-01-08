// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "shell/browser/extensions/neutron_extensions_browser_client.h"

#include <algorithm>
#include <utility>

#include "base/functional/bind.h"
#include "base/memory/ptr_util.h"
#include "base/path_service.h"
#include "chrome/browser/extensions/chrome_url_request_util.h"
#include "chrome/common/chrome_paths.h"
#include "components/version_info/version_info.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/frame_tree_node_id.h"
#include "content/public/browser/render_frame_host.h"
#include "extensions/browser/api/core_extensions_browser_api_provider.h"
#include "extensions/browser/api/extensions_api_client.h"
#include "extensions/browser/component_extension_resource_manager.h"
#include "extensions/browser/event_router.h"
#include "extensions/browser/extension_protocols.h"
#include "extensions/browser/extensions_browser_interface_binders.h"
#include "extensions/browser/null_app_sorting.h"
#include "extensions/browser/safe_browsing_delegate.h"
#include "extensions/browser/updater/null_extension_cache.h"
#include "extensions/browser/url_request_util.h"
#include "extensions/common/features/feature_channel.h"
#include "extensions/common/file_util.h"
#include "extensions/common/manifest_constants.h"
#include "extensions/common/manifest_handlers/chrome_url_overrides_handler.h"
#include "extensions/common/manifest_handlers/devtools_page_handler.h"
#include "extensions/common/manifest_url_handlers.h"
#include "services/network/public/mojom/url_loader.mojom.h"
#include "shell/browser/browser.h"
#include "shell/browser/neutron_browser_client.h"
#include "shell/browser/neutron_browser_context.h"
#include "shell/browser/extensions/api/runtime/neutron_runtime_api_delegate.h"
#include "shell/browser/extensions/neutron_component_extension_resource_manager.h"
#include "shell/browser/extensions/neutron_extension_host_delegate.h"
#include "shell/browser/extensions/neutron_extension_system_factory.h"
#include "shell/browser/extensions/neutron_extension_web_contents_observer.h"
#include "shell/browser/extensions/neutron_extensions_api_client.h"
#include "shell/browser/extensions/neutron_extensions_browser_api_provider.h"
#include "shell/browser/extensions/neutron_kiosk_delegate.h"
#include "shell/browser/extensions/neutron_navigation_ui_data.h"
#include "shell/browser/extensions/neutron_process_manager_delegate.h"
#include "ui/base/resource/resource_bundle.h"

using content::BrowserContext;
using content::BrowserThread;
using extensions::ExtensionsBrowserClient;

namespace neutron {

NeutronExtensionsBrowserClient::NeutronExtensionsBrowserClient()
    : extension_cache_(std::make_unique<extensions::NullExtensionCache>()),
      safe_browsing_delegate_(
          std::make_unique<extensions::SafeBrowsingDelegate>()) {
  AddAPIProvider(
      std::make_unique<extensions::CoreExtensionsBrowserAPIProvider>());
  AddAPIProvider(
      std::make_unique<extensions::NeutronExtensionsBrowserAPIProvider>());

  // Neutron does not have a concept of channel, so leave UNKNOWN to
  // enable all channel-dependent extension APIs.
  extensions::SetCurrentChannel(version_info::Channel::UNKNOWN);
}

NeutronExtensionsBrowserClient::~NeutronExtensionsBrowserClient() = default;

void NeutronExtensionsBrowserClient::Init() {
  process_manager_delegate_ =
      std::make_unique<extensions::NeutronProcessManagerDelegate>();
  api_client_ = std::make_unique<extensions::NeutronExtensionsAPIClient>();
  resource_manager_ =
      std::make_unique<extensions::NeutronComponentExtensionResourceManager>();
}

bool NeutronExtensionsBrowserClient::IsShuttingDown() {
  return neutron::Browser::Get()->is_shutting_down();
}

bool NeutronExtensionsBrowserClient::AreExtensionsDisabled(
    const base::CommandLine& command_line,
    BrowserContext* context) {
  return false;
}

bool NeutronExtensionsBrowserClient::IsValidContext(void* context) {
  return NeutronBrowserContext::IsValidContext(context);
}

bool NeutronExtensionsBrowserClient::IsSameContext(BrowserContext* first,
                                                    BrowserContext* second) {
  return first == second;
}

bool NeutronExtensionsBrowserClient::HasOffTheRecordContext(
    BrowserContext* context) {
  return false;
}

BrowserContext* NeutronExtensionsBrowserClient::GetOffTheRecordContext(
    BrowserContext* context) {
  // app_shell only supports a single context.
  return nullptr;
}

BrowserContext* NeutronExtensionsBrowserClient::GetOriginalContext(
    BrowserContext* context) {
  DCHECK(context);
  if (context->IsOffTheRecord()) {
    return NeutronBrowserContext::GetDefaultBrowserContext();
  } else {
    return context;
  }
}

content::BrowserContext*
NeutronExtensionsBrowserClient::GetContextRedirectedToOriginal(
    content::BrowserContext* context) {
  return GetOriginalContext(context);
}

content::BrowserContext* NeutronExtensionsBrowserClient::GetContextOwnInstance(
    content::BrowserContext* context) {
  return context;
}

content::BrowserContext*
NeutronExtensionsBrowserClient::GetContextForOriginalOnly(
    content::BrowserContext* context) {
  return context->IsOffTheRecord() ? nullptr : context;
}

bool NeutronExtensionsBrowserClient::AreExtensionsDisabledForContext(
    content::BrowserContext* context) {
  return false;
}

bool NeutronExtensionsBrowserClient::IsGuestSession(
    BrowserContext* context) const {
  return false;
}

bool NeutronExtensionsBrowserClient::IsExtensionIncognitoEnabled(
    const std::string& extension_id,
    content::BrowserContext* context) const {
  return false;
}

bool NeutronExtensionsBrowserClient::CanExtensionCrossIncognito(
    const extensions::Extension* extension,
    content::BrowserContext* context) const {
  return false;
}

base::FilePath NeutronExtensionsBrowserClient::GetBundleResourcePath(
    const network::ResourceRequest& request,
    const base::FilePath& extension_resources_path,
    int* resource_id) const {
  *resource_id = 0;
  base::FilePath chrome_resources_path;
  if (!base::PathService::Get(chrome::DIR_RESOURCES, &chrome_resources_path))
    return {};

  // Since component extension resources are included in
  // component_extension_resources.pak file in |chrome_resources_path|,
  // calculate the extension |request_relative_path| against
  // |chrome_resources_path|.
  if (!chrome_resources_path.IsParent(extension_resources_path))
    return {};

  base::FilePath request_relative_path =
      extensions::file_util::ExtensionURLToRelativeFilePath(request.url);
  if (!ExtensionsBrowserClient::Get()
           ->GetComponentExtensionResourceManager()
           ->IsComponentExtensionResource(extension_resources_path,
                                          request_relative_path, resource_id)) {
    return {};
  }
  DCHECK_NE(0, *resource_id);

  return request_relative_path;
}

void NeutronExtensionsBrowserClient::LoadResourceFromResourceBundle(
    const network::ResourceRequest& request,
    mojo::PendingReceiver<network::mojom::URLLoader> loader,
    const base::FilePath& resource_relative_path,
    int resource_id,
    scoped_refptr<net::HttpResponseHeaders> headers,
    mojo::PendingRemote<network::mojom::URLLoaderClient> client) {
  extensions::chrome_url_request_util::LoadResourceFromResourceBundle(
      request, std::move(loader), resource_relative_path, resource_id,
      std::move(headers), std::move(client));
}

namespace {
bool AllowCrossRendererResourceLoad(
    const network::ResourceRequest& request,
    network::mojom::RequestDestination destination,
    ui::PageTransition page_transition,
    int child_id,
    bool is_incognito,
    const extensions::Extension* extension,
    const extensions::ExtensionSet& extensions,
    const extensions::ProcessMap& process_map,
    const GURL& upstream_url,
    bool* allowed) {
  if (extensions::url_request_util::AllowCrossRendererResourceLoad(
          request, destination, page_transition, child_id, is_incognito,
          extension, extensions, process_map, upstream_url, allowed)) {
    return true;
  }

  // If there aren't any explicitly marked web accessible resources, the
  // load should be allowed only if it is by DevTools. A close approximation is
  // checking if the extension contains a DevTools page.
  if (extension && !extensions::chrome_manifest_urls::GetDevToolsPage(extension)
                        .is_empty()) {
    *allowed = true;
    return true;
  }

  // Couldn't determine if the resource is allowed or not.
  return false;
}
}  // namespace

bool NeutronExtensionsBrowserClient::AllowCrossRendererResourceLoad(
    const network::ResourceRequest& request,
    network::mojom::RequestDestination destination,
    ui::PageTransition page_transition,
    int child_id,
    bool is_incognito,
    const extensions::Extension* extension,
    const extensions::ExtensionSet& extensions,
    const extensions::ProcessMap& process_map,
    const GURL& upstream_url) {
  bool allowed = false;
  if (::neutron::AllowCrossRendererResourceLoad(
          request, destination, page_transition, child_id, is_incognito,
          extension, extensions, process_map, upstream_url, &allowed)) {
    return allowed;
  }

  // Couldn't determine if resource is allowed. Block the load.
  return false;
}

PrefService* NeutronExtensionsBrowserClient::GetPrefServiceForContext(
    BrowserContext* context) {
  return static_cast<NeutronBrowserContext*>(context)->prefs();
}

void NeutronExtensionsBrowserClient::GetEarlyExtensionPrefsObservers(
    content::BrowserContext* context,
    std::vector<extensions::EarlyExtensionPrefsObserver*>* observers) const {}

extensions::ProcessManagerDelegate*
NeutronExtensionsBrowserClient::GetProcessManagerDelegate() const {
  return process_manager_delegate_.get();
}

mojo::PendingRemote<network::mojom::URLLoaderFactory>
NeutronExtensionsBrowserClient::GetControlledFrameEmbedderURLLoader(
    const url::Origin& app_origin,
    content::FrameTreeNodeId frame_tree_node_id,
    content::BrowserContext* browser_context) {
  return mojo::PendingRemote<network::mojom::URLLoaderFactory>();
}

std::unique_ptr<extensions::ExtensionHostDelegate>
NeutronExtensionsBrowserClient::
    CreateExtensionHostDelegate() {  // TODO(samuelmaddock):
  return std::make_unique<extensions::NeutronExtensionHostDelegate>();
}

bool NeutronExtensionsBrowserClient::DidVersionUpdate(
    BrowserContext* context) {
  // TODO(jamescook): We might want to tell extensions when app_shell updates.
  return false;
}

void NeutronExtensionsBrowserClient::PermitExternalProtocolHandler() {}

bool NeutronExtensionsBrowserClient::IsInDemoMode() {
  return false;
}

bool NeutronExtensionsBrowserClient::IsScreensaverInDemoMode(
    const std::string& app_id) {
  return false;
}

bool NeutronExtensionsBrowserClient::IsRunningInForcedAppMode() {
  return false;
}

bool NeutronExtensionsBrowserClient::IsAppModeForcedForApp(
    const extensions::ExtensionId& extension_id) {
  return false;
}

bool NeutronExtensionsBrowserClient::IsLoggedInAsPublicAccount() {
  return false;
}

extensions::ExtensionSystemProvider*
NeutronExtensionsBrowserClient::GetExtensionSystemFactory() {
  return extensions::NeutronExtensionSystemFactory::GetInstance();
}

std::unique_ptr<extensions::RuntimeAPIDelegate>
NeutronExtensionsBrowserClient::CreateRuntimeAPIDelegate(
    content::BrowserContext* context) const {
  return std::make_unique<extensions::NeutronRuntimeAPIDelegate>(context);
}

const extensions::ComponentExtensionResourceManager*
NeutronExtensionsBrowserClient::GetComponentExtensionResourceManager() {
  return resource_manager_.get();
}

void NeutronExtensionsBrowserClient::BroadcastEventToRenderers(
    extensions::events::HistogramValue histogram_value,
    const std::string& event_name,
    base::Value::List args,
    bool dispatch_to_off_the_record_profiles) {
  if (!BrowserThread::CurrentlyOn(BrowserThread::UI)) {
    content::GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE,
        base::BindOnce(
            &NeutronExtensionsBrowserClient::BroadcastEventToRenderers,
            base::Unretained(this), histogram_value, event_name,
            std::move(args), dispatch_to_off_the_record_profiles));
    return;
  }

  for (auto* browser_context : NeutronBrowserContext::BrowserContexts()) {
    extensions::EventRouter::Get(browser_context)
        ->BroadcastEvent(std::make_unique<extensions::Event>(
            histogram_value, event_name, args.Clone()));
  }
}

extensions::ExtensionCache*
NeutronExtensionsBrowserClient::GetExtensionCache() {
  return extension_cache_.get();
}

bool NeutronExtensionsBrowserClient::IsBackgroundUpdateAllowed() {
  return true;
}

bool NeutronExtensionsBrowserClient::IsMinBrowserVersionSupported(
    const std::string& min_version) {
  return true;
}

void NeutronExtensionsBrowserClient::SetAPIClientForTest(
    extensions::ExtensionsAPIClient* api_client) {
  api_client_.reset(api_client);
}

void NeutronExtensionsBrowserClient::CreateExtensionWebContentsObserver(
    content::WebContents* web_contents) {
  extensions::NeutronExtensionWebContentsObserver::CreateForWebContents(
      web_contents);
}

extensions::ExtensionWebContentsObserver*
NeutronExtensionsBrowserClient::GetExtensionWebContentsObserver(
    content::WebContents* web_contents) {
  return extensions::NeutronExtensionWebContentsObserver::FromWebContents(
      web_contents);
}

extensions::KioskDelegate* NeutronExtensionsBrowserClient::GetKioskDelegate() {
  if (!kiosk_delegate_)
    kiosk_delegate_ = std::make_unique<NeutronKioskDelegate>();
  return kiosk_delegate_.get();
}

extensions::SafeBrowsingDelegate*
NeutronExtensionsBrowserClient::GetSafeBrowsingDelegate() {
  return safe_browsing_delegate_.get();
}

std::string NeutronExtensionsBrowserClient::GetApplicationLocale() {
  return NeutronBrowserClient::Get()->GetApplicationLocale();
}

void NeutronExtensionsBrowserClient::RegisterBrowserInterfaceBindersForFrame(
    mojo::BinderMapWithContext<content::RenderFrameHost*>* map,
    content::RenderFrameHost* render_frame_host,
    const extensions::Extension* extension) const {
  PopulateExtensionFrameBinders(map, render_frame_host, extension);
}

}  // namespace neutron
