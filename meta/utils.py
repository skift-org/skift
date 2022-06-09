import errno
import os
import hashlib
import requests
import subprocess
import json


class Colors:
    BLACK = "\033[0;30m"
    RED = "\033[0;31m"
    GREEN = "\033[0;32m"
    BROWN = "\033[0;33m"
    BLUE = "\033[0;34m"
    PURPLE = "\033[0;35m"
    CYAN = "\033[0;36m"
    LIGHT_GRAY = "\033[0;37m"
    DARK_GRAY = "\033[1;30m"
    LIGHT_RED = "\033[1;31m"
    LIGHT_GREEN = "\033[1;32m"
    YELLOW = "\033[1;33m"
    LIGHT_BLUE = "\033[1;34m"
    LIGHT_PURPLE = "\033[1;35m"
    LIGHT_CYAN = "\033[1;36m"
    LIGHT_WHITE = "\033[1;37m"
    BOLD = "\033[1m"
    FAINT = "\033[2m"
    ITALIC = "\033[3m"
    UNDERLINE = "\033[4m"
    BLINK = "\033[5m"
    NEGATIVE = "\033[7m"
    CROSSED = "\033[9m"
    RESET = "\033[0m"


class CliException(Exception):
    def __init__(self, msg: str):
        self.msg = msg


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
            if isinstance(obj[key], bool):
                if obj[key]:
                    toKey.append(key)
            else:
                toKey.append(obj[key])

    return "-".join(toKey)


def mkdirP(path: str) -> str:
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


def runCmd(*args: str) -> bool:
    proc = subprocess.run(args)
    if proc.returncode != 0:
        raise Exception(f"Failed to run {' '.join(args)}")

    return True
