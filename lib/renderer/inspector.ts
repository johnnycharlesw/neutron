import { IPC_MESSAGES } from '@neutron/internal/common/ipc-messages';
import { internalContextBridge } from '@neutron/internal/renderer/api/context-bridge';
import { ipcRendererInternal } from '@neutron/internal/renderer/ipc-renderer-internal';
import * as ipcRendererUtils from '@neutron/internal/renderer/ipc-renderer-internal-utils';

import { webFrame } from 'neutron/renderer';

const { contextIsolationEnabled } = internalContextBridge;

/* Corrects for some Inspector adaptations needed in Neutron.
* 1) Use menu API to show context menu.
*/
window.onload = function () {
  if (contextIsolationEnabled) {
    internalContextBridge.overrideGlobalValueFromIsolatedWorld([
      'InspectorFrontendHost', 'showContextMenuAtPoint'
    ], createMenu);
  } else {
    window.InspectorFrontendHost!.showContextMenuAtPoint = createMenu;
  }
};

// The DOM implementation expects (message?: string) => boolean
window.confirm = function (message?: string, title?: string) {
  return ipcRendererUtils.invokeSync(IPC_MESSAGES.INSPECTOR_CONFIRM, message, title) as boolean;
};

const useEditMenuItems = function (x: number, y: number, items: ContextMenuItem[]) {
  return items.length === 0 && document.elementsFromPoint(x, y).some(element => {
    return element.nodeName === 'INPUT' ||
      element.nodeName === 'TEXTAREA' ||
      (element as HTMLElement).isContentEditable;
  });
};

const createMenu = function (x: number, y: number, items: ContextMenuItem[]) {
  const isEditMenu = useEditMenuItems(x, y, items);
  ipcRendererInternal.invoke<number>(IPC_MESSAGES.INSPECTOR_CONTEXT_MENU, items, isEditMenu).then(id => {
    if (typeof id === 'number') {
      webFrame.executeJavaScript(`window.DevToolsAPI.contextMenuItemSelected(${JSON.stringify(id)})`);
    }

    webFrame.executeJavaScript('window.DevToolsAPI.contextMenuCleared()');
  });
};
