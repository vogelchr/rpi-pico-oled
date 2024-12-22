#!/bin/sh

if ! [ -x ./venv/bin/python ] ; then
	virtualenv ./venv
	./venv/bin/pip install platformio
fi

exec ./venv/bin/platformio "$@"
