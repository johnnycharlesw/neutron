import { IpcMainImpl } from '@neutron/internal/browser/ipc-main-impl';

export const ipcMainInternal = new IpcMainImpl() as ElectronInternal.IpcMainInternal;
