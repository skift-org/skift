import logging
import magic
import json

from pathlib import Path
from typing import Any
from cutekit import model, builder, shell
from .store import Storage


class Image:
    _registry: model.Registry
    _store: Storage
    _logger: logging.Logger
    _paks: dict[str, dict[str, Any]]
    _finalized: str | None

    def __init__(self, registry: model.Registry, store: Storage):
        self._registry = registry
        self._store = store
        self._logger = logging.getLogger("Image")
        self._paks = {}
        self._finalized = None

    def ensurePak(self, id: str) -> dict[str, Any]:
        if id not in self._paks:
            self._paks[id] = {"id": id, "objects": {}}
        return self._paks[id]

    def cpRef(self, pak: str, src: str, id: str) -> str:
        ref = f"bundle://{id}"
        sha256 = shell.sha256sum(src)
        dest = f"objects/{sha256}"
        mime = magic.from_file(src, mime=True)
        self.ensurePak(pak)["objects"][ref] = {
            "mime": mime,
            "ref": f"file:/{dest}",
        }
        self._store.store(src, dest)
        return dest

    def installTo(self, componentSpec: str, targetSpec: str, dest: str):
        product = self.install(componentSpec, targetSpec)[0]
        self.cp(str(product.path), dest=dest)

    def _installProduct(self, product: builder.ProductScope):
        component = product.component
        target = product.target

        for depId in component.resolved[target.id].required + [component.id]:
            dep = self._registry.lookup(depId, model.Component)
            if dep is None:
                raise Exception(f"Component {depId} not found")

            for res in builder.listRes(dep):
                rel = Path(res).relative_to(dep.subpath("res"))
                self.cpRef("_index", res, f"{depId}/{rel}")
                self.cpRef(component.id, res, f"{depId}/{rel}")

        if component.type == model.Kind.EXE:
            self.cpRef("_index", str(product.path), f"{component.id}/_bin")
            self.cpRef(component.id, str(product.path), f"{component.id}/_bin")
        else:
            self.cpRef("_index", str(product.path), f"{component.id}/_lib")
            self.cpRef(component.id, str(product.path), f"{component.id}/_lib")

    def install(
        self, componentsSpec: str | list[str], targetSpec: str
    ) -> list[builder.ProductScope]:
        if isinstance(componentsSpec, str):
            componentsSpec = [componentsSpec]

        self._logger.info(f"Installing {componentsSpec}...")
        components = [self._registry.lookup(c, model.Component) for c in componentsSpec]

        target = self._registry.lookup(targetSpec, model.Target)
        assert target is not None

        scope = builder.TargetScope(self._registry, target)
        products = builder.build(scope, components)

        for p in products:
            self._installProduct(p)

        return products

    def installAll(self, targetSpec: str) -> list[builder.ProductScope]:
        target = self._registry.lookup(targetSpec, model.Target)
        assert target is not None

        scope = builder.TargetScope(self._registry, target)
        products = builder.build(scope, "all")
        for product in products:
            self._installProduct(product)

        return products

    def cp(self, src: str, dest: str):
        self._logger.info(f"Copying {src} to {dest}...")
        self._store.store(src, dest)

    def cpTree(self, src: str, dest: str):
        self._logger.info(f"Copying {src} to {dest}...")
        self._store.store(src, dest)

    def mkdir(self, path: str):
        self._logger.info(f"Creating directory {path}...")
        self._store.mkdir(path)

    def finalize(self) -> str:
        if not self._finalized:
            for k, v in self._paks.items():
                self._store.write(
                    json.dumps(v, indent=4).encode("utf-8"), f"bundles/{k}.json"
                )
            self._finalized = self._store.finalize()
        return self._finalized
