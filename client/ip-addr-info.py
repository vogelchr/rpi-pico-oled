#!/usr/bin/python
import argparse
import ipaddress
from pathlib import Path
import time
import netifaces
import socket
import serial
import datetime

af_num_name = [(17, 'hardware'), (2, 'IPv4'), (10, 'IPv6')]


def repeat_endless_with_flag(gen):
    accu = list()

    if gen is None :
        while True :
            yield False, None

    for val in gen:
        accu.append(val)
        yield True, val

    if len(accu) == 0:
        while True:
            yield False, None

    while True:
        for val in accu:
            yield False, val


def repeat_until_all_exhausted(*generators):
    iters = list()

    for gen in generators:
        iters.append(iter(repeat_endless_with_flag(gen)))

    while True:
        running_flags = list()
        values_list = list()
        for it in iters:
            run, val = next(it)
            running_flags.append(run)
            values_list.append(val)
        if not any(running_flags):
            break
        yield values_list


def list_or_none_by_key(d, keys):
    ret = list()
    for k in keys:
        ret.append()


def concat_from_dict_with_prefix(d, pfx_keys) :
    for pfx, k in pfx_keys :
        if k in d :
            for val in d[k] :
                yield pfx, val

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-t', '--tty', type=Path, default='/dev/ttyACM0',
                        help='serial port to write to [def:%(default)s]')
    parser.add_argument('-b', '--baud', type=int, default=9600, help='baudrate [def:%(default)d]')
    args = parser.parse_args()

    tty = serial.Serial(args.tty.as_posix(), args.baud)

    while True :

        for ifname in netifaces.interfaces():
            if ifname == 'lo':
                continue
            ifinfo = netifaces.ifaddresses(ifname)

            lines = [ '' for x in range(4) ]

            for what, addrs in concat_from_dict_with_prefix(ifinfo, [('hw', 17),('ipv4',2),('ipv6',10)]) :
                datestr = datetime.datetime.now().astimezone().strftime('%H:%M:%S')
                lines[0] = f'{ifname} {what} {datestr}'
                if what == 'hw' :
                    lines[1] = addrs['addr']
                elif what == 'ipv4' :
                    cidr = ipaddress.ip_interface(addrs['addr'] + '/' + addrs['netmask'])
                    lines[1] = f'{cidr.compressed}'
                elif what == 'ipv6' :
                    addr = addrs['addr']
                    mask_str = addrs['netmask']
                    ix = mask_str.find('/')
                    cidr = ipaddress.ip_interface(addr + mask_str[ix:]).compressed

                    lines[1] = cidr[0:20]
                    if len(cidr) > 20 :
                        lines[2] = cidr[20:40]
                    if len(cidr) > 40 :
                        lines[3] = cidr[40:60]

                if not lines[2] :
                    hostname = socket.gethostname()
                    lines[2] = hostname[0:20]
                    if len(hostname) > 20 :
                        lines[3] = hostname[20:40]
                

                tty.write(b'\014')
                for i in range(4) :
                    tty.write(lines[i].encode('ascii'))
                    if i < 3 :
                        tty.write(b'\n\r')
                time.sleep(5.0)




if __name__ == '__main__':
    main()
