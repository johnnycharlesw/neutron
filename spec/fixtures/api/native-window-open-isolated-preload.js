const { ipcRenderer } = require('neutron');

window.addEventListener('message', (event) => {
  ipcRenderer.send('answer', event.data);
});
