#!/bin/bash -e
#
# Copyright 2012 EPFL. All rights reserved.
# Author: Stefan Bucur (stefan.bucur@epfl.ch)

: ${WORKER_PATH:="/opt/cloud9/bin/c9-worker"}
: ${LB_PATH:="/opt/cloud9/bin/c9-lb"}

# scp %(coverage)s %(user)s@%(host)s:%(expdir)s/%(newdir)s/$(basename %(coverage)s)

if [ ! -f %(root)s/%(worker)s ]; then echo "Cannot find the Cloud9 worker executable: %(root)s/%(worker)s";  exit 1; fi
if [ ! -f %(root)s/%(klee)s ]; then echo "Cannot find the Klee executable: %(root)s/%(klee)s"; exit 1; fi
mkdir -p %(expdir)s/%(newdir)s
%(cleancores)s && find %(expdir)s -name 'core' | xargs rm -f
if [ -h %(expdir)s/last ]; then rm -f %(expdir)s/last; fi
[ ! -a %(expdir)s/last ] && ln -s %(expdir)s/%(newdir)s %(expdir)s/last