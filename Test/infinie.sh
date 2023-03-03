#!/bin/bash

i=5

while true
do
    sleep $i
    i=$((i-1))
    if [ $i -eq 1 ]
    then
        #echo "Break"
        break
    fi
done