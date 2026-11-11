#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#define MAX_IPV4_UDP_PAYLOAD_SIZE 65535 // this is based on the UDP header's 16 bit length field, which can represent up to 65535 bytes (including header) THIS IS NOT THE MAXIMUM FOR IPV6 BECAUSE OF JUMBOS

typedef struct {
    struct sockaddr_in source_ip;
    size_t length;
    unsigned char *data;
} raw_packet;

int socket_attain() {
    int sockfd = socket( // request socket (as file descriptor)
        AF_INET, // IPv4
        SOCK_DGRAM, // connectionless datagram socket
        0 // default for this combo (which is UDP)
    );

    if (sockfd < 0) { // socket returns -1 for an error
        perror("[socket_attain: ERROR] Failed to get a socket\n"); // print an error (text is error prefix)
        return -1; // exit
    }

    struct sockaddr_in addr; // create an instance of sockaddr_in called addr
    memset(&addr, 0, sizeof(addr)); // zero the structure so no garbage is sent to kernel
    addr.sin_family = AF_INET; // IPv4
    addr.sin_port  = htons(5060); // port 5060 (htons "host to network short" converts number to big endian, which is standard for networking)
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // IP address. INADDR_ANY = 0.0.0.0, htonl is not needed here because 0.0.0.0 is always 0 but important if you want to change IP to eg 127.0.0.1 for internal only

    if (bind(sockfd, (struct sockaddr*) &addr, sizeof(addr)) < 0) { // bind the socket and address together
        perror("[socket_attain: ERROR] Failed to bind a socket"); // if that fails, print an error
        close(sockfd); // and close the socket
        return -1; // exit
    }

    return sockfd;
}


int recv_packet(int sockfd, raw_packet* received_packet) {
    unsigned char* packet_data = (unsigned char*)malloc(MAX_IPV4_UDP_PAYLOAD_SIZE);
    if (packet_data == NULL) {
        perror("[recv_packet: ERROR] Failed to calloc");
        return -1;
    };

    struct sockaddr_in source_ip_addr = { 0 }; // init and zero an IPv4 address structure
    socklen_t source_ip_addr_len = sizeof source_ip_addr; // an io variable we have to init for recvfrom to tell us how big the IPv4 address ended up being (this should not change, but by spec it may)

    ssize_t len;
    do {
        len = recvfrom(
            sockfd, // the udp packet to receive from
            packet_data, // where to put the datagrams payload
            MAX_IPV4_UDP_PAYLOAD_SIZE, // data's maximum capacity
            0, // we are requesting no special receive flags
            (struct sockaddr *)&source_ip_addr, // where to put the senders IP address
            &source_ip_addr_len // how long that IP address is (specifically passing the address of the size variable, turning a value into a pointer using "&"
        );
    } while (len < 0 && errno == EINTR);

    if (len < 0) {
        perror("[recv_packet: ERROR] Failed to receive a packet properly");
        free(packet_data);
        return -1;
    }

    received_packet->source_ip = source_ip_addr;
    received_packet->length = (size_t)len;
    received_packet->data = packet_data;
    return 0;
}

int main() {
    // in this file we assume:
    //      char is exactly 1 octet
    //      unsigned int is at least 1 octet
    int in_socket = socket_attain(); // file descriptor socket
    int out_socket = in_socket; // this is for stand-alone mode, if you'd like to chain multiple programs together change this or the input socket

    if (in_socket < 0 || out_socket < 0) {
        return -1;
    }

    while (true) {
        raw_packet received_packet = (raw_packet){ 0 };

        if (recv_packet(in_socket, &received_packet) < 0) {
            continue;
        }

        printf("from %s:%u, %zu bytes\n", inet_ntoa(received_packet.source_ip.sin_addr), (unsigned)ntohs(received_packet.source_ip.sin_port), received_packet.length);

        // there will eventually be a return call in here to exit, but not here rn, TODO

        free(received_packet.data);
    }

    close(in_socket); // close and free the socket
    return 0; // exit
}
