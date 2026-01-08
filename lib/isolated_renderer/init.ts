import type * as webViewElementModule from '@neutron/internal/renderer/web-view/web-view-element';
import type { WebViewImplHooks } from '@neutron/internal/renderer/web-view/web-view-impl';

declare const isolatedApi: WebViewImplHooks;

if (isolatedApi.guestViewInternal) {
  // Must setup the WebView element in main world.
  const { setupWebView } = require('@neutron/internal/renderer/web-view/web-view-element') as typeof webViewElementModule;
  setupWebView(isolatedApi);
}
