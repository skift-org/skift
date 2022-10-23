#!/usr/bin/env bash
sudo chown -c root:kvm /dev/kvm && sudo chmod -c 770 /dev/kvm
# add user to kvm group
sudo usermod -a -G kvm $USER