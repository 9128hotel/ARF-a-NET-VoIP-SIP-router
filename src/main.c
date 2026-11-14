#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "udp.h"
#include "main.h"

const char* IPv4_allow_list[100] = {"173.139.48.93", "74.210.80.4", "226.187.130.24", "226.171.157.151", "146.157.220.216", "202.192.31.113", "77.112.125.179", "220.224.169.117", "34.38.140.216", "67.27.93.91", "223.75.181.67", "135.102.121.109", "219.58.177.115", "127.123.170.37", "161.174.229.33", "148.220.152.29", "150.57.4.63", "234.99.0.195", "247.59.228.249", "46.136.139.89", "202.202.230.117", "250.173.78.212", "237.91.221.210", "142.129.180.151", "203.224.179.2", "113.128.132.173", "97.236.221.87", "39.185.205.44", "160.142.226.161", "189.245.148.133", "98.154.66.40", "221.235.40.159", "150.181.105.10", "181.91.154.105", "106.232.162.89", "37.75.11.242", "103.149.245.243", "158.171.137.77", "15.207.25.222", "51.3.224.129", "86.189.86.35", "16.186.146.191", "210.37.201.6", "14.167.28.50", "218.197.215.96", "12.19.95.236", "138.184.103.89", "120.235.169.193", "65.81.196.59", "171.253.219.29", "23.254.78.105", "229.201.3.236", "38.189.85.252", "144.245.119.66", "192.102.21.31", "158.104.83.159", "31.64.150.169", "206.183.65.27", "65.11.252.125", "139.87.136.107", "72.251.174.49", "135.199.146.94", "194.184.70.227", "47.169.137.211", "54.35.215.101", "85.46.200.203", "222.164.183.250", "186.41.168.35", "222.247.161.138", "139.213.29.99", "141.45.213.114", "243.163.16.125", "197.167.161.141", "252.86.156.87", "169.247.119.10", "207.26.162.61", "115.97.190.148", "212.135.135.132", "229.136.28.55", "23.255.36.141", "101.42.172.10", "116.231.209.97", "36.109.196.214", "204.220.120.58", "47.248.104.125", "13.126.5.224", "115.233.27.235", "202.96.169.194", "124.13.112.197", "3.245.10.100", "167.41.106.137", "254.16.179.149", "10.130.102.64", "139.4.5.186", "32.86.42.123", "161.189.246.247", "31.117.178.204", "181.147.170.211", "232.153.70.32", "51.204.5.87"};

int main() {
    // in this file we assume:
    //      char is exactly 1 octet
    //      unsigned int is at least 1 octet
    int in_socket = socket_attain(); // file descriptor socket
    int out_socket = in_socket; // this is for stand-alone mode, if you'd like to chain multiple programs together change this or the input socket

    if (in_socket < 0 || out_socket < 0) { // if either sockets have failed to aquire, we cannot operate the program, so we have to stop.
        return -1; // remember, whoever makes it frees it, and whoever errors prints, so we assume the error has already printed if we got here, so we can just exit
    }

    while (true) {
        raw_packet received_packet = (raw_packet){ 0 }; // make an instance of the raw packet struct

        if (recv_packet(in_socket, &received_packet) < 0) { // call the function that receives a packet, which is written to the received_packet pointer that we pass in (using & to make pointer from value)
            continue; // the return value should be 0 for success, and -1 for error. if we error we've already consumed the packet, so we can just go again (unless malloc failed in which we just... try again?)
        }

        printf(
            "[main] Received packet from %s:%u, %zu bytes\n",
            inet_ntoa(received_packet.source_ip.sin_addr), // ip address (converted to a readable format)
            (unsigned)ntohs(received_packet.source_ip.sin_port), // port (converted to a readable format)
            received_packet.length
        );



        // there will eventually be a return call in here to exit, but not here rn, TODO

        free(received_packet.data);
    }

    close(in_socket); // close and free the socket
    return 0; // exit
}
