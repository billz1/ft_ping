#ifndef FT_PING_H
# define FT_PING_H

# include <arpa/inet.h>
# include <errno.h>
# include <math.h>
# include <netdb.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <signal.h>
# include <stdbool.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/select.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <sys/types.h>
# include <time.h>
# include <unistd.h>

# define FT_PING_VERSION "ft_ping 1.0"
# define DATA_SIZE 56
# define PACKET_SIZE (sizeof(struct icmphdr) + DATA_SIZE)
# define RECV_BUF_SIZE 1500
# define DEFAULT_INTERVAL_SEC 1

extern volatile sig_atomic_t g_running;

typedef struct s_options
{
    bool verbose;
    bool help;
    const char *target;
}   t_options;

typedef struct s_stats
{
    uint16_t seq;
    unsigned long transmitted;
    unsigned long received;
    unsigned long errors;
    double min_rtt;
    double max_rtt;
    double sum_rtt;
    double sumsq_rtt;
    struct timeval start_time;
}   t_stats;

typedef struct s_ping
{
    int sockfd;
    int ident;
    t_options opts;
    struct sockaddr_in dest;
    char ip_str[INET_ADDRSTRLEN];
    t_stats stats;
}   t_ping;

void        usage(FILE *out, const char *prog);
int         parse_options(int argc, char **argv, t_options *opts);
int         resolve_target(const char *target, struct sockaddr_in *dest, char ip_str[INET_ADDRSTRLEN]);
uint16_t    icmp_checksum(const void *data, size_t len);
double      time_diff_ms(const struct timeval *start, const struct timeval *end);
void        init_stats(t_stats *stats);
void        update_rtt_stats(t_stats *stats, double rtt);
void        print_statistics(const t_ping *ping);
int         create_socket(void);
int         send_echo_request(t_ping *ping);
int         receive_packets(t_ping *ping, int timeout_ms);
const char  *icmp_type_string(int type, int code);
void        run_ping(t_ping *ping);

#endif
