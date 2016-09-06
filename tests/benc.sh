#!/bin/sh
set -e
AEC=../src/aec
test_data=https://gitlab.dkrz.de/k202009/libaec/raw/master/data/typical.rz
if [ ! -f  typical.dat ]; then
    rm -f typical.rz
    type curl >/dev/null 2>&1 || {
        echo >&2 "curl not found. Please download $test_data by other means and place it in tests. Aborting."
        exit 1
    }
    curl $test_data -O || {
        echo >&2 "Could not download $test_data. Please download it by other means and place it in tests. Aborting."
        exit 1
    }
    $AEC -d -n16 -j64 -r256 -m typical.rz typical.dat
    rm -f bench.dat
fi
if [ ! -f  bench.dat ]; then
    for i in $(seq 0 499);
    do
        cat typical.dat >> bench.dat
    done
fi
rm -f bench.rz
utime=$(../src/utime $AEC -n16 -j64 -r256 -m bench.dat bench.rz 2>&1)
bsize=$(wc -c bench.dat | awk '{print $1}')
perf=$(echo "$bsize/1048576/$utime" | bc)
echo "[0;32m*** Encoding with $perf MiB/s user time ***[0m"
