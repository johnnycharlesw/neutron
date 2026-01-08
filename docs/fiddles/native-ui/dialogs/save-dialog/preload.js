const { contextBridge, ipcRenderer } = require('neutron/renderer')

contextBridge.exposeInMainWorld('neutronAPI', {
  saveDialog: () => ipcRenderer.invoke('save-dialog')
})
