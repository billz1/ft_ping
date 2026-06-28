#include "ft_ping.h"

int create_socket(void)
{
    int sockfd;
    int ttl;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        if (errno == EPERM || errno == EACCES)
            fprintf(stderr, "ft_ping: socket: operation not permitted. Run as root or set CAP_NET_RAW.\n");
        else
            perror("ft_ping: socket");
        return -1;
    }
    ttl = 64;
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
        perror("ft_ping: setsockopt IP_TTL");
    return sockfd;
}
