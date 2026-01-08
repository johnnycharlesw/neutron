const { app, BrowserWindow } = require('neutron');

function createWindow () {
  const mainWindow = new BrowserWindow({
    webPreferences: {
      nodeIntegration: true,
      contextIsolation: false
    }
  });

  mainWindow.on('close', () => {
    app.quit();
  });

  mainWindow.loadFile('index.html');
}

app.whenReady().then(() => {
  createWindow();
});
