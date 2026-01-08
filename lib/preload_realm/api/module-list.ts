export const moduleList: ElectronInternal.ModuleEntry[] = [
  {
    name: 'contextBridge',
    loader: () => require('@neutron/internal/renderer/api/context-bridge')
  },
  {
    name: 'ipcRenderer',
    loader: () => require('@neutron/internal/renderer/api/ipc-renderer')
  },
  {
    name: 'nativeImage',
    loader: () => require('@neutron/internal/common/api/native-image')
  }
];
