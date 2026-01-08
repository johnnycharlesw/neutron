const { contextBridge, ipcRenderer } = require('neutron/renderer')

contextBridge.exposeInMainWorld('neutronAPI', {
  getAppPath: () => ipcRenderer.invoke('get-app-path')
})
