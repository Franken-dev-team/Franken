#!/usr/bin/sh

cd build
cmake ..
cmake --build .
echo "package has been built!"
exit 0
