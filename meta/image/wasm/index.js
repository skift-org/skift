var wasmObj = null;
var index = null;
const worker = new Worker("worker.js");

function read_index() {
    let req = new XMLHttpRequest();
    req.open("GET", "bundles/_index.json", false);
    req.send(null);

    index = JSON.parse(req.responseText);
}

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

        embedGetCanvasWidth: () => {
            return canvas.width;
        },

        embedGetCanvasHeight: () => {
            return canvas.height;
        },

        embedGetEstimedFileSize: (uri, length) => {
            let buf = new Uint8Array(wasmObj.instance.exports.memory.buffer, uri, length);
            const data = new TextDecoder("utf-8").decode(buf);

            let obj = index['objects'][data];

            if (obj === undefined) {
                console.error(`Couldn't find object with uri ${data}`);
                return 0;
            }

            let ref = obj['ref'].replace('file:/', '');

            let req = new XMLHttpRequest();
            req.open("GET", ref, false);
            req.setRequestHeader("Range", "bytes=0-0");
            req.send(null);
            return parseInt(req.getResponseHeader("Content-Length"));
        },

        embedCheckAsync: (id, buffer, len) => {
            const workerMem = new WebAssembly.Memory({ initial: len / (64 * 1024), maximum: len / (64 * 1024) + 1, shared: true });
            const workerBuf = new Uint8Array(workerMem.buffer);
            workerBuf[0] = 0;

            worker.postMessage({ method: "read", id: id, buf: workerBuf });

            // while (workerBuf[0] == 0) {
            //     continue
            // }

            console.log("Done");

            let buf = new Uint8Array(wasmObj.instance.exports.memory.buffer, buffer, len);
            buf.set(workerBuf);

            return 1;
        },

        embedOpenFile: (uri, length) => {
            let buf = new Uint8Array(wasmObj.instance.exports.memory.buffer, uri, length);
            const data = new TextDecoder("utf-8").decode(buf);

            let obj = index['objects'][data];

            if (obj === undefined) {
                console.error(`Couldn't find object with uri ${data}`);
                return 0;
            }

            let ref = obj['ref'].replace('file:/', '');
            let id = new Date().valueOf();

            worker.postMessage({ method: "open", id: id, ref: ref });

            // queueMicrotask(() => {
            //     fetch(ref).then((response) => {
            //         response.arrayBuffer().then((buffer) => {
            //             queue[id] = buffer;
            //         });
            //     });
            // });

            // queue[id] = new Promise((resolve, _) => {
            //     fetch(ref).then((response) => {
            //         response.arrayBuffer().then((buffer) => {
            //             queue[id].value = buffer;
            //             resolve(buffer);
            //         });
            //     })
            // });

            return BigInt(id);
        },

        embedFlipCanvas: (ptr) => {
            const image = new ImageData(
                new Uint8ClampedArray(ptr, canvas.width * canvas.height * 4), canvas.width
            );

            const ctx = canvas.getContext("2d");
            ctx.putImageData(image, 0, 0);
        }
    }
};

(function () {
    var canvas = document.getElementById("canvas");
    canvas.width = document.width;
    canvas.height = document.height;
});

(async () => {
    read_index()
    const uri = new URLSearchParams(window.location.search).get("uri");
    const bundle = index['objects'][uri];

    if (bundle['mime'] != 'application/wasm') {
        console.error(`Couldn't load bundle. Mime type is ${bundle['mime']}`);
        return;
    }

    const wasmRaw = await fetch(bundle['ref'].replace('file:/', '')).then((response) => response.arrayBuffer());
    wasmObj = await WebAssembly.instantiate(wasmRaw, importObject);
    wasmObj.instance.exports.memory.grow(4096);
    wasmObj.instance.exports.wasm_main();
})();