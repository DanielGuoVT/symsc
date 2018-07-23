# SYMSC
Author: Shengjian (Daniel) Guo  
Email : guosj@vt.edu  
### Introduction
This repository contains artifacts for the FSE'18 paper "Adversarial Symbolic Execution for 
Detecting Concurrency-Related Cache Timing Leaks". It consists of following files:

* The SymSC symbolic execution tool for the cache side-channel leak detection.
* The benchmarks used in Table 3, Table 4 and Table 5 in our paper.
* The scripts running these benchmarks for the experimental data collection.
* The supporting documents (README.md, INSTALL.md, LICENSE and CONTACT.md).

### Before the evaluation: 
Please prepare a clean Ubuntu 12.04 64-bit Desktop Operating System. 
You can either download an iso file from http://releases.ubuntu.com/12.04/, or use the VirtualBox 
to run a prepared Ubuntu image from https://www.osboxes.org/ubuntu/.

### Install
Assume you’ve already installed the Ubuntu system successfully.
Then clone the repository to your own computer. Here we just assume you 
checkout it to your home directory.
``` bash
cd
git clone git@github.com:DanielGuoVT/symsc.git
``` 
Next, please refer to the INSTALL.md to install our tool.

### Evaluation
After the installation, please go to the benchmark directory and compile the benchmarks:
``` bash
csc
../build/gyp_testing_targets
../build/make_all
```

Some benchmarks have the same name in our paper and they are differentiated by the
name-suffixes as following:

| Benchmark        | Directory  |
| ------------- |:-------------:|
| AES\[6\]      | aes-openssl |
| AES\[9\]      | aes      |
|CAST5\[4\] | cast5-tom      |
|CAST5\[19\] | cast5      |
|DES\[3\] | des-libgcrypt      |
|DES\[19\] | des      |

I also created five script files (also in the benchmark directory) for the experimental data collection.  
You can directly run them (no parameters required) to generate data tables:  
```
genTable3.sh： generates table3.csv which provides the data  for Table 3 in the paper.  
genTable4-precise.sh： generate the data (table4-precise.csv) for the “Precise” columns in Table 4.  
genTable4-twostep.sh： generate the data (table4-twostep.csv) for the “Two-Step” columns in Table 4.  
genTable5-precise.sh： generate the data (table5-precise.csv) for the “Precise” columns in Table 5.  
genTable5-twostep.sh： generate the data (table5-twostep.csv) for the “Two-Step” columns in Table 5.  
```

If any execution of a benchmark goes wrong, you can simply enter the benchmark directory and re-run it individually.  
E.g., assume “aes” goes wrong, then you please run it again with the following commands: 
``` bash
csc
cd aes
```

For the precise approach with fixed-address please run:
``` bash
cp aes.c.precise aes.c
./usc-run  -precise  -fixed  -max-time=96000
```

For the precise approach with symbolic-address please run:
``` bash
cp aes.c.precise aes.c
./usc-run  -precise  -max-time=96000
```

For the two-step approach with fixed-address please run:
``` bash
cp aes.c.twostep aes.c
./usc-run  -fixed  -max-time=96000
```

For the two-step approach with symbolic-address please run:
``` bash
cp aes.c.twostep aes.c
./usc-run  -max-time=96000
```

__Note:__ 
Important options:
* -max-time option specifies timeout time for an execution in second. The side-channel analysis is extremely heavy in symbolic-address mode, and I set 1600 minutes as the timeout threshold in experiment.
* In the provided 5 scripts, you can also freely alternate the -max-time value to set global timeout threshold for each benchmark.
* Please assign the virtual machine as much computing resources as possible to finish all the executions.

For each execution, our tool generates several log files for the execution information:
* log.precise-fixed: log file for the precise approach with fixed-address.
* log.precise-symbolic: log file for the precise approach with symbolic-address.
* log.twostep-fixed: log file for the two-step approach with fixed-address.
* log.twostep-symbolic: log file for the two-step approach with symbolic-address.
* log.Inter: the number of the generated interleavings.
* log.Fail: the number of failed 1st-step tries in the two-step approach.
* log.Test: the number of valid tests.
* log.maxaccs: the actual number of memory accesses during execution.
* log.time: the execution start/end time.
* log.ks: the size of the symbolic key in execution.


All the benchmarks are pre-configured with gyp and the path of the global gyp file is:
```bash
/path/to/cloud9_root/src/testing_targets/build/all_llvm.gyp
```
