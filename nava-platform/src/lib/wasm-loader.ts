/**
 * Nava Sanskritam WASM Loader
 * Optimized for Next.js and Cloudflare Edge
 */

let compilerInstance: any = null;
let initializationPromise: Promise<any> | null = null;

export async function loadNavaCompiler() {
  if (typeof window === 'undefined') return null; // Only run on client

  if (compilerInstance) return compilerInstance;
  if (initializationPromise) return initializationPromise;

  initializationPromise = new Promise((resolve, reject) => {
    // Check if the global constructor exists (loaded via script or dynamic import)
    const script = document.createElement('script');
    script.src = '/nvc.js';
    script.async = true;
    
    script.onload = () => {
      // @ts-ignore - 'NavaCompiler' is defined by Emscripten glue code
      if (typeof window.NavaCompiler === 'function') {
        // @ts-ignore
        window.NavaCompiler().then((instance: any) => {
          compilerInstance = {
            compile: (code: string) => instance.cwrap('compileSanskrit', 'string', ['string'])(code),
            instance: instance
          };
          resolve(compilerInstance);
        });
      } else {
        reject(new Error("NavaCompiler constructor not found in nvc.js"));
      }
    };

    script.onerror = () => reject(new Error("Failed to load nvc.js"));
    document.body.appendChild(script);
  });

  return initializationPromise;
}
