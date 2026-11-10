#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int length;
    char* data;
} raw_packet;

int socket_attain() {
    int sockfd = socket( // request socket (as file descriptor)
        AF_INET, // IPv4
        SOCK_DGRAM, // connectionless datagram socket
        0 // default for this combo (which is UDP)
    );

    if (sockfd < 0) { // socket returns -1 for an error
        perror("[socket_attain: ERROR] Failed to get a socket"); // print an error (text is error prefix)
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

int filter_udp_packet(unsigned char* source, unsigned char* dest) {
    if (0 == 1) { // currently there is no filtering so this block is dead for now TODO: implement
        perror("[filter_udp_packet: ERROR] Packet from %hu to %hu failed verification", ntohl(source), ntohl(dest)); // print a warning
        return -1;
    }
    return 0;
}

int recv_packet(int sockfd, raw_packet* return_packet) {
    unsigned char header[8] = { 0 }; // make an array the length of the UDP header and initialize to zero to avoid garbage data
    recvfrom(sockfd, header, sizeof(header), MSG_PEEK, NULL, NULL) // receive the header from sockfd using the peek flag to not pop the message off the queue. we don't check return value as if it failed the packet is invalid or something is VERY wrong with the machine

    unsigned char source[2]; // define a short array to store the source of the UDP header
    memcpy(source, header, 2); // copy the source field of the UDP packet to the source variable
    unsigned char dest[2]; // define a short array to store the destination of the UDP header
    memcpy(dest, header + 2, 2); // copy the source field of the UDP packet to the source variable

    if (filter_udp_packet(source, dest) != 0) { // if the packet fails the filter (also btw this is also UDP specific (well, IP specific)) return an error
        return -1; // and exit (filter_udp_packet already printed an error)
    }

    unsigned char charlen[2]; // init a 2 bit large char which'll hold the length of the UDP packet
    memcpy(charlen, header + 4, 2); // extract the length field from the UDP packet header
    unsigned short len = (charlen[0] << 8) | charlen[1]; // quickly convert the big endian length to an unsigned short

    char* udp_packet = calloc(len, 2); // allocate some heap the size of the UDP packet (or atleast what it advertises) and zero it

    if (udp_packet == NULL) { // check if calloc has failed. if this occurs its really bad, this means you might be out of memory
        char trashbin[1] = { 0 };
        recvfrom(sockfd, trashbin, 0, 0, NULL, NULL); // drop the packet to clear it off the queue (udp should be ok with this)
        perror("[recv_packet: ERROR] calloc'ing udp_packet has failed");
        return -1;
    }

    if (recvfrom(sockfd, udp_packet, len, 0, NULL, NULL) != len) { // read the full packet now from sockfd, and without the peek flag so it does pop the packet off the queue!
        perror("A packet is lying about its size\n"); // sanity check if the packet length read and the size it reported are different. if you see this error add more checking here.
        free(udp_packet); // free the memory allocated earlier
        return -1;
    }

    return_data = calloc(len - 8, 1);

    if (return_data == NULL) { // check if calloc has failed
        perror("[recv_packet: ERROR] calloc'ing return_data has failed"); // show an error
        free(udp_packet); // free the packet
        return -1;
    }

    strncpy(return_data, udp_packet + 8, len - 8); // copy the udp packet to return data, skipping the udp header (so copying the data only)

    return_packet->length = len - 8; // set the length field of the return struct to the length of the data
    return_packet->data = return_data; // add the pointer

    free(udp_packet); // free the temporary memory allocation
    return 0; // exit successfully
}

int main() {
    // in this file we assume:
    //      char is exactly 1 octet
    //      unsigned int is at least 1 octet
    int in_socket = socket_attain(); // file descriptor socket
    int out_socket = in_socket; // this is for stand-alone mode, if you'd like to chain multiple programs together change this or the input socket

    struct raw_packet* received_packet; // initialize a structure for the sip packet
    while (true) {
        received_packet = NULL; // set the pointer to the structure to null
        if (recv_packet(in_socket, received_packet) != 0) {  // receive the next packet into a raw_packet struct. this is datatype specific and should be changed if you change the socket.
            free(received_packet); // free the memory allocation for the packet data
            continue; // go back to the start of the loop
        };

        // there will eventually be a return call in here to exit, but not here rn, TODO

        if (received_packet != NULL) {
            free(received_packet);
        }
    }

    close(in_socket); // close and free the socket
    return 0; // exit
}
