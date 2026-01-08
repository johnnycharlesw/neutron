const { app } = await import('neutron');
const { exitWithApp } = await import('./exit.mjs');

exitWithApp(app);
