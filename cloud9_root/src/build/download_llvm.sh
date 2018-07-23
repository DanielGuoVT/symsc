#!/usr/bin/env bash
# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# This script will check out llvm and clang into third_party/llvm and build it.

# Do NOT CHANGE this if you don't know what you're doing -- see
# https://code.google.com/p/chromium/wiki/UpdatingClang
# Reverting problematic clang rolls is safe, though.
CLANG_REVISION=159523


THIS_DIR="$(dirname "${0}")"
LLVM_DIR="${THIS_DIR}/../third_party/llvm"
LLVM_BUILD_DIR="${LLVM_DIR}/../llvm-build"
LLVM_BOOTSTRAP_DIR="${LLVM_DIR}/../llvm-bootstrap"
CLANG_DIR="${LLVM_DIR}/tools/clang"
COMPILER_RT_DIR="${LLVM_DIR}/projects/compiler-rt"
STAMP_FILE="${LLVM_BUILD_DIR}/cr_build_revision"

# ${A:-a} returns $A if it's set, a else.
LLVM_REPO_URL=${LLVM_URL:-https://llvm.org/svn/llvm-project}

# Die if any command dies.
set -e

OS="$(uname -s)"

# Parse command line options.
force_local_build=
mac_only=
run_tests=
bootstrap=
debug_build=
trunk=
while [[ $# > 0 ]]; do
  case $1 in
    --bootstrap)
      bootstrap=yes
      ;;
    --force-local-build)
      force_local_build=yes
      ;;
    --mac-only)
      mac_only=yes
      ;;
    --run-tests)
      run_tests=yes
      ;;
    --debug-build)
      debug_build=yes
      ;;
    --trunk)
      trunk=yes
      ;;
    --help)
      echo "usage: $0 [--force-local-build] [--mac-only] [--run-tests] "
      echo "--bootstrap: First build clang with CC, then with itself."
      echo "--force-local-build: Don't try to download prebuilt binaries."
      echo "--mac-only: Do initial download only on Mac systems."
      echo "--run-tests: Run tests after building. Only for local builds."
      echo "--debug-build: Also do the debug build."
      echo "--trunk: Check out code from trunk, instead of release."
      exit 1
      ;;
  esac
  shift
done

# --mac-only prevents the initial download on non-mac systems, but if clang has
# already been downloaded in the past, this script keeps it up to date even if
# --mac-only is passed in and the system isn't a mac. People who don't like this
# can just delete their third_party/llvm-build directory.
if [[ -n "$mac_only" ]] && [[ "${OS}" != "Darwin" ]] &&
    ! [[ -d "${LLVM_BUILD_DIR}" ]]; then
  exit 0
fi

if [[ -n "${trunk}" ]]; then
	LLVM_REPO_PATH="llvm/trunk"
	CLANG_REPO_PATH="cfe/trunk"
	COMPILER_RT_REPO_PATH="compiler-rt/trunk"
else
	LLVM_REPO_PATH="llvm/tags/RELEASE_31/final"
	CLANG_REPO_PATH="cfe/tags/RELEASE_31/final"
	COMPILER_RT_REPO_PATH="compiler-rt/tags/RELEASE_31/final"
fi

# Xcode and clang don't get along when predictive compilation is enabled.
# http://crbug.com/96315
if [[ "${OS}" = "Darwin" ]] && xcodebuild -version | grep -q 'Xcode 3.2' ; then
  XCONF=com.apple.Xcode
  if [[ "${GYP_GENERATORS}" != "make" ]] && \
     [ "$(defaults read "${XCONF}" EnablePredictiveCompilation)" != "0" ]; then
    echo
    echo "          HEARKEN!"
    echo "You're using Xcode3 and you have 'Predictive Compilation' enabled."
    echo "This does not work well with clang (http://crbug.com/96315)."
    echo "Disable it in Preferences->Building (lower right), or run"
    echo "    defaults write ${XCONF} EnablePredictiveCompilation -boolean NO"
    echo "while Xcode is not running."
    echo
  fi

  SUB_VERSION=$(xcodebuild -version | sed -Ene 's/Xcode 3\.2\.([0-9]+)/\1/p')
  if [[ "${SUB_VERSION}" < 3 ]]; then
    echo
    echo "          YOUR LD IS BUGGY!"
    echo "Please upgrade Xcode to at least 3.2.3."
    echo
  fi
fi


# Check if there's anything to be done, exit early if not.
if [[ -f "${STAMP_FILE}" ]]; then
  PREVIOUSLY_BUILT_REVISON=$(cat "${STAMP_FILE}")
  if [[ -z "$force_local_build" ]] && \
       [[ "${PREVIOUSLY_BUILT_REVISON}" = "${CLANG_REVISION}" ]]; then
    echo "Clang already at ${CLANG_REVISION}"
    exit 0
  fi
fi
# To always force a new build if someone interrupts their build half way.
rm -f "${STAMP_FILE}"

# Clobber pch files, since they only work with the compiler version that
# created them. Also clobber .o files, to make sure everything will be built
# with the new compiler.
if [[ "${OS}" = "Darwin" ]]; then
  XCODEBUILD_DIR="${THIS_DIR}/../../../xcodebuild"

  # Xcode groups .o files by project first, configuration second.
  if [[ -d "${XCODEBUILD_DIR}" ]]; then
    echo "Clobbering .o files for Xcode build"
    find "${XCODEBUILD_DIR}" -name '*.o' -exec rm {} +
  fi
fi

echo Getting LLVM r"${CLANG_REVISION}" in "${LLVM_DIR}"
if ! svn co --force "${LLVM_REPO_URL}/${LLVM_REPO_PATH}@${CLANG_REVISION}" \
                    "${LLVM_DIR}"; then
  echo Checkout failed, retrying
  rm -rf "${LLVM_DIR}"
  svn co --force "${LLVM_REPO_URL}/${LLVM_REPO_PATH}@${CLANG_REVISION}" "${LLVM_DIR}"
fi

echo Getting clang r"${CLANG_REVISION}" in "${CLANG_DIR}"
svn co --force "${LLVM_REPO_URL}/${CLANG_REPO_PATH}@${CLANG_REVISION}" "${CLANG_DIR}"

echo Getting compiler-rt r"${CLANG_REVISION}" in "${COMPILER_RT_DIR}"
svn co --force "${LLVM_REPO_URL}/${COMPILER_RT_REPO_PATH}@${CLANG_REVISION}" \
               "${COMPILER_RT_DIR}"

# Echo all commands.
set -x

echo "Applying Gold Makefile patch..."
patch -sNd "${LLVM_DIR}" -p0 <"${THIS_DIR}/llvm-32-gold-makefile.patch" >/dev/null || \
    echo "Patch already applied or could not be applied"


NUM_JOBS=3
if [[ "${OS}" = "Linux" ]]; then
  NUM_JOBS="$(grep -c "^processor" /proc/cpuinfo)"
elif [ "${OS}" = "Darwin" ]; then
  NUM_JOBS="$(sysctl -n hw.ncpu)"
fi

# Build bootstrap clang if requested.
if [[ -n "${bootstrap}" ]]; then
  echo "Building bootstrap compiler"
  mkdir -p "${LLVM_BOOTSTRAP_DIR}"
  cd "${LLVM_BOOTSTRAP_DIR}"
  if [[ ! -f ./config.status ]]; then
    # The bootstrap compiler only needs to be able to build the real compiler,
    # so it needs no cross-compiler output support. In general, the host
    # compiler should be as similar to the final compiler as possible.
    ../llvm/configure \
        --enable-optimized \
        --enable-targets=host-only
    MACOSX_DEPLOYMENT_TARGET=10.5 make -j"${NUM_JOBS}"
  fi
  if [[ -n "${run_tests}" ]]; then
    make check-all
  fi
  cd -
  export CC="${PWD}/${LLVM_BOOTSTRAP_DIR}/Release+Asserts/bin/clang"
  export CXX="${PWD}/${LLVM_BOOTSTRAP_DIR}/Release+Asserts/bin/clang++"
  echo "Building final compiler"
fi

# Build clang (in a separate directory).
# The clang bots have this path hardcoded in built/scripts/slave/compile.py,
# so if you change it you also need to change these links.
mkdir -p "${LLVM_BUILD_DIR}"
cd "${LLVM_BUILD_DIR}"
if [[ ! -f ./config.status ]]; then
  ../llvm/configure \
      --enable-optimized \
			--enable-assertions \
	    --with-binutils-include="$(readlink -f ../binutils-install/include)"
fi

MACOSX_DEPLOYMENT_TARGET=10.5 make -j"${NUM_JOBS}"

if [[ -n "${debug_build}" ]]; then
	MACOSX_DEPLOYMENT_TARGET=10.5 make ENABLE_OPTIMIZED=0 -j"${NUM_JOBS}"
fi

cd -

# After everything is done, log success for this revision.
echo "${CLANG_REVISION}" > "${STAMP_FILE}"
