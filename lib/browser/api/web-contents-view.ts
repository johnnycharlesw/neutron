import { View } from 'neutron/main';

const { WebContentsView } = process._linkedBinding('neutron_browser_web_contents_view');

Object.setPrototypeOf(WebContentsView.prototype, View.prototype);

export default WebContentsView;
