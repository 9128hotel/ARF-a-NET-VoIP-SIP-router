#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
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
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // IP address. INADDR_ANY = 0.0.0.0, htonl is not needed here because 0.0.0.0 is always 0, but important if you want to change IP

    if (bind(sockfd, (struct sockaddr*) &addr, sizeof(addr)) < 0) { // bind the socket and address together
        perror("Socket connection failed"); // if that fails, print an error
        close(sockfd); // and close the socket
        return 1; // exit
    }

    close(sockfd); // close and free the socket
    return 0; // exit
}
