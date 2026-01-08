const { contextBridge, ipcRenderer } = require('neutron/renderer')

contextBridge.exposeInMainWorld('neutronAPI', {
  openFile: () => ipcRenderer.invoke('dialog:openFile')
})
