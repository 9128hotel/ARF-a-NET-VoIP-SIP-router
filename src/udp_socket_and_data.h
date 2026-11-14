#ifndef UDP_H
#define UDP_H

#include "main.h"

int socket_attain();
int recv_packet(int sockfd, raw_packet* received_packet);

#endif
