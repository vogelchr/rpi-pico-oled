#!/bin/bash
set -euo pipefail

install -m644 99-ip-addr-info.rules /etc/udev/rules.d
install -m644 ip-addr-info@.service /etc/systemd/system
install -m755 ip-addr-info.py /usr/local/sbin
udevadm control --reload-rules
systemctl daemon-reload
