#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <math.h>
#include <time.h>

static const int BROADCAST_PORT = 9999;
static const char BROADCAST_IP[] = "192.168.0.255";
static const int RATE_MS = 50000000;

static int open_broadcast_socket (int *broadcast_socket, struct sockaddr_in *host_addr){

    int trueval = 1;

    memset(host_addr, 0, sizeof(struct sockaddr_in));
    host_addr->sin_family = AF_INET;
    host_addr->sin_addr.s_addr = inet_addr(BROADCAST_IP);
    host_addr->sin_port = htons (BROADCAST_PORT);

    *broadcast_socket = socket (PF_INET, SOCK_DGRAM, getprotobyname ("udp")->p_proto);
    if (broadcast_socket < 0){
        printf("Error creating broadcast socket.\n");
    }

    setsockopt (*broadcast_socket, SOL_SOCKET, SO_REUSEADDR, &trueval, sizeof (trueval));
    setsockopt (*broadcast_socket, SOL_SOCKET, SO_BROADCAST, &trueval, sizeof (trueval));

    if (bind (*broadcast_socket, (const struct sockaddr *)host_addr, sizeof (struct sockaddr_in)) < 0){
        printf("Couldn't bind to port %d \n", BROADCAST_PORT);
        return -1;
    }

    return 0;
}

#pragma pack(1)
typedef struct _data_packet{
    double i;
    double sin;
    double cos;
} data_packet_t;
#pragma pack()

void main(void){

    data_packet_t data_packet;
    int broadcast_sock = 0;
    struct sockaddr_in host_addr;
    struct timespec ts;

    ts.tv_sec = 0;
    ts.tv_nsec = RATE_MS;

    open_broadcast_socket(&broadcast_sock, &host_addr);

    for(double i = 0 ;; i += .1){

        data_packet.i = i;
        data_packet.sin = sin(i);
        data_packet.cos = cos(i);

        sendto(broadcast_sock,
               &data_packet,
               sizeof(data_packet),
               0,
               (struct sockaddr *) &host_addr,
               sizeof(host_addr));

        nanosleep(&ts, &ts);
    }
}
