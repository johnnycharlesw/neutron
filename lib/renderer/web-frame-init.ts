import { IPC_MESSAGES } from '@neutron/internal/common/ipc-messages';
import * as ipcRendererUtils from '@neutron/internal/renderer/ipc-renderer-internal-utils';

import { webFrame, WebFrame } from 'neutron/renderer';

// All keys of WebFrame that extend Function
type WebFrameMethod = {
  [K in keyof WebFrame]:
    WebFrame[K] extends Function ? K : never
}

export const webFrameInit = () => {
  // Call webFrame method
  ipcRendererUtils.handle(IPC_MESSAGES.RENDERER_WEB_FRAME_METHOD, (
    event, method: keyof WebFrameMethod, ...args: any[]
  ) => {
    // The TypeScript compiler cannot handle the sheer number of
    // call signatures here and simply gives up. Incorrect invocations
    // will be caught by "keyof WebFrameMethod" though.
    return (webFrame[method] as any)(...args);
  });
};
