#!/bin/bash

cat out.log | grep 'OK' | sed -n '/2022-07-22 16:42:51/,/*/p' | awk '{print $4, $8, $9}'
