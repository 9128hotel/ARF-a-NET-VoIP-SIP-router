#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

/*
      Request-URI: The Request-URI names the domain of the location
           service for which the registration is meant (for example,
           "sip:chicago.com").  The "userinfo" and "@" components of the
           SIP URI MUST NOT be present.

      To: The To header field contains the address of record whose
           registration is to be created, queried, or modified.  The To
           header field and the Request-URI field typically differ, as
           the former contains a user name.  This address-of-record MUST
           be a SIP URI or SIPS URI.

      From: The From header field contains the address-of-record of the
           person responsible for the registration.  The value is the
           same as the To header field unless the request is a third-
           party registration.

      Call-ID: All registrations from a UAC SHOULD use the same Call-ID
           header field value for registrations sent to a particular
           registrar.

           If the same client were to use different Call-ID values, a
           registrar could not detect whether a delayed REGISTER request
           might have arrived out of order.

      CSeq: The CSeq value guarantees proper ordering of REGISTER
           requests.  A UA MUST increment the CSeq value by one for each
           REGISTER request with the same Call-ID.

      Contact: REGISTER requests MAY contain a Contact header field with
           zero or more values containing address bindings.
*/

struct REGISTER {
    char* request_URI; // unbounded-length URI not including the "userinfo" and "@" components of the SIP URI
    char* to; // refer to: https://en.wikipedia.org/wiki/SIP_URI_scheme, SIP addresses look like this: sip:+1-999-123-4567@voip-provider.example.net (or sips: start for SIPS addresses)
    char* from; // same as above, and may even be the same as above
    char call_id[32]; // UUID
    unsigned long long int CSeq; // TODO: check what this is for, i think its for switching between phones?
    char** contact // TODO: not a clue what this is for
}

int socket_attain() {
    int sockfd = socket( // request socket (as file descriptor)
        AF_INET, // IPv4
        SOCK_DGRAM, // connectionless datagram socket
        0 // default for this combo (which is UDP)
    );

    if (sockfd < 0) { // socket returns -1 for an error
        perror("Socket connection failed"); // print an error (text is error prefix)
        return 1; // exit
    }

    struct sockaddr_in addr; // create an instance of sockaddr_in called addr
    memset(&addr, 0, sizeof(addr)); // zero the structure so no garbage is sent to kernel
    addr.sin_family = AF_INET; // IPv4
    addr.sin_port  = htons(5060); // port 5060 (htons "host to network short" converts number to big endian, which is standard for networking)
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // IP address. INADDR_ANY = 0.0.0.0, htonl is not needed here because 0.0.0.0 is always 0 but important if you want to change IP to eg 127.0.0.0 for internal only

    if (bind(sockfd, (struct sockaddr*) &addr, sizeof(addr)) < 0) { // bind the socket and address together
        perror("Socket connection failed"); // if that fails, print an error
        close(sockfd); // and close the socket
        return 1; // exit
    }
}

struct raw_packet {
    int length;
    char* data;
}

char* recv_packet(int sockfd, raw_packet* return_packet) {
    unsigned char header[8] = { 0 }; // make an array the length of the UDP header and initialize to zero to avoid garbage data
    recvfrom(sockfd, header, sizeof(header), MSG_PEEK, NULL, NULL) // receive the header from sockfd using the peek flag to not pop the message off the queue. we don't check return value as if it failed the packet is invalid or something is VERY wrong with the machine
    unsigned char len; // init a 1 bit large char which'll hold the length of the UDP packet
    strncpy(len, header + 4, 1); // extract the length field from the UDP packet header

    char* udp_packet = calloc(len, 1); // allocate some heap the size of the UDP packet (or atleast what it advertises) and zero it

    if (udp_packet == NULL) { // check if calloc has failed. if this occurs its really bad, this means you might be out of memory
        char trashbin[1] = { 0 }
        recvfrom(sockfd, trashbin, 0, 0, NULL, NULL); // drop the packet to clear it off the queue (udp should be ok with this)
        printf("[recv_packet: ERROR] calloc'ing udp_packet has failed");
        return -1;
    }

    if (recvfrom(sockfd, udp_packet, len, 0, NULL, NULL) != len) { // read the full packet now from sockfd, and without the peek flag so it does pop the packet off the queue!
        printf("A packet is lying about its size\n"); // sanity check if the packet length read and the size it reported are different. if you see this error add more checking here.
        free(udp_packet); // free the memory allocated earlier
        return -1;
    }

    return_data = calloc(len - 8, 1);

    if (return_data == NULL) { // check if calloc has failed
        printf("[recv_packet: ERROR] calloc'ing return_data has failed"); // show an error
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
    int in_socket = socket_attain(); // file descriptor socket
    int out_socket = in_socket; // this is for stand-alone mode, if you'd like to chain multiple programs together change this or the input socket

    raw_packet* received_packet; // initialize a structure for the sip packet
    while (true) {
        received_packet = NULL; // set the pointer to the structure to null
        if (recv_packet(phone_socket, received_packet) != 0) {  // receive the next packet into a raw_packet struct. this is datatype specific and should be changed if you change the socket.
            free(received_packet); // free the memory allocation for the packet data
            continue; // go back to the start of the loop
        };

        

        if (received_packet != NULL) {
            free(received_packet);
        }
    }

    close(in_socket); // close and free the socket
    return 0; // exit
}
