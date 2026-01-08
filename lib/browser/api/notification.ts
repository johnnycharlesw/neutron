const {
  Notification: ElectronNotification,
  isSupported
} = process._linkedBinding('neutron_browser_notification');

ElectronNotification.isSupported = isSupported;

export default ElectronNotification;
