#include "ft_ping.h"

static void fill_payload(unsigned char *payload, size_t len)
{
    struct timeval now;
    size_t i;

    gettimeofday(&now, NULL);
    memset(payload, 0, len);
    memcpy(payload, &now, sizeof(now) < len ? sizeof(now) : len);
    i = sizeof(now);
    while (i < len)
    {
        payload[i] = (unsigned char)(i & 0xff);
        i++;
    }
}

int send_echo_request(t_ping *ping)
{
    unsigned char packet[PACKET_SIZE];
    struct icmphdr *icmp;
    ssize_t sent;

    memset(packet, 0, sizeof(packet));
    icmp = (struct icmphdr *)packet;
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id = htons((uint16_t)ping->ident);
    icmp->un.echo.sequence = htons(++ping->stats.seq);
    fill_payload(packet + sizeof(struct icmphdr), DATA_SIZE);
    icmp->checksum = 0;
    icmp->checksum = icmp_checksum(packet, sizeof(packet));
    sent = sendto(ping->sockfd, packet, sizeof(packet), 0,
        (struct sockaddr *)&ping->dest, sizeof(ping->dest));
    if (sent < 0)
    {
        perror("ft_ping: sendto");
        return -1;
    }
    ping->stats.transmitted++;
    return 0;
}

static int packet_belongs_to_us(const struct icmphdr *icmp, size_t icmp_len, int ident)
{
    const unsigned char *inner;
    const struct iphdr *inner_ip;
    size_t inner_ip_len;
    const struct icmphdr *inner_icmp;

    if (icmp->type == ICMP_ECHOREPLY)
        return ntohs(icmp->un.echo.id) == (uint16_t)ident;
    if (icmp_len < sizeof(struct icmphdr) + sizeof(struct iphdr) + sizeof(struct icmphdr))
        return 0;
    inner = (const unsigned char *)icmp + sizeof(struct icmphdr);
    inner_ip = (const struct iphdr *)inner;
    inner_ip_len = (size_t)inner_ip->ihl * 4;
    if (inner_ip_len < sizeof(struct iphdr) || icmp_len < sizeof(struct icmphdr) + inner_ip_len + sizeof(struct icmphdr))
        return 0;
    inner_icmp = (const struct icmphdr *)(inner + inner_ip_len);
    return ntohs(inner_icmp->un.echo.id) == (uint16_t)ident;
}

static uint16_t extract_sequence(const struct icmphdr *icmp, size_t icmp_len)
{
    const unsigned char *inner;
    const struct iphdr *inner_ip;
    size_t inner_ip_len;
    const struct icmphdr *inner_icmp;

    if (icmp->type == ICMP_ECHOREPLY)
        return ntohs(icmp->un.echo.sequence);
    inner = (const unsigned char *)icmp + sizeof(struct icmphdr);
    inner_ip = (const struct iphdr *)inner;
    inner_ip_len = (size_t)inner_ip->ihl * 4;
    if (icmp_len < sizeof(struct icmphdr) + inner_ip_len + sizeof(struct icmphdr))
        return 0;
    inner_icmp = (const struct icmphdr *)(inner + inner_ip_len);
    return ntohs(inner_icmp->un.echo.sequence);
}

static void handle_echo_reply(t_ping *ping, const unsigned char *packet, ssize_t len,
    const struct iphdr *ip, const struct icmphdr *icmp, const char *from)
{
    const unsigned char *payload;
    struct timeval sent_at;
    struct timeval now;
    double rtt;
    size_t ip_len;
    size_t icmp_len;

    ip_len = (size_t)ip->ihl * 4;
    icmp_len = (size_t)len - ip_len;
    if (icmp_len < sizeof(struct icmphdr) + sizeof(struct timeval))
        return;
    payload = packet + ip_len + sizeof(struct icmphdr);
    memcpy(&sent_at, payload, sizeof(sent_at));
    gettimeofday(&now, NULL);
    rtt = time_diff_ms(&sent_at, &now);
    ping->stats.received++;
    update_rtt_stats(&ping->stats, rtt);
    printf("%zd bytes from %s: icmp_seq=%u ttl=%u time=%.3f ms\n",
        icmp_len, from, ntohs(icmp->un.echo.sequence), ip->ttl, rtt);
    fflush(stdout);
}

static void handle_verbose_error(t_ping *ping, const struct sockaddr_in *from_addr,
    const struct icmphdr *icmp, size_t icmp_len)
{
    char from[INET_ADDRSTRLEN];
    uint16_t seq;

    if (!packet_belongs_to_us(icmp, icmp_len, ping->ident))
        return;
    seq = extract_sequence(icmp, icmp_len);
    if (inet_ntop(AF_INET, &from_addr->sin_addr, from, sizeof(from)) == NULL)
        strcpy(from, "unknown");
    ping->stats.errors++;
    printf("From %s: icmp_seq=%u %s\n", from, seq,
        icmp_type_string(icmp->type, icmp->code));
    fflush(stdout);
}

static int process_packet(t_ping *ping, const unsigned char *packet, ssize_t len,
    const struct sockaddr_in *from_addr)
{
    const struct iphdr *ip;
    const struct icmphdr *icmp;
    size_t ip_len;
    size_t icmp_len;
    char from[INET_ADDRSTRLEN];

    if (len < (ssize_t)(sizeof(struct iphdr) + sizeof(struct icmphdr)))
        return 0;
    ip = (const struct iphdr *)packet;
    ip_len = (size_t)ip->ihl * 4;
    if (ip_len < sizeof(struct iphdr) || len < (ssize_t)(ip_len + sizeof(struct icmphdr)))
        return 0;
    icmp = (const struct icmphdr *)(packet + ip_len);
    icmp_len = (size_t)len - ip_len;
    if (!packet_belongs_to_us(icmp, icmp_len, ping->ident))
        return 0;
    if (icmp->type == ICMP_ECHOREPLY)
    {
        if (inet_ntop(AF_INET, &from_addr->sin_addr, from, sizeof(from)) == NULL)
            strcpy(from, ping->ip_str);
        handle_echo_reply(ping, packet, len, ip, icmp, from);
        return 1;
    }
    if (ping->opts.verbose)
        handle_verbose_error(ping, from_addr, icmp, icmp_len);
    return 0;
}

int receive_packets(t_ping *ping, int timeout_ms)
{
    fd_set rfds;
    struct timeval timeout;
    unsigned char buffer[RECV_BUF_SIZE];
    struct sockaddr_in from_addr;
    socklen_t from_len;
    ssize_t len;
    int ret;

    FD_ZERO(&rfds);
    FD_SET(ping->sockfd, &rfds);
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;
    ret = select(ping->sockfd + 1, &rfds, NULL, NULL, &timeout);
    if (ret < 0)
    {
        if (errno == EINTR)
            return 0;
        perror("ft_ping: select");
        return -1;
    }
    if (ret == 0)
        return 0;
    from_len = sizeof(from_addr);
    len = recvfrom(ping->sockfd, buffer, sizeof(buffer), 0,
        (struct sockaddr *)&from_addr, &from_len);
    if (len < 0)
    {
        if (errno == EINTR)
            return 0;
        perror("ft_ping: recvfrom");
        return -1;
    }
    return process_packet(ping, buffer, len, &from_addr);
}

void run_ping(t_ping *ping)
{
    struct timeval before;
    struct timeval after;
    double elapsed;
    int remaining_ms;

    while (g_running)
    {
        gettimeofday(&before, NULL);
        send_echo_request(ping);
        remaining_ms = DEFAULT_INTERVAL_SEC * 1000;
        while (g_running && remaining_ms > 0)
        {
            gettimeofday(&after, NULL);
            elapsed = time_diff_ms(&before, &after);
            remaining_ms = DEFAULT_INTERVAL_SEC * 1000 - (int)elapsed;
            if (remaining_ms <= 0)
                break;
            if (receive_packets(ping, remaining_ms) < 0)
                break;
        }
    }
}
