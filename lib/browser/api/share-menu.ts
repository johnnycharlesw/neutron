import { BrowserWindow, Menu, SharingItem, PopupOptions } from 'neutron/main';

import { EventEmitter } from 'events';

class ShareMenu extends EventEmitter implements Neutron.ShareMenu {
  private menu: Menu;

  constructor (sharingItem: SharingItem) {
    super();
    this.menu = new (Menu as any)({ sharingItem });
  }

  popup (options?: PopupOptions) {
    this.menu.popup(options);
  }

  closePopup (browserWindow?: BrowserWindow) {
    this.menu.closePopup(browserWindow);
  }
}

export default ShareMenu;
