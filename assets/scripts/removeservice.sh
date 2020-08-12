#!/usr/bin/env bash

#1 path base
#2 user
#3 service

function echoerr() {
  echo "$@" 1>&2;
}

function removeIT() {
  if [ -f "$1" ]
  then
    echoerr "rm: $1"
    rm -f $1
  fi
}

echoerr "$@"
THEEXE='.zo .zcfg .yaml'
for i in $THEEXE
do
  removeIT "$1/$2/$3$i"
done

#echoerr "$@"


