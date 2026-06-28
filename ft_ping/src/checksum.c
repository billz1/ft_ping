#include "ft_ping.h"

uint16_t icmp_checksum(const void *data, size_t len)
{
    const uint16_t *words;
    uint32_t sum;
    uint16_t last;

    words = (const uint16_t *)data;
    sum = 0;
    while (len > 1)
    {
        sum += *words++;
        len -= 2;
    }
    if (len == 1)
    {
        last = 0;
        *(uint8_t *)&last = *(const uint8_t *)words;
        sum += last;
    }
    while (sum >> 16)
        sum = (sum & 0xffff) + (sum >> 16);
    return (uint16_t)(~sum);
}
