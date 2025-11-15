import logging
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

    def installTo(self, componentSpec: str, targetSpec: str, dest: str):
        product = self.install(componentSpec, targetSpec)[0]
        self.cp(str(product.path), dest=dest)

    def _installProduct(self, product: builder.ProductScope):
        component = product.component
        target = product.target

        # Base dir for the component inside the image
        base = f"bundles/{component.id}"

        # Copy resources of the component and its deps
        for depId in component.resolved[target.id].required + [component.id]:
            dep = self._registry.lookup(depId, model.Component)
            if dep is None:
                raise Exception(f"Component {depId} not found")

            depBase = f"bundles/{depId}"

            for res in builder.listRes(dep):
                rel = Path(res).relative_to(dep.subpath("res"))
                dest = f"{depBase}/{rel}"
                self.cp(res, dest)

        # Copy the product binary/library
        if component.type == model.Kind.EXE:
            dest = f"{base}/_bin"
        else:
            dest = f"{base}/_lib"

        self.cp(str(product.path), f"{dest}")

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
            self._finalized = self._store.finalize()
        return self._finalized
