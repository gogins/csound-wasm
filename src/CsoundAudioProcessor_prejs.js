/// Module['ENIVIRONMENT']='NODE';
// Polyfill for change in Emscripten target behavior:
if (typeof performance === 'undefined' || typeof performance.now !== 'function') {
  globalThis.performance = {
    now: () => Date.now()
  };
}