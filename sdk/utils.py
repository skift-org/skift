from copy import copy
import errno
import os
import hashlib
import signal
import requests
import subprocess
import json
import copy
from types import SimpleNamespace


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


def hashFile(filename: str) -> str:
    with open(filename, "rb") as f:
        return hashlib.sha256(f.read()).hexdigest()


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
        raise CliException(f"Failed to download {url}")


def runCmd(*args: str) -> bool:
    try:
        proc = subprocess.run(args)
    except FileNotFoundError:
        raise CliException(f"Failed to run {args[0]}: command not found")

    if proc.returncode == -signal.SIGSEGV:
        raise CliException("Segmentation fault")

    if proc.returncode != 0:
        raise CliException(
            f"Failed to run {' '.join(args)}: process exited with code {proc.returncode}")

    return True


CACHE = {}


def processJson(e: any) -> any:
    if isinstance(e, dict):
        for k in e:
            e[processJson(k)] = processJson(e[k])
    elif isinstance(e, list):
        for i in range(len(e)):
            e[i] = processJson(e[i])
    elif isinstance(e, str):
        if e == "@sysname":
            e = os.uname().sysname.lower()
        elif e.startswith("@include("):
            e = loadJson(e[9:-1])

    return e


def loadJson(filename: str) -> dict:
    result = {}
    if filename in CACHE:
        result = CACHE[filename]
    else:
        with open(filename) as f:
            result = json.load(f)

            result["dir"] = os.path.dirname(filename)
            result["json"] = filename
            result = processJson(result)
            CACHE[filename] = result

    result = copy.deepcopy(result)
    return result
