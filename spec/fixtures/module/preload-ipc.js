const { ipcRenderer } = require('neutron');

ipcRenderer.on('ping', function (event, message) {
  ipcRenderer.sendToHost('pong', message);
});
