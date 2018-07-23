#!/bin/bash

BENCHDIR=.
BENCHS=`ls -l ${BENCHDIR} | egrep '^d' | awk '{print $9}'`
TARGET='table3.csv'
if [ -f ${TARGET} ]; then
	rm ${TARGET}
else
	touch ${TARGET}
fi

echo 'Name, Lines_of_C, Lines_of_LL, Key_size, Memory_accesses' >> ${TARGET}
for BENCH in ${BENCHS}
	do 
		if [ ${BENCH} = 'bak' ]; then
			continue;
		else
			cd ${BENCH}
			prefix=${BENCH}_original
			cfile=${prefix}.c
			loc=`wc -l < ${cfile}`
			if [ ! -f ${prefix}.ll ]; then 
				clang -emit-llvm --no-warnings -c ${cfile} -o ${prefix}.bc 
				llvm-dis ${prefix}.bc
			fi
			lol=`wc -l < ${prefix}.ll`
			./usc-run -regular
			ks=$(cat "log.ks")
			ma=$(cat "log.accs")
			rm -f log.ks
			rm -f log.accs
			cd ..
			echo "${BENCH}, ${loc}, ${lol}, ${ks}, ${ma}" >> ${TARGET}
		fi
done

echo ""
echo "All finished, and please check ${TARGET}."
echo ""
