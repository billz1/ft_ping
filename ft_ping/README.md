# ft_ping

A C implementation of the mandatory 42 `ft_ping` project.

## Build

```sh
make
```

## Run

Raw ICMP sockets require root privileges or `CAP_NET_RAW`:

```sh
sudo ./ft_ping 8.8.8.8
sudo ./ft_ping google.com
sudo ./ft_ping -v 8.8.8.8
./ft_ping -?
```

Alternative without `sudo` after compilation:

```sh
sudo setcap cap_net_raw+ep ./ft_ping
./ft_ping 8.8.8.8
```

Stop with `Ctrl-C` to print statistics.

## Implemented subject requirements

- Executable name: `ft_ping`
- Written in C with a Makefile and the usual rules: `all`, `clean`, `fclean`, `re`
- Uses raw ICMP sockets directly; it does not call the system `ping`
- Supports `-v` and `-?`
- Accepts IPv4 addresses and hostnames/FQDNs
- Does not do reverse DNS resolution in packet replies; reply lines print numeric IPv4 addresses
- Handles errors without crashing

Test 1 — Clean build
make fclean
make

Expected: no compiler errors.

Then test Makefile rules:

make
make clean
make fclean
make re

Important: the subject requires a Makefile with the usual rules and recompilation only when necessary.

Test 2 — Help option
./ft_ping -?

Expected: help/usage output.

The subject specifically requires -?.

Test 3 — Basic IP ping
sudo ./ft_ping 8.8.8.8

or if you used setcap:

./ft_ping 8.8.8.8

Let it send 4 or 5 packets, then press:

Ctrl + C

Expected:

--- 8.8.8.8 ping statistics ---
... packets transmitted, ... packets received, ...% packet loss
round-trip min/avg/max/stddev = ...
Test 4 — Hostname / FQDN
sudo ./ft_ping google.com

or:

./ft_ping google.com

The subject requires IPv4 address/hostname support and FQDN handling.

Test 5 — Verbose mode
sudo ./ft_ping -v 8.8.8.8

or:

./ft_ping -v 8.8.8.8

Then test a bad/unreachable address:

sudo ./ft_ping -v 10.255.255.1

Stop with Ctrl + C.

The subject says -v should help show results when there is a packet-related problem or error.

Test 6 — Error handling

Run these:

./ft_ping
./ft_ping -x
./ft_ping invalid-host-name-123456789.com
sudo ./ft_ping 999.999.999.999

Expected: clean error messages.

Not acceptable:

Segmentation fault
Bus error
double free

The subject explicitly says the program must handle errors carefully and must not quit unexpectedly.
