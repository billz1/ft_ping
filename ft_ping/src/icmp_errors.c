#include "ft_ping.h"

const char *icmp_type_string(int type, int code)
{
    if (type == ICMP_DEST_UNREACH)
    {
        if (code == ICMP_NET_UNREACH)
            return "Destination Net Unreachable";
        if (code == ICMP_HOST_UNREACH)
            return "Destination Host Unreachable";
        if (code == ICMP_PROT_UNREACH)
            return "Destination Protocol Unreachable";
        if (code == ICMP_PORT_UNREACH)
            return "Destination Port Unreachable";
        if (code == ICMP_FRAG_NEEDED)
            return "Frag needed and DF set";
        if (code == ICMP_SR_FAILED)
            return "Source Route Failed";
        return "Destination Unreachable";
    }
    if (type == ICMP_TIME_EXCEEDED)
    {
        if (code == ICMP_EXC_TTL)
            return "Time to live exceeded";
        if (code == ICMP_EXC_FRAGTIME)
            return "Fragment reassembly time exceeded";
        return "Time exceeded";
    }
    if (type == ICMP_PARAMETERPROB)
        return "Parameter problem";
    if (type == ICMP_REDIRECT)
        return "Redirect";
    if (type == ICMP_ECHOREPLY)
        return "Echo reply";
    return "ICMP error";
}
