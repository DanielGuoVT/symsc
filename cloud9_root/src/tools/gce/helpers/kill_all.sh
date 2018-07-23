#!/bin/bash -e
#
# Copyright 2012 EPFL. All rights reserved.
# Author: Stefan Bucur (stefan.bucur@epfl.ch)

: ${FREQUENCY:="5"}
: ${SIGNAL:="SIGINT"}
: ${TOOLS:="c9-worker c9-lb klee"}
: ${MILD:=""}

while true; do
		killall -q -${SIGNAL} ${TOOLS}
		DONE="true"
		for TOOL in ${TOOLS}; do
				pgrep ${TOOL} >/dev/null && DONE=""
		done
		[ -n "$DONE" -o -n "$MILD" ] && break
		echo "Tasks still running. Waiting ${FREQUENCY} more second(s)..."
		sleep ${FREQUENCY}
done
