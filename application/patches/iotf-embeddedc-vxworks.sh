#!/bin/bash

git apply ../application/patches/iotf-embeddedc-for-vxworks-20160315.patch
rm -rf samples
rm buildlib.sh gatewayclient.c gatewayclient.h lib/MQTTLinux.c lib/MQTTLinux.h
