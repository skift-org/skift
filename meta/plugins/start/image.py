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
    _finalized: Path | None

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

    def cpRef(self, pak: str, src: Path, id: str) -> Path:
        ref = f"bundle://{id}"
        sha256 = shell.sha256sum(src)
        dest = Path("objects") / sha256
        mime = magic.from_file(src, mime=True)
        self.ensurePak(pak)["objects"][ref] = {
            "mime": mime,
            "ref": f"file:/{dest}",
        }
        self._store.store(src, dest)
        return dest

    def installTo(self, componentSpec: str, targetSpec: str, dest: Path | str):
        if isinstance(dest, str):
            dest = Path(*dest.split("/"))

        product = self.install(componentSpec, targetSpec)
        self.cp(product.path, dest=dest)

    def install(self, componentSpec: str, targetSpec: str) -> builder.ProductScope:
        self._logger.info(f"Installing {componentSpec}...")
        component = self._registry.lookup(componentSpec, model.Component)
        assert component is not None

        target = self._registry.lookup(targetSpec, model.Target)
        assert target is not None

        scope = builder.TargetScope(self._registry, target)
        product = builder.build(scope, component)[0]

        for depId in component.resolved[target.id].required + [componentSpec]:
            dep = self._registry.lookup(depId, model.Component)
            if dep is None:
                raise Exception(f"Component {depId} not found")

            for res in builder.listRes(dep):
                rel = res.relative_to(dep.path.parent / "res")
                self.cpRef("_index", res, f"{depId}/{rel}")
                self.cpRef(componentSpec, res, f"{depId}/{rel}")

        self.cpRef("_index", product.path, f"{componentSpec}/_bin")
        self.cpRef(componentSpec, product.path, f"{componentSpec}/_bin")

        return product

    def cp(self, src: Path, dest: Path):
        self._logger.info(f"Copying {src} to {dest}...")
        self._store.store(src, dest)

    def cpTree(self, src: Path | str, dest: Path | str):
        if isinstance(src, str):
            src = Path(*src.split("/"))
        self._logger.info(f"Copying {src} to {dest}...")
        self._store.store(src, dest)

    def mkdir(self, path: Path | str):
        if isinstance(path, str):
            path = Path(*path.split("/"))
        self._logger.info(f"Creating directory {path}...")
        self._store.mkdir(path)

    def finalize(self) -> Path:
        if not self._finalized:
            for k, v in self._paks.items():
                self._store.write(
                    json.dumps(v, indent=4).encode("utf-8"),
                    Path("bundles") / f"{k}.json",
                )
            self._finalized = self._store.finalize()
        return self._finalized
