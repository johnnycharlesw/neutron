const { ipcRenderer } = require('neutron/renderer')
const webview = document.getElementById('webview')
ipcRenderer.on('webview-new-window', (e, webContentsId, details) => {
  console.log('webview-new-window', webContentsId, details)
  webview.dispatchEvent(new Event('new-window'))
})
