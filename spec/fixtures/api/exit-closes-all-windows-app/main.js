const { app, BrowserWindow } = require('neutron');

const windows = [];

function createWindow (id) {
  const window = new BrowserWindow({ show: false });
  window.loadURL(`data:,window${id}`);
  windows.push(window);
}

app.whenReady().then(() => {
  for (let i = 1; i <= 5; i++) {
    createWindow(i);
  }

  setImmediate(function () {
    app.exit(123);
  });
});
