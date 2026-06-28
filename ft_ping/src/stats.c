#include "ft_ping.h"

void init_stats(t_stats *stats)
{
    memset(stats, 0, sizeof(*stats));
    stats->min_rtt = -1.0;
    gettimeofday(&stats->start_time, NULL);
}

double time_diff_ms(const struct timeval *start, const struct timeval *end)
{
    double sec;
    double usec;

    sec = (double)(end->tv_sec - start->tv_sec) * 1000.0;
    usec = (double)(end->tv_usec - start->tv_usec) / 1000.0;
    return sec + usec;
}

void update_rtt_stats(t_stats *stats, double rtt)
{
    if (stats->min_rtt < 0.0 || rtt < stats->min_rtt)
        stats->min_rtt = rtt;
    if (rtt > stats->max_rtt)
        stats->max_rtt = rtt;
    stats->sum_rtt += rtt;
    stats->sumsq_rtt += rtt * rtt;
}

void print_statistics(const t_ping *ping)
{
    struct timeval end;
    double elapsed;
    double loss;

    gettimeofday(&end, NULL);
    elapsed = time_diff_ms(&ping->stats.start_time, &end);
    loss = 0.0;
    if (ping->stats.transmitted > 0)
        loss = 100.0 - ((double)ping->stats.received * 100.0 / (double)ping->stats.transmitted);
    printf("\n--- %s ping statistics ---\n", ping->opts.target);
    printf("%lu packets transmitted, %lu packets received, %.0f%% packet loss\n",
        ping->stats.transmitted, ping->stats.received, loss);
    if (ping->stats.received > 0)
    {
        double avg = ping->stats.sum_rtt / (double)ping->stats.received;
        double variance = ping->stats.sumsq_rtt / (double)ping->stats.received - avg * avg;
        double stddev = variance > 0.0 ? sqrt(variance) : 0.0;
        printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
            ping->stats.min_rtt, avg, ping->stats.max_rtt, stddev);
    }
    (void)elapsed;
}
