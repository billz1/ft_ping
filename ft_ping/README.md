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
