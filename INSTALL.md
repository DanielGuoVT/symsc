Assume symsc is cloned and placed in /home/user/symsc.  
Next, please use the following steps to compile our tool before running the experiments.  
### Install third-party packages
```bash
sudo apt-get install -y dejagnu flex bison protobuf-compiler libprotobuf-dev \
libboost-thread-dev libboost-system-dev build-essential libcrypto++-dev libfreetype6-dev gawk
```
### Install glog
```bash
tar xzvf symsc.tar.gz
export CLOUD9_ROOT=/home/user/symsc/cloud9_root
cd /home/user/symsc/glog-0.3.3
./configure 2>&1 | tee -a $CLOUD9_ROOT/build.log
make -j3 2>&1 | tee -a $CLOUD9_ROOT/build.log
sudo make install 2>&1 | tee -a $CLOUD9_ROOT/build.log
sudo ldconfig 2>&1 | tee -a $CLOUD9_ROOT/build.log
```

__Note:__
* Please use the path information on your own computer accordingly.
* Please use a proper number to compile glog in terms of the CPU cores on your own computer.


### Install binutils-gold
```bash
cd $CLOUD9_ROOT/src/third_party
mkdir binutils-install
cd binutils
./configure --prefix=/home/user/symsc/cloud9_root/src/third_party/binutils-install --enable-gold --enable-plugins
make all -j3
make install
export BINUTILS_DIR=/home/user/symsc/cloud9_root/src/third_party/binutils
export BINUTILS_BUILD_DIR=/home/user/symsc/cloud9_root/src/third_party/binutils-install
rm -f ${BINUTILS_BUILD_DIR}/bin/ld
ln ${BINUTILS_BUILD_DIR}/bin/ld.gold ${BINUTILS_BUILD_DIR}/bin/ld
```

### Build llvm
```bash
cd $CLOUD9_ROOT/src/third_party
mkdir llvm-build
cd llvm-build
../llvm/configure  --enable-optimized  --enable-assertions --with-binutils-include="$(readlink -f ../binutils-install/include)"
make -j3
```
__Note:__ you can take a break here, since building llvm always takes a lot of time.

### Build the uClibc:
```bash
cd $CLOUD9_ROOT/src/klee-uclibc
echo "Build Klee's uClibc:" 2>&1 | tee -a $CLOUD9_ROOT/build.log
make -j3 2>&1 | tee -a $CLOUD9_ROOT/build.log
```

### Build STP
```bash
cd $CLOUD9_ROOT/src/third_party/stp
echo "Build STP:" 2>&1 | tee -a $CLOUD9_ROOT/build.log
./scripts/configure --with-prefix=$(pwd) 2>&1 | tee -a $CLOUD9_ROOT/build.log
make -j3 2>&1 | tee -a $CLOUD9_ROOT/build.log
```

### Build Cloud9 itself
```bash
cd $CLOUD9_ROOT/src
echo "Build Cloud9 itself:" 2>&1 | tee -a $CLOUD9_ROOT/build.log
make -j3 2>&1 | tee -a $CLOUD9_ROOT/build.log
cd $CLOUD9_ROOT/src/cloud9
./configure --with-llvmsrc=../third_party/llvm /
     --with-llvmobj=../third_party/llvm-build / 
     --with-uclibc=../klee-uclibc / 
     --enable-posix-runtime /
     --with-stp=../third_party/stp  /
     CXXFLAGS="-I$CLOUD9_ROOT/src/cloud9/include"
make -j3
```

### Update the environment variables
Append the following three lines to ~/.bashrc:
```bash
export PATH=$PATH:”/home/user/symsc/cloud9_root/src/cloud9/Release+Asserts/bin”
export PATH=$PATH:”/home/user/symsc/cloud9_root/src/third_party/llvm-build/Release+Asserts/bin”
alias csc=”cd /home/user/symsc/cloud9_root/src/testing_targets/cache_side_channel”
```
And reload the terminal profile by the following command:
```bash
source ~/.bashrc
```

Now, you can test klee with the --help option:
```bash
$ klee --help
```
If everything works correctly you will see a list of klee options.
