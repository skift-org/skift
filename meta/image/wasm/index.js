var wasmObj = null;

const importObject = {
    env: {
        embedGetTimeStamp: () => {
            return BigInt(Date.now());
        },
        embedConsoleLog: (string, length) => {
            let buf = new Uint8Array(wasmObj.instance.exports.memory.buffer, string, length);
            const out = new TextDecoder("utf-8").decode(buf);
            console.log(out);
        },
        embedConsoleError: (string, length) => {
            let buf = new Uint8Array(wasmObj.instance.exports.memory.buffer, string, length);
            const out = new TextDecoder("utf-8").decode(buf);
            console.error(out);
        },
        embedAlloc: (size) => {
            new WebAssembly.Memory({initial: size});
        }
    }
};

(async () => {
    const json = await fetch("bundles/_index.json").then((response) => response.json());
    const uri = new URLSearchParams(window.location.search).get("uri");
    const bundle = json['objects'][uri];

    if (bundle['mime'] != 'application/wasm') {
        console.error(`Couldn't load bundle. Mime type is ${bundle['mime']}`);
        return;
    }

    const wasmRaw = await fetch(bundle['ref'].replace('file:/', '')).then((response) => response.arrayBuffer());
    wasmObj = await WebAssembly.instantiate(wasmRaw, importObject);
    wasmObj.instance.exports.wasm_main();
})();

281474976710661