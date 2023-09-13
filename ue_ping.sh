#!/bin/bash

_TUN='oaitun_ue1'

_inWait=''

while /bin/true; do
    if grep -qE "^$_TUN" /proc/net/route; then
	if [ -n "$_inWait" ]; then
	    _inWait=''
	    echo
	fi
	ping -I $_TUN -D -c 1 8.8.8.8 | grep -E '(packet loss)|icmp_seq'
    else
	if [ -z "$_inWait" ]; then
	    echo -n "...waiting for $_TUN"
	    _inWait="y"
	else
	    echo -n "."
	fi
    fi
    sleep 2
done
