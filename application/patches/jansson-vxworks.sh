#!/bin/bash

autoreconf -i
./configure
git apply ../application/patches/jansson-vxworks-20160413.patch
rm -rf doc test examples
