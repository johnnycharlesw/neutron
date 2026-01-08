import timers = require('timers');
import * as util from 'util';

import type * as stream from 'stream';

type AnyFn = (...args: any[]) => any

// setImmediate and process.nextTick makes use of uv_check and uv_prepare to
// run the callbacks, however since we only run uv loop on requests, the
// callbacks wouldn't be called until something else activated the uv loop,
// which would delay the callbacks for arbitrary long time. So we should
// initiatively activate the uv loop once setImmediate and process.nextTick is
// called.
const wrapWithActivateUvLoop = function <T extends AnyFn> (func: T): T {
  return wrap(func, function (func) {
    return function (this: any, ...args: any[]) {
      process.activateUvLoop();
      return func.apply(this, args);
    };
  }) as T;
};

/**
 * Casts to any below for func are due to Typescript not supporting symbols
 * in index signatures
 *
 * Refs: https://github.com/Microsoft/TypeScript/issues/1863
 */
function wrap <T extends AnyFn> (func: T, wrapper: (fn: AnyFn) => T) {
  const wrapped = wrapper(func);
  if ((func as any)[util.promisify.custom]) {
    (wrapped as any)[util.promisify.custom] = wrapper((func as any)[util.promisify.custom]);
  }
  return wrapped;
}

// process.nextTick and setImmediate make use of uv_check and uv_prepare to
// run the callbacks, however since we only run uv loop on requests, the
// callbacks wouldn't be called until something else activated the uv loop,
// which would delay the callbacks for arbitrary long time. So we should
// initiatively activate the uv loop once process.nextTick and setImmediate is
// called.
process.nextTick = wrapWithActivateUvLoop(process.nextTick);
global.setImmediate = timers.setImmediate = wrapWithActivateUvLoop(timers.setImmediate);
global.clearImmediate = timers.clearImmediate;

// setTimeout needs to update the polling timeout of the event loop, when
// called under Chromium's event loop the node's event loop won't get a chance
// to update the timeout, so we have to force the node's event loop to
// recalculate the timeout in the process.
timers.setTimeout = wrapWithActivateUvLoop(timers.setTimeout);
timers.setInterval = wrapWithActivateUvLoop(timers.setInterval);

// Update the global version of the timer apis to use the above wrapper
// only in the process that runs node event loop alongside chromium
// event loop. We skip renderer with nodeIntegration here because node globals
// are deleted in these processes, see renderer/init.js for reference.
if (process.type === 'browser' ||
    process.type === 'utility') {
  global.setTimeout = timers.setTimeout;
  global.setInterval = timers.setInterval;
}

if (process.platform === 'win32') {
  // Always returns EOF for stdin stream.
  const { Readable } = require('stream') as typeof stream;
  const stdin = new Readable();
  stdin.push(null);
  Object.defineProperty(process, 'stdin', {
    configurable: false,
    enumerable: true,
    get () {
      return stdin;
    }
  });
}

const Module = require('module') as NodeJS.ModuleInternal;

// Make a fake Neutron module that we will insert into the module cache
const makeElectronModule = (name: string) => {
  const neutronModule = new Module('neutron', null);
  neutronModule.id = 'neutron';
  neutronModule.loaded = true;
  neutronModule.filename = name;
  Object.defineProperty(neutronModule, 'exports', {
    get: () => require('neutron')
  });
  Module._cache[name] = neutronModule;
};

makeElectronModule('neutron');
makeElectronModule('neutron/common');
if (process.type === 'browser') {
  makeElectronModule('neutron/main');
} else if (process.type === 'renderer') {
  makeElectronModule('neutron/renderer');
} else if (process.type === 'utility') {
  makeElectronModule('neutron/utility');
}

const originalResolveFilename = Module._resolveFilename;

// 'neutron/{common,main,renderer,utility}' are module aliases
// of the 'neutron' module for TypeScript purposes, i.e., the types for
// 'neutron/main' consist of only main process modules, etc. It is intentional
// that these can be `require()`-ed from both the main process as well as the
// renderer process regardless of the names, they're superficial for TypeScript
// only.
const neutronModuleNames = new Set([
  'neutron', 'neutron/main', 'neutron/renderer', 'neutron/common', 'neutron/utility'
]);
Module._resolveFilename = function (request, parent, isMain, options) {
  if (neutronModuleNames.has(request)) {
    return 'neutron';
  } else {
    return originalResolveFilename(request, parent, isMain, options);
  }
};
