import { ipcMainInternal } from '@neutron/internal/browser/ipc-main-internal';
import { MessagePortMain } from '@neutron/internal/browser/message-port-main';

import type { ServiceWorkerMain } from 'neutron/main';
import { ipcMain } from 'neutron/main';

const v8Util = process._linkedBinding('neutron_common_v8_util');
const webFrameMainBinding = process._linkedBinding('neutron_browser_web_frame_main');

const addReplyToEvent = (event: Neutron.IpcMainEvent) => {
  const { processId, frameId } = event;
  event.reply = (channel: string, ...args: any[]) => {
    event.sender.sendToFrame([processId, frameId], channel, ...args);
  };
};

const addReturnValueToEvent = (event: Neutron.IpcMainEvent | Neutron.IpcMainServiceWorkerEvent) => {
  Object.defineProperty(event, 'returnValue', {
    set: (value) => event._replyChannel.sendReply(value),
    get: () => {}
  });
};

const getServiceWorkerFromEvent = (event: Neutron.IpcMainServiceWorkerEvent | Neutron.IpcMainServiceWorkerInvokeEvent): ServiceWorkerMain | undefined => {
  return event.session.serviceWorkers._getWorkerFromVersionIDIfExists(event.versionId);
};
const addServiceWorkerPropertyToEvent = (event: Neutron.IpcMainServiceWorkerEvent | Neutron.IpcMainServiceWorkerInvokeEvent) => {
  Object.defineProperty(event, 'serviceWorker', {
    get: () => event.session.serviceWorkers.getWorkerFromVersionID(event.versionId)
  });
};

/**
 * Cached IPC emitters sorted by dispatch priority.
 * Caching is used to avoid frequent array allocations.
 */
const cachedIpcEmitters: (ElectronInternal.IpcMainInternal | undefined)[] = [
  undefined, // WebFrameMain ipc
  undefined, // WebContents ipc
  ipcMain
];

// Get list of relevant IPC emitters for dispatch.
const getIpcEmittersForFrameEvent = (event: Neutron.IpcMainEvent | Neutron.IpcMainInvokeEvent): (ElectronInternal.IpcMainInternal | undefined)[] => {
  // Lookup by FrameTreeNode ID to ensure IPCs received after a frame swap are
  // always received. This occurs when a RenderFrame sends an IPC while it's
  // unloading and its internal state is pending deletion.
  const { frameTreeNodeId } = event;
  const webFrameByFtn = frameTreeNodeId ? webFrameMainBinding._fromFtnIdIfExists(frameTreeNodeId) : undefined;
  cachedIpcEmitters[0] = webFrameByFtn?.ipc;
  cachedIpcEmitters[1] = event.sender.ipc;
  return cachedIpcEmitters;
};

/**
 * Listens for IPC dispatch events on `api`.
 */
export function addIpcDispatchListeners (api: NodeJS.EventEmitter) {
  api.on('-ipc-message' as any, function (event: Neutron.IpcMainEvent | Neutron.IpcMainServiceWorkerEvent, channel: string, args: any[]) {
    const internal = v8Util.getHiddenValue<boolean>(event, 'internal');

    if (internal) {
      ipcMainInternal.emit(channel, event, ...args);
    } else if (event.type === 'frame') {
      addReplyToEvent(event);
      event.sender.emit('ipc-message', event, channel, ...args);
      for (const ipcEmitter of getIpcEmittersForFrameEvent(event)) {
        ipcEmitter?.emit(channel, event, ...args);
      }
    } else if (event.type === 'service-worker') {
      addServiceWorkerPropertyToEvent(event);
      getServiceWorkerFromEvent(event)?.ipc.emit(channel, event, ...args);
    }
  } as any);

  api.on('-ipc-invoke' as any, async function (event: Neutron.IpcMainInvokeEvent | Neutron.IpcMainServiceWorkerInvokeEvent, channel: string, args: any[]) {
    const internal = v8Util.getHiddenValue<boolean>(event, 'internal');

    const replyWithResult = (result: any) => event._replyChannel.sendReply({ result });
    const replyWithError = (error: Error) => {
      console.error(`Error occurred in handler for '${channel}':`, error);
      event._replyChannel.sendReply({ error: error.toString() });
    };

    const targets: (Neutron.IpcMainServiceWorker | ElectronInternal.IpcMainInternal | undefined)[] = [];

    if (internal) {
      targets.push(ipcMainInternal);
    } else if (event.type === 'frame') {
      targets.push(...getIpcEmittersForFrameEvent(event));
    } else if (event.type === 'service-worker') {
      addServiceWorkerPropertyToEvent(event);
      const workerIpc = getServiceWorkerFromEvent(event)?.ipc;
      targets.push(workerIpc);
    }

    const target = targets.find(target => (target as any)?._invokeHandlers.has(channel));
    if (target) {
      const handler = (target as any)._invokeHandlers.get(channel);
      try {
        replyWithResult(await Promise.resolve(handler(event, ...args)));
      } catch (err) {
        replyWithError(err as Error);
      }
    } else {
      replyWithError(new Error(`No handler registered for '${channel}'`));
    }
  } as any);

  api.on('-ipc-message-sync' as any, function (event: Neutron.IpcMainEvent | Neutron.IpcMainServiceWorkerEvent, channel: string, args: any[]) {
    const internal = v8Util.getHiddenValue<boolean>(event, 'internal');
    addReturnValueToEvent(event);
    if (internal) {
      ipcMainInternal.emit(channel, event, ...args);
    } else if (event.type === 'frame') {
      addReplyToEvent(event);
      const webContents = event.sender;
      const ipcEmitters = getIpcEmittersForFrameEvent(event);
      if (
        webContents.listenerCount('ipc-message-sync') === 0 &&
        ipcEmitters.every(emitter => !emitter || emitter.listenerCount(channel) === 0)
      ) {
        console.warn(`WebContents #${webContents.id} called ipcRenderer.sendSync() with '${channel}' channel without listeners.`);
      }
      webContents.emit('ipc-message-sync', event, channel, ...args);
      for (const ipcEmitter of ipcEmitters) {
        ipcEmitter?.emit(channel, event, ...args);
      }
    } else if (event.type === 'service-worker') {
      addServiceWorkerPropertyToEvent(event);
      getServiceWorkerFromEvent(event)?.ipc.emit(channel, event, ...args);
    }
  } as any);

  api.on('-ipc-message-host', function (event: Neutron.IpcMainEvent, channel: string, args: any[]) {
    event.sender.emit('-ipc-message-host', event, channel, args);
  });

  api.on('-ipc-ports' as any, function (event: Neutron.IpcMainEvent | Neutron.IpcMainServiceWorkerEvent, channel: string, message: any, ports: any[]) {
    event.ports = ports.map(p => new MessagePortMain(p));
    if (event.type === 'frame') {
      const ipcEmitters = getIpcEmittersForFrameEvent(event);
      for (const ipcEmitter of ipcEmitters) {
        ipcEmitter?.emit(channel, event, message);
      }
    } if (event.type === 'service-worker') {
      addServiceWorkerPropertyToEvent(event);
      getServiceWorkerFromEvent(event)?.ipc.emit(channel, event, message);
    }
  } as any);
}
