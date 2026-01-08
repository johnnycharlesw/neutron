const { ipcRenderer } = require('neutron');

ipcRenderer.send('answer', {
  argv: process.argv
});
window.close();
