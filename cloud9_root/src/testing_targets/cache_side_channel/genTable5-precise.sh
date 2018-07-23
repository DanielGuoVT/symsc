#!/bin/bash

BENCHDIR=.
BENCHS=`ls -l ${BENCHDIR} | egrep '^d' | awk '{print $9}'`
TARGET='table5-precise.csv'
CONFIG='config'
LAYOUT='layout'

if [ -f ${TARGET} ]; then
	rm ${TARGET}
else
	touch ${TARGET}
fi

echo "Name, nterleaving, Test, Time(m)" > ${TARGET}
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
		if [ ! -f ${BENCH}.c.precise ]; then 
			echo "No ${BENCH}.c.precise file, now exit ${BENCH}"
			cd ..
			continue
		else
			cp ${BENCH}.c.precise ${BENCH}.c
		fi

		echo "Now enter ${BENCH}."
		./clean
		./genSymTable
		BEG_TIME=$(date +%s)
		./usc-run -precise -max-time=96000
		END_TIME=$(date +%s)
		EXE_SEC=$(echo "${END_TIME} - ${BEG_TIME}" | bc)
		EXE_MIN=$(echo "scale=1;${EXE_SEC}/60" | bc)

		INTER=$(cat "log.Inter")
		TEST=$(cat "log.Test")

		echo ${INTER}
		echo ${TEST}
		
		cd ..
		echo "${BENCH}, ${INTER}, ${TEST}, ${EXE_MIN}" >> ${TARGET}
done

