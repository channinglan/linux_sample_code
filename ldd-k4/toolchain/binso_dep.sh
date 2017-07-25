#!/bin/bash

# Original script came from;
#     http://domseichter.blogspot.com/2008/02/visualize-dependencies-of-binaries-and.html
# Written by Dominik Seichter and added patch from pfree
# added patches from Barry Song for ARM and embedded systems
#
# analyze a given file on its
# dependecies using ldd and write
# the results to a given temporary file
#
# Usage: analyze [OUTPUTFILE] [INPUTFILE]
function analyze
{
    local OUT=$1
    local IN=$2
    local NAME=$(basename $IN)
 
    for i in $LIST
    do
        if [ "$i" == "$NAME" ];
        then
            # This file was already parsed
            return
        fi
    done
    # Put the file in the list of all files
    LIST="$LIST $NAME"
 
    DEPTH=$[$DEPTH + 1]
 
    echo "Parsing file:              $IN"

    if [ -e $IN ]; then
	LIBPATH=$IN
    else
	LIBPATH=`find $ROOTPATH -name $IN`
    fi

    $READELF $LIBPATH &> $READELFTMPFILE
    ELFRET=$?
    if [ $ELFRET = 0 ]; then
	IN=$LIBPATH
    fi

    if [ $ELFRET != 0 ];
        then
        echo "ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
        cat $TMPFILE
        echo "Aborting at $IN, does it exist or is it an ELF file?"
        rm $TMPFILE
        rm $READELFTMPFILE
        rm $OBJDUMPTMPFILE
        echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ERROR"
        exit 1
    fi
 
    DEPENDENCIES=$(cat $READELFTMPFILE | grep NEEDED | awk '{if (substr($NF,1,1) == "[") print substr($NF, 2, length($NF) - 2); else print $NF}')
 
    for DEP in $DEPENDENCIES;
    do
        if [ -n "$DEP" ];
        then
 
            objdump -x $IN | grep NEEDED &> $OBJDUMPTMPFILE
            OBJDUMPRET=$?
 
            if [ $OBJDUMPRET != 0 ];
                then
                echo "ERROR: objdump returned error code $RET"
                echo "ERROR:"
                cat $TMPFILE
                echo "Aborting..."
                rm $TMPFILE
                rm $READELFTMPFILE
                rm $OBJDUMPTMPFILE
                exit 1
            fi
 
            DEPPATH=$(grep $DEP $OBJDUMPTMPFILE | awk '{print $2}')
            if [ -n "$DEPPATH" ];
            then
                if [ $DEPTH -lt $MAXDEPTH ]; then
                    echo -e "  \"$NAME\" -> \"$DEP\";" >> $OUT
                fi
                analyze $OUT $DEPPATH
            fi
        fi
    done
 
    DEPTH=$[$DEPTH - 1]
}
 
########################################
# main                                 #
########################################

MAXDEPTH=14
ROOTPATH=`pwd`

while getopts "d:o:r:" OPT; do
    case "$OPT" in
        d)	MAXDEPTH=$OPTARG;;
        o)	OUTPUT=$OPTARG;;
        r)	ROOTPATH=$OPTARG;;
        [?])	echo "Usage: $0 [-d max_depth] [-o output_png] [-r rootfs_path] FILE" >&2
                echo
                echo "This tools analyses a shared library or an executable"
                echo "and generates a dependency graph as an image."
                echo
                echo "GraphViz must be installed for this tool to work."
                exit 1;;
    esac
done
shift $((OPTIND-1))

INPUT=$@
if [ ! -e $INPUT ]; then
    echo "ERROR: File not found: $INPUT"
    echo "Aborting..."
    exit 2
fi

DEPTH=0
TMPFILE=$(mktemp -t)
OBJDUMPTMPFILE=$(mktemp -t)
READELFTMPFILE=$(mktemp -t)
LIST=""
 
# Use either readelf or dump
# Linux has readelf, Solaris has dump
READELF=$(type arm-linux-gnueabihf-readelf 2> /dev/null)
if [ $? != 0 ]; then
  READELF=$(type dump 2> /dev/null)
  if [ $? != 0 ]; then
    echo Unable to find ELF reader
    exit 1
  fi
  READELF="dump -Lv"
else
  READELF="arm-linux-gnueabihf-readelf -d"
fi
 
echo "Analyzing dependencies of $INPUT...
"
echo "digraph DependencyTree {" > $TMPFILE
echo "  \"$(basename $INPUT)\" [shape=box];" >> $TMPFILE

analyze $TMPFILE "$INPUT"
echo "}" >> $TMPFILE

if [ -z $OUTPUT ]; then
    cat $TMPFILE # output generated dotfile for debugging purposses
else
    echo "Creating output as $OUTPUT..."
    dot -Tpng $TMPFILE -o$OUTPUT
fi
 
rm $OBJDUMPTMPFILE
rm $TMPFILE
 
exit 0
