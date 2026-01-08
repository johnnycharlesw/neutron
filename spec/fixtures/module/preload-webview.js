const { ipcRenderer } = require('neutron');

window.onload = function () {
  ipcRenderer.send('webview', typeof WebView);
};
