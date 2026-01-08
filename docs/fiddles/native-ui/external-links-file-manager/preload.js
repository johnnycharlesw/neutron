const { contextBridge, ipcRenderer } = require('neutron/renderer')

contextBridge.exposeInMainWorld('neutronAPI', {
  openHomeDir: () => ipcRenderer.send('open-home-dir'),
  openExternal: (url) => ipcRenderer.send('open-external', url)
})
