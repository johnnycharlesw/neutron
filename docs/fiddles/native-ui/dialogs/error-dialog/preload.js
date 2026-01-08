const { contextBridge, ipcRenderer } = require('neutron/renderer')

contextBridge.exposeInMainWorld('neutronAPI', {
  openErrorDialog: () => ipcRenderer.send('open-error-dialog')
})
