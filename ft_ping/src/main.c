#include "ft_ping.h"

volatile sig_atomic_t g_running = 1;

static void handle_sigint(int sig)
{
    (void)sig;
    g_running = 0;
}

static void setup_signal(void)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
}

int main(int argc, char **argv)
{
    t_ping ping;

    memset(&ping, 0, sizeof(ping));
    ping.sockfd = -1;
    if (parse_options(argc, argv, &ping.opts) != 0)
        return EXIT_FAILURE;
    if (ping.opts.help)
    {
        usage(stdout, argv[0]);
        return EXIT_SUCCESS;
    }
    if (resolve_target(ping.opts.target, &ping.dest, ping.ip_str) != 0)
        return EXIT_FAILURE;
    ping.sockfd = create_socket();
    if (ping.sockfd < 0)
        return EXIT_FAILURE;
    ping.ident = getpid() & 0xffff;
    init_stats(&ping.stats);
    setup_signal();
    printf("PING %s (%s): %d data bytes\n", ping.opts.target, ping.ip_str, DATA_SIZE);
    fflush(stdout);
    run_ping(&ping);
    print_statistics(&ping);
    close(ping.sockfd);
    return EXIT_SUCCESS;
}
