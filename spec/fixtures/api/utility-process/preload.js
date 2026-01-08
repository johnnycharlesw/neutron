const { ipcRenderer } = require('neutron');

ipcRenderer.on('port', (e, msg) => {
  e.ports[0].postMessage(msg);
});
