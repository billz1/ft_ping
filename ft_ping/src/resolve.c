#include "ft_ping.h"

int resolve_target(const char *target, struct sockaddr_in *dest, char ip_str[INET_ADDRSTRLEN])
{
    struct addrinfo hints;
    struct addrinfo *res;
    int ret;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;
    ret = getaddrinfo(target, NULL, &hints, &res);
    if (ret != 0)
    {
        fprintf(stderr, "ft_ping: unknown host: %s\n", target);
        return -1;
    }
    memcpy(dest, res->ai_addr, sizeof(struct sockaddr_in));
    if (inet_ntop(AF_INET, &dest->sin_addr, ip_str, INET_ADDRSTRLEN) == NULL)
    {
        perror("ft_ping: inet_ntop");
        freeaddrinfo(res);
        return -1;
    }
    freeaddrinfo(res);
    return 0;
}
