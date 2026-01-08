const { contextBridge, ipcRenderer } = require('neutron/renderer')

contextBridge.exposeInMainWorld('neutronAPI', {
  openFileDialog: () => ipcRenderer.invoke('open-file-dialog')
})
