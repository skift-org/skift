#!/bin/env python3

import sys
import subprocess

disk_image = sys.argv[1]
qemu_command = [
    "qemu-system-x86_64",
    "-hda", disk_image,
    "-serial", "mon:stdio",
    "-nographic",
    "-rtc", "base=localtime",
    "-no-reboot",
    "-m", "1G"]

qemu_proc = subprocess.Popen(
    qemu_command,
    stdout=subprocess.PIPE,
    stdin=subprocess.PIPE)

qemu_stdin = qemu_proc.stdin
qemu_stdout = qemu_proc.stdout

while True:
    if qemu_proc.poll() != None:
        sys.exit(-1)

    message = qemu_stdout.readline().decode(errors='replace').strip()

    if message == 'test: PASSED':
        qemu_proc.kill()
        sys.exit(0)

    elif message == 'test: FAILED':
        qemu_proc.kill()
        sys.exit(-1)

    elif message != '':
        print(message)
