#include "ft_ping.h"

void usage(FILE *out, const char *prog)
{
    fprintf(out,
        "Usage: %s [OPTION...] HOST\n"
        "Send ICMP ECHO_REQUEST packets to network hosts.\n\n"
        "  -v              verbose output\n"
        "  -?              give this help list\n\n",
        prog);
}

int parse_options(int argc, char **argv, t_options *opts)
{
    int opt;

    memset(opts, 0, sizeof(*opts));
    opterr = 0;
    while ((opt = getopt(argc, argv, "v?")) != -1)
    {
        if (opt == 'v')
            opts->verbose = true;
        else if (opt == '?')
        {
            if (optopt != 0)
            {
                fprintf(stderr, "%s: invalid option -- '%c'\n", argv[0], optopt);
                usage(stderr, argv[0]);
                return -1;
            }
            opts->help = true;
            return 0;
        }
        else
        {
            usage(stderr, argv[0]);
            return -1;
        }
    }
    if (opts->help)
        return 0;
    if (optind >= argc)
    {
        fprintf(stderr, "%s: missing host operand\n", argv[0]);
        usage(stderr, argv[0]);
        return -1;
    }
    if (optind + 1 != argc)
    {
        fprintf(stderr, "%s: extra operand '%s'\n", argv[0], argv[optind + 1]);
        usage(stderr, argv[0]);
        return -1;
    }
    opts->target = argv[optind];
    return 0;
}
