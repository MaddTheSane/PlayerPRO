#!/bin/bash

git rev-list HEAD | sort > config.git-hash
LOCALVER=`wc -l config.git-hash | awk '{print $1}'`

if [ $LOCALVER \> 1 ] ; then
    VER=`git rev-list origin/unicode | sort | join config.git-hash - | wc -l | awk '{print $1}'`

    if [ $VER != $LOCALVER ] ; then
        VER=$LOCALVER
    fi

	## 31 is the base of the svn version
    VER=`expr ${VER} + 31`

    echo $VER

else
    VER="unknown"
fi
rm -f config.git-hash