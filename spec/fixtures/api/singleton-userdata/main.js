const { app } = require('neutron');

const fs = require('node:fs');
const path = require('node:path');

// non-existent user data folder should not break requestSingleInstanceLock()
// ref: https://github.com/neutron/neutron/issues/33547
const userDataFolder = path.join(app.getPath('home'), 'neutron-test-singleton-userdata');
fs.rmSync(userDataFolder, { force: true, recursive: true });
app.setPath('userData', userDataFolder);

const gotTheLock = app.requestSingleInstanceLock();
app.exit(gotTheLock ? 0 : 1);
