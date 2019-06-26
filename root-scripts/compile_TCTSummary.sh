#!/bin/sh

#
# ./compile.sh Sourcefile_with_extension
#
# PROGRAMS can be:
#   plot1D,edge_tree,plot2D,plotvd_tleft,plotvd,plotDerivative,FitErf,GetZ0
#

if (test "$#" -ne 1);then
  echo "Please give source filename with extension"
  echo
  return
fi

#Find out 32 or 64 bits
UNM=`uname -m`
BITS=32
if ( test $UNM = "x86_64" );then
  BITS=64
fi

#Find out ROOT VERSION
ROOTV=`root-config --version | awk -F '.' '{print $1}'`
if ( test $ROOTV = "5" );then
  RCINT="rootcint"
else
  RCINT="rootcling"
fi


#PROG=`./mybasename $1`
PROG=`echo $1| awk -F '.' '{print $1}'`
EXT=`echo $1| awk -F '.' '{print $2}'`


CFLAGS=`root-config --cflags`
RCLIBS=`root-config --libs` 
MYLIBS="TCTSummary.o TMeas.o TMeasHeader.o TWaveform.o TMeasDict.o TMeasHeaderDict.o TCTSummaryDict.o TWaveDict.o"
MYLIBS2="TCTSummary.o TCTSummaryDict.o TWaveform.o TWaveDict.o"
if ( test $BITS = "64" );then
  GC="g++ -g -std=c++0x -Wall -fPIC"
else
  GC="g++ -g -m32 -Wall -fPIC"
fi

ClassesComp() {

  #TMeas
  echo Compiling TMeas
  $GC $CFLAGS -c TMeas.cpp 
  if (test "$ROOTV" = "5");then
    rootcint -f TMeasDict.C -c -p -I./ TMeas.h
  else
    rootcling -f TMeasDict.C -c -p -I./ TMeas.h LinkDef.h    
  fi
  $GC `root-config --cflags` -c TMeasDict.C -o TMeasDict.o

  #TMeasHeader
  echo Compiling TMeasHeader
  $GC $CFLAGS -c TMeasHeader.cpp 
  $RCINT -f TMeasHeaderDict.C -c -p -I./ TMeasHeader.h
  $GC `root-config --cflags` -c TMeasHeaderDict.C -o TMeasHeaderDict.o

  #TWaveform
  echo Compiling TWaveform
  $GC $CFLAGS -c TWaveform.cpp 
  if (test "$ROOTV" = "5");then
    $RCINT  -f TWaveDict.C -c -p -I./ TWaveform.h 
  else
    $RCINT  -f TWaveDict.C -c -p -I./ TWaveform.h LinkDefWvf.h
  fi
  $GC `root-config --cflags` -c TWaveDict.C -o TWaveDict.o

  #TCTSummary
  echo Compiling TCTSummary
  $GC $CFLAGS -c TCTSummary.cpp 
  $RCINT -f TCTSummaryDict.C -c -p -I./ TCTSummary.h LinkDef_Summary.h
  $GC `root-config --cflags` -c TCTSummaryDict.C -o TCTSummaryDict.o

  echo 0
}

if (test "$PROG" = "MakeTCTSummary");then
  ex=`ClassesComp`
  #$GC $CFLAGS -I /usr/include -c "$PROG"."$EXT"
  #g++ -g $PROG.o $MYLIBS $RCLIBS -lTreePlayer -L/usr/lib -o $PROG
  $GC $CFLAGS -c "$PROG"."$EXT"
  g++ -g $PROG.o $MYLIBS $RCLIBS -lTreePlayer -lMathMore -o $PROG

else
  ex=`ClassesComp`
  $GC $CFLAGS -c "$PROG"."$EXT"
  g++ -g $PROG.o $MYLIBS2 $RCLIBS -lTreePlayer -lMathMore -o $PROG
fi



