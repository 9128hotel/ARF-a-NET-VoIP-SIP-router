#ifndef MAIN_H
#define MAIN_H

#include <stddef.h>
#include <arpa/inet.h>

#define MAX_IPV4_UDP_PAYLOAD_SIZE 65535 // this is based on the UDP header's 16 bit length field, which can represent up to 65535 bytes (including header) THIS IS NOT THE MAXIMUM FOR IPV6 BECAUSE OF JUMBOS

typedef struct {
    struct sockaddr_in source_ip;
    size_t length;
    unsigned char *data;
} raw_packet;

#endif
