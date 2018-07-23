#!/bin/bash
#
# Copyright 2012 EPFL. All rights reserved.
# Author: Stefan Bucur (stefan.bucur@epfl.ch)
#
# Package the Cloud9 binaries, in order to be deployed on a VM.

THIS_DIR="$(dirname "${0}")"
CLOUD9_DIR="${THIS_DIR}/.."

CLOUD9_BUILD_DIR="${CLOUD9_DIR}/cloud9/Release+Asserts"
DEPOT_BUILD_DIR="${CLOUD9_DIR}/out/Default"
LLVM_BUILD_DIR="${CLOUD9_DIR}/third_party/llvm-build/Release+Asserts"
TARGETS_BUILD_DIR="${CLOUD9_DIR}/testing_targets/out/Default"
UCLIBC_BUILD_DIR="${CLOUD9_DIR}/klee-uclibc"

# Die if any command dies
set -e

# Echo all commands
set -x

# Preparing the directory structure
ARCHIVE_DIR=$(mktemp -d)

mkdir "${ARCHIVE_DIR}/cloud9"
mkdir "${ARCHIVE_DIR}/cloud9/bin"
mkdir "${ARCHIVE_DIR}/cloud9/lib"
mkdir "${ARCHIVE_DIR}/cloud9/llvm-bin"

cp -r ${CLOUD9_BUILD_DIR}/bin/* "${ARCHIVE_DIR}/cloud9/bin"
cp -r ${CLOUD9_BUILD_DIR}/lib/* "${ARCHIVE_DIR}/cloud9/lib"

cp ${DEPOT_BUILD_DIR}/lib.target/* "${ARCHIVE_DIR}/cloud9/lib"

# TODO(bucur): Enable this via option
#cp -r ${LLVM_BUILD_DIR}/bin/* "${ARCHIVE_DIR}/cloud9/bin"
#cp -r ${LLVM_BUILD_DIR}/lib/* "${ARCHIVE_DIR}/cloud9/lib"

cp ${UCLIBC_BUILD_DIR}/lib/*.a "${ARCHIVE_DIR}/cloud9/llvm-bin"
cp ${TARGETS_BUILD_DIR}/*.bc "${ARCHIVE_DIR}/cloud9/llvm-bin"

# Creating the archive
tar -czvf cloud9.tar.gz -C ${ARCHIVE_DIR} cloud9/

rm -rf "$ARCHIVE_DIR"
