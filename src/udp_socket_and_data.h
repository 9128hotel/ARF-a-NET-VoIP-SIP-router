#ifndef UDP_SOCKET_AND_DATA_H
#define UDP_SOCKET_AND_DATA_H

#include "main.h"

int socket_attain();
int recv_packet(int sockfd, raw_packet* received_packet);

#endif
