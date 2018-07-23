#!/bin/bash

find ./ -not -type l \( -name '*.cpp' -o -name '*.cc' -o -name '*.h' -o -name '*.hh' \) | \
		while read FILENAME
		do 
				expand -t2 $FILENAME >$FILENAME.tmp && mv $FILENAME.tmp $FILENAME
		done
