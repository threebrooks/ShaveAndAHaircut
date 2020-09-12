#!/bin/bash -e

export JAVA_HOME=/usr/lib/jvm/java-11-openjdk-armhf

. ../Godec/install/env.sh
. install/env.sh

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/boost_1_74_0/stage/lib

python3 run.py
