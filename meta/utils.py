import errno
import os
import hashlib
import requests
import subprocess
import json


def stripDups(l: list[str]) -> list[str]:
    # Remove duplicates from a list
    # by keeping only the last occurence
    result: list[str] = []
    for item in l:
        if item in result:
            result.remove(item)
        result.append(item)
    return result


def findFiles(dir: str, exts: list[str] = []) -> list[str]:
    if not os.path.isdir(dir):
        return []

    result: list[str] = []

    for f in os.listdir(dir):
        if len(exts) == 0:
            result.append(f)
        else:
            for ext in exts:
                if f.endswith(ext):
                    result.append(os.path.join(dir, f))
                    break

    return result


def objSha256(obj: dict, keys: list[str] = []) -> str:
    toHash = {}
    if len(keys) == 0:
        toHash = obj
    else:
        for key in keys:
            if key in obj:
                toHash[key] = obj[key]

    data = json.dumps(toHash, sort_keys=True)
    return hashlib.sha256(data.encode("utf-8")).hexdigest()


def objKey(obj: dict, keys: list[str]) -> str:
    toKey = []
    for key in keys:
        if key in obj:
            toKey.append(str(obj[key]))

    return "-".join(toKey)


def mkdirP(path):
    try:
        os.makedirs(path)
    except OSError as exc:
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise
    return path


def downloadFile(url: str) -> str:
    dest = ".cache/remote/" + hashlib.sha256(url.encode('utf-8')).hexdigest()
    tmp = dest + ".tmp"

    if os.path.isfile(dest):
        return dest

    print(f"Downloading {url} to {dest}")

    try:
        r = requests.get(url, stream=True)
        r.raise_for_status()
        mkdirP(os.path.dirname(dest))
        with open(tmp, 'wb') as f:
            for chunk in r.iter_content(chunk_size=8192):
                if chunk:
                    f.write(chunk)

        os.rename(tmp, dest)
        return dest
    except:
        raise Exception(f"Failed to download {url}")


def runCmd(*args: list[str]) -> bool:
    proc = subprocess.run(args)
    if proc.returncode != 0:
        raise Exception(f"Failed to run {' '.join(args)}")

    return True
