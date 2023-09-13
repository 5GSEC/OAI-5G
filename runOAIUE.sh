#!/bin/bash

_usage()
{
    cat <<_EOF
$(basename "$0") [-h|--help] [-d|--debug]
           [-U|--use-core <core #>] [-C|--config <5G config>]
           [-L|--log <output log file>] [-p|--pcap [--ts[stdout] <t_stdout>]
           [-b|--branch <branch]
           [<mode>] [<network type>]
           [-- ...]

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

  <t_stdout>, which must be specified before --pcap, is the value of T_stdout (default: $_t_stdout):

    0 disable console logging and send messages to T-Tracer
    1 enable console logging and do not send messages to T-Tracer
    2 enable console logging and send messages to T-Tracer

  <output log file> is the output file for the console log (default: "$_cLog"), and

  arguments following '--' are passed to lte-softmodem.<mode>

  Optional environment symbols:

    CFREQ       center frequency (default: $_cfreq)
    RXGAIN      receive gain (default: $_rxgain)
    TXGAIN      transmit gain (default: $_txgain)
    PRB         physical resource blocks (default: $_prb)

Example:

  $(basename "$0") blind -- -g 1 --TMSI 314152
_EOF
}


_BASE_DIR=$(cd $(dirname ${BASH_SOURCE[0]}) >/dev/null 2>&1 && pwd)
cd ${_BASE_DIR}

_cliArgs="$*"

_network='lte'
_ueCLIArgs='--ue-scan-carrier'

_config5G="${_BASE_DIR}/../nr-ue.conf"

_mode="master"

_t_stdout='1'

_cLog='/tmp/oaiUE.log'

_cfreq=${CFREQ:-2630000000}
_rxgain=${RXGAIN:-120}
_txgain=${TXGAIN:-0}
_maxpower=${MAXPOWER:-0}
_prb=${PRB:-25}

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
            ;;
        -C|--config)
            _config5G=$1; shift
            if [ "$_network" == 'nr' ]; then
                echo 'ERROR: --config must be specified before nr.' >&2
                exit 1
            fi
            ;;
        5g|5G|NR|nr)
            _network='nr'
            if [ -z "$CFREQ" ]; then
                _cfreq=3309480000       # 3300300000
                _prb=51
                _cfreq=3619200000
                _prb=106
            fi
            if [ -z "$_PRB" ]; then
                _prb=51
                _prb=106
            fi
            # Per https://gitlab.eurecom.fr/oai/openairinterface5g/blob/develop/doc/RUNMODEM.md#sa-setup-with-oai-nr-ue
            _ueCLIArgs='--sa --ue-fo-compensation'
            # Per https://gitlab.eurecom.fr/oai/openairinterface5g/-/blob/develop/doc/TESTING_GNB_W_COTS_UE.md
            _ueCLIArgs="$_ueCLIArgs -E"
            _ueCLIArgs="$_ueCLIArgs -O $(realpath -e $_config5G)"
            ;;
        -U|--use-core)
            _runPrefix="taskset -c $1"; shift
            ;;
        -p|--pcap)
            _pcapOpt="--T_stdout $_t_stdout --T_nowait"
            ;;
        --ts*)
            _t_stdout="$1"; shift
            ;;
        -L|--log)
            _cLog="$1"; shift
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

if ! touch "$_cLog" > /dev/null 2>&1; then
    echo "ERROR: \"$_cLog\" is not writable."
    exit 2
fi

_ueCLIArgs="$_ueCLIArgs $_pcapOpt $*"
_ueCLIArgs="$_ueCLIArgs --log_config.global_log_options level"
if [ "$_mode" == 'develop' ]; then
    _moreLogOpts='time'
else
    _moreLogOpts='thread,local_clock'
fi
_ueCLIArgs="$_ueCLIArgs,$_moreLogOpts"

_ueSMmode=${_network}-uesoftmodem.$_mode
_ueSoftModem=$(find ${_BASE_DIR} -type f -name $_ueSMmode)

_prefix=/tmp/$(date +%Y%m%d%H%M)-${_network}-$_mode
_outLog=$_prefix.log
_optPath=$_prefix.pcap


_checkRun()     # <iferr func>
{
    if [ -n "$_ueSoftModem" ]; then
        $_debug cd $(dirname "$_ueSoftModem")
	if [ "$_network" == 'lte' ]; then
	    _netOpts="--ue-rxgain $_rxgain --ue-txgain $_txgain --ue-max-power $_maxpower --ue-scan-carrier --nokrnmod 1"
	else
            # Per eurolab-openairinterface5g.git/docker/Dockerfile.nrUE.ubuntu20
	    _netOpts='--numerology 1'
	fi
        $_debug $_runPrefix ./$_ueSMmode -C $_cfreq -r $_prb $_netOpts --opt.type pcap --opt.path $_optPath $_ueCLIArgs 2>&1 | tee "$_outLog"
    else
        $1
    fi

}

_buildUE()
{
    echo "WARNING: \"$_ueSMmode\" not found.  Building."
    ./EKBuildOAIUE.sh $_cliArgs

    _checkRun "_buildErrorExit"
}

_buildErrorExit()
{
    echo "ERROR: \"$_ueSMmode\" not found."
    exit 2
}

_checkRun "_buildUE"
