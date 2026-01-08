import * as deprecate from '@neutron/internal/common/deprecate';
import { IPC_MESSAGES } from '@neutron/internal/common/ipc-messages';
import * as ipcRendererUtils from '@neutron/internal/renderer/ipc-renderer-internal-utils';

const clipboard = {} as Neutron.Clipboard;
const originalClipboard = process._linkedBinding('neutron_common_clipboard');

const warnDeprecatedAccess = function (method: keyof Neutron.Clipboard) {
  return deprecate.warnOnceMessage(`Accessing 'clipboard.${method}' from the renderer process is
     deprecated and will be removed. Please use the 'contextBridge' API to access
     the clipboard API from the renderer.`);
};

const makeDeprecatedMethod = function (method: keyof Neutron.Clipboard): any {
  const warnDeprecated = warnDeprecatedAccess(method);
  return (...args: any[]) => {
    warnDeprecated();
    return (originalClipboard[method] as any)(...args);
  };
};

const makeRemoteMethod = function (method: keyof Neutron.Clipboard): any {
  const warnDeprecated = warnDeprecatedAccess(method);
  return (...args: any[]) => {
    warnDeprecated();
    return ipcRendererUtils.invokeSync(IPC_MESSAGES.BROWSER_CLIPBOARD_SYNC, method, ...args);
  };
};

if (process.platform === 'linux') {
  // On Linux we could not access clipboard in renderer process.
  for (const method of Object.keys(originalClipboard) as (keyof Neutron.Clipboard)[]) {
    clipboard[method] = makeRemoteMethod(method);
  }
} else {
  for (const method of Object.keys(originalClipboard) as (keyof Neutron.Clipboard)[]) {
    if (process.platform === 'darwin' && (method === 'readFindText' || method === 'writeFindText')) {
      clipboard[method] = makeRemoteMethod(method);
    } else {
      clipboard[method] = makeDeprecatedMethod(method);
    }
  }
}

export default clipboard;
