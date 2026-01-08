const { ipcRenderer } = require('neutron');

window.onload = function () {
  ipcRenderer.send('answer', process.argv);
};
