#!/usr/bin/env python

import json
import utils
import cache
import argparse
from typing import Optional

def strips_nulls(d: list) -> list:
    return [x for x in d if x is not None]

code, data = cache.make("dump")
pkgs = strips_nulls(json.loads(data))

print(json.dumps(pkgs))

