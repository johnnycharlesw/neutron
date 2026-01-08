const { contextBridge, ipcRenderer } = require('neutron/renderer')

contextBridge.exposeInMainWorld('neutronAPI', {
  openInformationDialog: () => ipcRenderer.invoke('open-information-dialog')
})
