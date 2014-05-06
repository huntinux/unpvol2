#!/bin/bash

SEMNAME="/tmpsem"


./semcreate $SEMNAME
./semgetval $SEMNAME
./semwait $SEMNAME &
./semgetval $SEMNAME
./sempost $SEMNAME
./semunlink $SEMNAME
