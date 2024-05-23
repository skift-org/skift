let async = {};

onmessage = function (e) {
    const method = e.data.method;
    const ref = e.data.ref;
    const id = e.data.id;

    console.log(e);

    switch (method) {
        case "read": {
            const memory = e.data.buf;
            if (async[id] === undefined) {
                postMessage({ method: "error" });
            }
            else if (async[id] === null) {
                postMessage({ method: "read", id: id, value: null });
            } else {
                console.log("Read !");
                postMessage({ method: "read", id: id, value: async[id] });
            }
        }
        case "open": {
            async[id] = null;
            this.fetch(ref).then((response) => {
                response.arrayBuffer().then((buffer) => {
                    async[id] = buffer;
                });
            });
            break;
        }
    }
}