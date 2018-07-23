#!/bin/bash

BENCHDIR=.
BENCHS=`ls -l ${BENCHDIR} | egrep '^d' | awk '{print $9}'`
TARGET='table4-twostep.csv'
CONFIG='config'
LAYOUT='layout'

if [ -f ${TARGET} ]; then
	rm ${TARGET}
else
	touch ${TARGET}
fi

echo "Name, Interleaving, Fail/Test, Time(m)" > ${TARGET}
for BENCH in ${BENCHS}
	do 
		if [ ${BENCH} = 'bak' ]; then
			continue
		fi

		cd ${BENCH}

		if [ ! -f ${CONFIG} ]; then 
			echo 'No config file, now exit ${BENCH}'
			cd ..
			continue
		fi
		if [ ! -f ${LAYOUT} ]; then 
			echo 'No layout file, now exit ${BENCH}'
			cd ..
			continue
		fi
		if [ ! -f ${BENCH}.c.twostep ]; then 
			echo "No ${BENCH}.c.twostep file, now exit ${BENCH}"
			cd ..
			continue
		else
			cp ${BENCH}.c.twostep ${BENCH}.c
		fi

		echo "Now enter ${BENCH}."
		./clean
		./genSymTable
		BEG_TIME=$(date +%s)
		./usc-run -fixed -max-time=20000
		END_TIME=$(date +%s)
		EXE_SEC=$(echo "${END_TIME} - ${BEG_TIME}" | bc)
		EXE_MIN=$(echo "scale=1;${EXE_SEC}/60" | bc)

		INTER=$(cat "log.Inter")
		FAIL=$(cat "log.Fail")
		TEST=$(cat "log.Test")

		echo ${INTER}
		echo ${FAIL}
		echo ${TEST}
		
		cd ..
		echo "${BENCH}, ${INTER}, ${FAIL}/${TEST}, ${EXE_MIN}" >> ${TARGET}
done

