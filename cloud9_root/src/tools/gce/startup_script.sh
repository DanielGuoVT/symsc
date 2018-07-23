#!/bin/bash -e
#
# Copyright 2012 EPFL. All rights reserved.
# Author: Stefan Bucur (stefan.bucur@epfl.ch)

METADATA_SERVER='http://metadata/0.1/meta-data'
CLOUD9_BINARY="gs://cloud9-binaries/cloud9.tar.gz"
NUM_JOBS="$(grep -c "^processor" /proc/cpuinfo)"

# STEP 1: Install all the Cloud9 prerequisites
apt-get install \
    dejagnu flex bison protobuf-compiler libprotobuf-dev libboost-thread-dev \
    libboost-system-dev build-essential libcrypto++-dev

cd /tmp
wget http://google-glog.googlecode.com/files/glog-0.3.1-1.tar.gz
tar -xzvf glog-0.3.1-1.tar.gz

cd glog-0.3.1/
./configure
make -j${NUM_JOBS}
make install
ldconfig

cd /tmp
rm -rf glog-0.3.1-1.tar.gz glog-0.3.1/

# STEP 2: Copy the Cloud9 binaries
gsutil cp ${CLOUD9_BINARY} /tmp

# STEP 3: Extract the Cloud9 binaries
tar -xzvf /tmp/cloud9.tar.gz -C /opt
rm -rf /tmp/cloud9.tar.gz
