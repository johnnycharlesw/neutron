const { ipcRenderer } = require('neutron');

window.onload = function () {
  ipcRenderer.send('answer', typeof window.process, typeof window.Buffer);
};
