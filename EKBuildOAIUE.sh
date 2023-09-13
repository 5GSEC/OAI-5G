#!/bin/bash

_usage()
{
    cat <<_EOF
$(basename "$0") [-h|--help] [-d|--debug] [-b|--branch <branch] [<mode>] [<network type>] [-- ...]

  where <mode> is

    0 or mas[ter] (default)
    1 or tt[racer]
    2 or dev[elop]
    3 or bts[_resource_depletion]
    4 or blind[_dos]
    5 or att[ack]

  <network type> is

    4G | 4g | LTE | lte (default)
    5G | 5g | NR  | nr
_EOF
}


_BASE_DIR=$(cd $(dirname ${BASH_SOURCE[0]}) >/dev/null 2>&1 && pwd)
cd ${_BASE_DIR}

_mode="master"

_network='lte'
_ueCLIType='UE'

while [ -n "$1" ]; do
    _arg="$1"; shift
    case "$_arg" in
        0|mas*)
            _mode="master"
            ;;
        1|tt*)
            _mode="ttracer"
            ;;
        2|dev*)
            _mode="develop"
            ;;
        3|bts*)
            _mode="bts_resource_depletion"
            ;;
        4|blind*)
            _mode="blind_dos"
            ;;
        5|att*)
            _mode="attack"
            ;;
        -b|--branch)
            _mode=$1
            shift
            ;;
        4g|4G|LTE|lte)
            _network='lte'
            _ueCLIType='UE'
            ;;
        5g|5G|NR|nr)
            _network='nr'
            _ueCLIType='nrUE'
            ;;
        -d|--debug)
            _debug=echo
            ;;
        -h|--help)
            _usage
            exit 0
            ;;
        --)
            break
            ;;
        *)
            echo "ERROR: unrecognized option: \"$_arg\"."
            exit 1
            ;;
    esac
done

$_debug git checkout $_mode

if [ ! -d ${_BASE_DIR}/.EK257 ]; then
    $_debug git checkout attack -- .EK257 runOAIUE.sh ue_ping.sh
fi

$_debug source oaienv
$_debug cd cmake_targets

_buildOAIOpts='--cmake-opt -DASN1C_EXEC=/opt/asn1c/bin/asn1c'
if [ ! -f '/opt/asn1c/bin/asn1c' ]; then
    _buildOAIOpts="-I $_buildOAIOpts"
fi

date
$_debug ./build_oai --${_ueCLIType} $_buildOAIOpts -w USRP --ninja --noavx512 $@
date

_ueSM=${_network}-uesoftmodem
_ueSMmode=$_ueSM.$_mode
_ueSoftModem=$(find ${_BASE_DIR} -type f -name $_ueSM)

if [ -n "$_ueSoftModem" ]; then
    $_debug mv -f "$_ueSoftModem" "$_ueSoftModem.$_mode"
else
    echo "ERROR: \"$_ueSMmode\" not found."
    $_debug exit 2
fi

$_debug cp -f ${_BASE_DIR}/.EK257/.u* $(dirname "$_ueSoftModem")
