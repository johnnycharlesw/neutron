import { MessagePortMain } from '@neutron/internal/browser/message-port-main';

import { EventEmitter } from 'events';

const { createPair } = process._linkedBinding('neutron_browser_message_port');

export default class MessageChannelMain extends EventEmitter implements Neutron.MessageChannelMain {
  port1: MessagePortMain;
  port2: MessagePortMain;
  constructor () {
    super();
    const { port1, port2 } = createPair();
    this.port1 = new MessagePortMain(port1);
    this.port2 = new MessagePortMain(port2);
  }
}
