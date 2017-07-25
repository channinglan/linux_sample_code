#!/bin/bash
 
# written by Barry Song

while getopts "s:d:" OPT; do
    case "$OPT" in
        s)	SRC=$OPTARG;;
        d)	DST=$OPTARG;;
        [?])	echo "Usage: $0 [-s exec_or_lib] [-d depend_lib]" >&2
                echo
                echo "This tool figures out the symbols level dependency for between"
                echo "exec_or_lib and another lib"
                echo
                exit 1;;
    esac
done
shift $((OPTIND-1))

UNDEFINE=.UNDEF.tmp
DEFINED=.DEFIN.tmp

if [ ! -e $SRC ]; then
    echo "ERROR: File not found: $SRC"
    echo "Aborting..."
    exit 2
else
    #echo exec/lib: $SRC
    nm -D --undefined-only $SRC > $UNDEFINE
fi

if [ ! -e $DST ]; then
    echo "ERROR: File not found: $DST"
    echo "Aborting..."
    exit 2
else
    #echo depended lib: $DST
    nm -D --defined-only  $DST > $DEFINED
fi

DEPENDENCIES=$(awk -F '[ ]' '{print substr($11,1)}' $UNDEFINE) 
EXPORTS=$(awk -F '[ ]' '{print substr($3,1)}' $DEFINED)

echo "Dependencies between $SRC and $DST:"

for SRCFUNC in $DEPENDENCIES ; do
	for DSTFUNC in $EXPORTS ; do
		if [ $SRCFUNC = $DSTFUNC ]; then
		echo $SRCFUNC
		break
		fi
	done
done

rm $UNDEFINE $DEFINED

exit 0
