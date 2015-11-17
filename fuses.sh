#!/bin/bash

chip=${1}
device="usbtiny"
port="usb"
fmt="b"

function toHex() {
    bin=`echo $1 | cut -d'b' -f2`
    s="/tmp/toHex.$$.bc"
    {
        echo "obase=16"
        echo "ibase=2"
        echo "${bin}"
        echo "quit"
    } > $s
    hex=`bc -q $s`
    rm -rf $s

    echo -n "$hex"
}

function displayFuse() {
    lbl=${1}
    file=${2}
    bin=`cat ${file}`
    echo -n "${lbl}: ${bin} (0x"
    toHex ${bin}
    echo -n ") "
}

function displayFuses() {
    fH="/tmp/avrfuses.$$.high.bin"
    fL="/tmp/avrfuses.$$.low.bin"
    fE="/tmp/avrfuses.$$.extended.bin"

    avrdude -p ${chip} -P ${port} -c ${device} -U hfuse:r:${fH}:${fmt} -U lfuse:r:${fL}:${fmt} -U efuse:r:${fE}:${fmt}

    echo "================================ FUSES ================================"
    displayFuse "LOW" ${fL}
    displayFuse "HIGH" ${fH}
    [ -f ${fE} ] && displayFuse "EXT" ${fE}
    echo ""
    echo "======================================================================="

    rm -rf ${fileH} ${fileL} ${fileE}
}

displayFuses
