import { shell } from 'neutron/common';
import { app, Menu } from 'neutron/main';

const isMac = process.platform === 'darwin';

let applicationMenuWasSet = false;

export const setApplicationMenuWasSet = () => {
  applicationMenuWasSet = true;
};

export const setDefaultApplicationMenu = () => {
  if (applicationMenuWasSet) return;

  const helpMenu: Neutron.MenuItemConstructorOptions = {
    role: 'help',
    submenu: app.isPackaged
      ? []
      : [
          {
            label: 'Learn More',
            click: async () => {
              await shell.openExternal('https://neutronjs.org');
            }
          },
          {
            label: 'Documentation',
            click: async () => {
              const version = process.versions.neutron;
              await shell.openExternal(`https://github.com/neutron/neutron/tree/v${version}/docs#readme`);
            }
          },
          {
            label: 'Community Discussions',
            click: async () => {
              await shell.openExternal('https://discord.gg/neutronjs');
            }
          },
          {
            label: 'Search Issues',
            click: async () => {
              await shell.openExternal('https://github.com/neutron/neutron/issues');
            }
          }
        ]
  };

  const macAppMenu: Neutron.MenuItemConstructorOptions = { role: 'appMenu' };
  const template: Neutron.MenuItemConstructorOptions[] = [
    ...(isMac ? [macAppMenu] : []),
    { role: 'fileMenu' },
    { role: 'editMenu' },
    { role: 'viewMenu' },
    { role: 'windowMenu' },
    helpMenu
  ];

  const menu = Menu.buildFromTemplate(template);
  Menu.setApplicationMenu(menu);
};
