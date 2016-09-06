#!/bin/sh
set -e
AEC=../src/aec
CCSDS_DATA=121B2TestData
ALLO=${CCSDS_DATA}/AllOptions
EXTP=${CCSDS_DATA}/ExtendedParameters
LOWE=${CCSDS_DATA}/LowEntropyOptions
archive=121B2TestData.zip
archive_url=http://cwe.ccsds.org/sls/docs/SLS-DC/BB121B2TestData/$archive
if [ ! -f $archive ]; then
    type curl >/dev/null 2>&1 || {
        echo >&2 "curl not found. Please download $archive_url by other means and place it in tests.\nAborting."
        exit 1
    }
    curl $archive_url -O || {
        echo >&2 "Could not download $archive_url. Please download $archive by other means and place it in tests. Aborting."
        exit 1
    }
fi
type unzip >/dev/null 2>&1 || {
    echo >&2 "unzip not found. Please install unzip or unpack $archive in tests.\nAborting."
    exit 1
}
unzip -oq $archive

filesize () {
    wc -c $1 | awk '{print $1}'
}

decode () {
    $AEC -d $3 $1 test.dat
    cmp -n $(filesize $2) $2 test.dat
}

code () {
    $AEC $3 $2 test.rz
    cmp $1 test.rz
}

code_size () {
    $AEC $3 $2 test.rz
    if [ ! $(filesize test.rz) -eq $(filesize $1) ]; then
        echo "$1 size mismatch"
        exit 1
    fi
}

codec () {
    code "$@"
    decode "$@"
}

cosdec () {
    code_size "$@"
    decode "$@"
}

echo All Options
mv -f ${ALLO}/test_P512n22.dat ${ALLO}/test_p512n22.dat
for i in 01 02 03 04
do
    uf=$ALLO/test_p256n${i}.dat
    codec $ALLO/test_p256n${i}-basic.rz $uf "-n$i -j16 -r16"
    codec $ALLO/test_p256n${i}-restricted.rz $uf "-n$i -j16 -r16 -t"
done
for i in 05 06 07 08 09 10 11 12 13 14 15 16
do
    cosdec $ALLO/test_p256n${i}.rz $ALLO/test_p256n${i}.dat \
        "-n$i -j16 -r16"
done
for i in 17 18 19 20 21 22 23 24
do
    cosdec $ALLO/test_p512n${i}.rz $ALLO/test_p512n${i}.dat \
        "-n$i -j16 -r32"
done

echo Low Entropy Options
for i in 1 2 3
do
    for j in 01 02 03 04
    do
        uf=$LOWE/Lowset${i}_8bit.dat
        codec $LOWE/Lowset${i}_8bit.n${j}-basic.rz $uf "-n$j -j16 -r64"
        codec $LOWE/Lowset${i}_8bit.n${j}-restricted.rz $uf "-n$j -j16 -r64 -t"
    done
    for j in 05 06 07 08
    do
        codec $LOWE/Lowset${i}_8bit.n${j}.rz $LOWE/Lowset${i}_8bit.dat \
            "-n$j -j16 -r64"
    done
done

echo Extended Parameters
decode $EXTP/sar32bit.j16.r256.rz $EXTP/sar32bit.dat "-n32 -j16 -r256 -p"
decode $EXTP/sar32bit.j64.r4096.rz $EXTP/sar32bit.dat "-n32 -j64 -r4096 -p"
