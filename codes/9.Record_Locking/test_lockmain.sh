#!/bin/bash

#
# 测试lockmain
# 

NUM=2

(
cat <<!EOF
$NUM
!EOF
) >seqno

#cat >seqno <<!EOF
#$NUM
#!EOF


./lockmain &
./lockmain &
./lockmain &

#rm seqno
