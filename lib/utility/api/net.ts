import { fetchWithSession } from '@neutron/internal/browser/api/net-fetch';
import { ClientRequest } from '@neutron/internal/common/api/net-client-request';

import type { ClientRequestConstructorOptions, IncomingMessage } from 'neutron/utility';

const { isOnline, resolveHost } = process._linkedBinding('neutron_common_net');

export function request (options: ClientRequestConstructorOptions | string, callback?: (message: IncomingMessage) => void) {
  return new ClientRequest(options, callback);
}

export function fetch (input: RequestInfo, init?: RequestInit): Promise<Response> {
  return fetchWithSession(input, init, undefined, request);
}

exports.resolveHost = resolveHost;

exports.isOnline = isOnline;

Object.defineProperty(exports, 'online', {
  get: () => isOnline()
});
