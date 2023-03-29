#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "cJSON.c"
#include "cJSON.h"
#include <stdbool.h>
#include <sys/time.h>
#include <signal.h>
// #define PORT 8080
#define SA struct sockaddr
int flag = 0;

/* Struct is used to store the values of config file*/
struct config_file{
    char server_ip[50];
    char source_port_udp[10];
    char dest_port_udp[10];
    char port_tcp[10];
    char payload[50];
    char inter_measure_time[50];
    char no_of_packets[50];
    char ttl[50];
};

struct config_file* config_file;

/**
 * Here structure is created which contains the fields of config file which has been sent by the client to the server
 * 
 * @param buffer
 * 
 * @return boolean variable
*/
bool 
create_json(char* buffer){

    cJSON *json = cJSON_Parse(buffer);
    bool json_created = true;
    if(json == NULL){
        printf("---------------");
        printf("Error parsing json string:%s\n", cJSON_GetErrorPtr());
        printf("---------------");
        json_created = false;
        exit(1);
    }

    const cJSON *server_ip = cJSON_GetObjectItemCaseSensitive(json, "serverIP");
    const cJSON *source_port_udp = cJSON_GetObjectItemCaseSensitive(json, "sourcePortUDP");
    const cJSON *dest_port_udp = cJSON_GetObjectItemCaseSensitive(json, "destPortUDP");
    const cJSON *port_tcp = cJSON_GetObjectItemCaseSensitive(json, "portTCP");
    const cJSON *payload = cJSON_GetObjectItemCaseSensitive(json, "payload");
    const cJSON *inter_measure_time = cJSON_GetObjectItemCaseSensitive(json, "interMeasureTime");
    const cJSON *no_of_packets = cJSON_GetObjectItemCaseSensitive(json, "noOfPackets");
    const cJSON *ttl = cJSON_GetObjectItemCaseSensitive(json, "TTL");


 
    strcpy(config_file->server_ip, server_ip->valuestring);
    strcpy(config_file->source_port_udp, source_port_udp->valuestring);
    strcpy(config_file->dest_port_udp, dest_port_udp->valuestring);
    strcpy(config_file->port_tcp, port_tcp->valuestring);
    strcpy(config_file->payload, payload->valuestring);
    strcpy(config_file->inter_measure_time, inter_measure_time->valuestring);
    strcpy(config_file->no_of_packets, no_of_packets->valuestring);
    strcpy(config_file->ttl, ttl->valuestring);


    return json_created;
}

/**
 * Here client data is taken into an array and json is created
 * 
 * @param connfd
 * 
 * @return boolean variable
*/
bool 
get_client_data(int connfd){
    char buffer[2000];


    int n;
    
    

    bzero(buffer, sizeof(buffer));
    read(connfd, buffer, sizeof(buffer));// read message from client and store it in buffer

    
    
    bool json_created = create_json(buffer);

    bzero(buffer, sizeof(buffer));
    strcpy(buffer, "Server received Config file");
    write(connfd, buffer, sizeof(buffer));

    
    
    return json_created;
}


/**
 * Here TCP connection takes place and config file is received by the server from the client
 * 
 * @param cmd_line_arg A character pointer
 * 
 * @return character pointer
*/
bool 
tcp_connection(char* cmd_line_arg){

    int sockfd, connfd, len;
    struct sockaddr_in serv_addr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);//ipv4/6, connection_oriented/connection_less, protocol

    if(sockfd == -1){
        printf("Error in socket creation\n");
        exit(0);
    }else{
        printf("Socket successfully created\n");
    }
    
    bzero(&serv_addr, sizeof(serv_addr)); //set I guess elements of struct bzero to 0

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //assigns s_addr = 0.0.0.0
    int port = atoi(cmd_line_arg);
    serv_addr.sin_port = htons(port);

    if((bind(sockfd, (SA*)&serv_addr, sizeof(serv_addr))) != 0){
        printf("socket binding failed\n");
        exit(0);        
    }else
        printf("Socket binding successful\n");

    if(listen(sockfd, 5) != 0){
        printf("Listen Failed");
        exit(0);
    }else
        printf("Server Listening\n");

    int len_cli = sizeof(cli);

    connfd = accept(sockfd, (SA*)&cli, &len_cli); // Used to accept incoming request from client.
    
    if(connfd<0){
        printf("Server accept failed\n");
        exit(0);
    }else
        printf("Client accepted by the server\n");

    
 
    bool json_created =  get_client_data(connfd);
    

    close(sockfd);

    return json_created;
}


/**
 * If alarm is trigger this method is called
 * @param sig_num
*/
void 
sig_handler(int sig_num){
    printf("loss");
    flag =1;
    //exit(0);
}

/**
 * Here low entropy data is taken from the client and time difference is calculated between the first and last packet
 * 
 * @param listenfd
 * @param cli_addr
 * @param len 
*/
long int 
low_entropy(int listenfd, struct sockaddr_in cli_addr, int len){

    struct timeval t1, t2;

    unsigned char packet[atoi(config_file->payload)];

    for(int i =0;i<atoi(config_file->no_of_packets) && flag == 0;i++){
        
        
        int n = recvfrom(listenfd, packet, sizeof(packet), MSG_WAITALL, (struct sockaddr*)&cli_addr, &len);

        // Packet ID retrieval 
        int int_value = (packet[0] << 8 | packet[1]);
        // printf("%d\t", int_value);

        if(i==0 && n>0){
            gettimeofday(&t1, NULL);
            signal(SIGALRM, sig_handler);

            alarm(10);
        }

        
        if(i>0 && n>0)
            gettimeofday(&t2, NULL);

        
    }

    alarm(0);

    long int time_diff = (t2.tv_sec - t1.tv_sec)*1000000+(t2.tv_usec - t1.tv_usec);
    printf("TD of l data is %ld", time_diff);
    return time_diff;
}

/**
 * Here high entropy data is taken from the client and time difference is calculated between the first and last packet
 * 
 * @param listenfd
 * @param cli_addr
 * @param len 
*/
long int 
high_entropy(int listenfd, struct sockaddr_in cli_addr, int len){
    struct timeval t1, t2;

    unsigned char packet[atoi(config_file->payload)];
    flag = 0;//Check whether this will work 

    for(int i =0;i<atoi(config_file->no_of_packets) && flag == 0;i++){
        //printf("i = %d\n",i);
        int n = recvfrom(listenfd, packet, sizeof(packet), MSG_WAITALL, (struct sockaddr*)&cli_addr, &len);
        // printf("Packet->%d\n", i);

        // Packet ID retrieval 
        int int_value = (packet[0] << 8 | packet[1]);
        // printf("%d", int_value);
        
        if(i==0 && n>0){
            gettimeofday(&t1, NULL);
            signal(SIGALRM, sig_handler);
            alarm(10);
        }

        // packet[n] = '\0';
        if(i>0 && n>0){
            gettimeofday(&t2, NULL);
        }


    }
    alarm(0);
    

    long int time_diff = (t2.tv_sec - t1.tv_sec)*1000000 +(t2.tv_usec - t1.tv_usec);
    printf("TD for HE data is %ld", time_diff);

    return time_diff;
}

/**
 * Here low entropy and high entropy data is taken from the client and time difference is calculated between the high and low entropy packets
 *
*/
long 
int udp_packets(){

    int listenfd, len;
    char findings[50];
    struct sockaddr_in serv_addr, cli_addr;
    bzero(&serv_addr, sizeof(serv_addr));

    listenfd = socket(AF_INET, SOCK_DGRAM, 0);//UDP Socket

    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int port = atoi(config_file->dest_port_udp);
    serv_addr.sin_port = htons(port);
    serv_addr.sin_family = AF_INET;

    
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    len = sizeof(cli_addr);

    // printf("Before Low Entropy method");
    long int delta_low = low_entropy(listenfd, cli_addr,len);

    
    long int delta_high = high_entropy(listenfd, cli_addr,len);
    
    return fabs(delta_high - delta_low);
    
}

/**
 * Here the findings whether compression is detected or not detected is sent to the client
 * 
 * @param connfd
 * @param findings
*/
void 
send_findings(int connfd, char* findings){

    // printf("Findings->%s", findings);
    printf("Message->%s", findings);
    write(connfd, findings, strlen(findings));//strlen, fprintf(CONNFD, FINDINGS)

}

/**
 * Here TCP connection takes place between server and client and the findings are sent to the client
 * 
 * @param cmd_line_arg
 * @param findings
 * 
*/
void 
post_probing(char* cmd_line_arg, char* findings){
    int sockfd, connfd, len;
    struct sockaddr_in serv_addr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);//ipv4/6, connection_oriented/connection_less, protocol

    if(sockfd == -1){
        printf("Error in socket creation\n");
        exit(0);
    }else{
        printf("Socket successfully created\n");
    }
    
    bzero(&serv_addr, sizeof(serv_addr)); //set I guess elements of struct bzero to 0

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //assigns s_addr = 0.0.0.0
    int port = atoi(cmd_line_arg);
    serv_addr.sin_port = htons(port);

    int sock, optval = 1;

    if((sock=socket(AF_INET, SOCK_STREAM, 0))<0){
        perror("Couldn't create TCP socket");
        abort();
    }

    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))<0){
        perror("couldn't reuse the address");
        abort();
    }
    if((bind(sockfd, (SA*)&serv_addr, sizeof(serv_addr))) != 0){
        printf("socket binding failed\n");
        exit(0);        
    }else
        printf("Socket binding successful\n");

    if(listen(sockfd, 5) != 0){
        printf("Listen Failed");
        exit(0);
    }else
        printf("Server Listening\n");

    int len_cli = sizeof(cli);

    connfd = accept(sockfd, (SA*)&cli, &len_cli); // Used to accept incoming request from client.
    
    if(connfd<0){
        printf("Server accept failed\n");
        exit(0);
    }else
        printf("Client accepted by the server\n");

    send_findings(connfd, findings);
}

/**
 * main method
 * 
 * @param argc Number of arguments
 * @param argv Pointer to a pointer to argv
*/
void 
main(int argc, char **argv){

    config_file = malloc(sizeof(*config_file));

    if(argc<2){
        printf("Please provide command line arguments");
        return;
    }

    bool json_created = tcp_connection(argv[1]);

    long int time_difference = udp_packets();

    char findings[100];
    bzero(findings, sizeof(findings));
    if(time_difference>100000){//time in micro seconds
        //printf("Compression Detected");
        strcpy(findings, "Compression Detected");
    }else{
        //printf("No Compression Detected");
        strcpy(findings, "No Compression Detected");
    }

    // sleep(5);
    
    post_probing(argv[1], findings);
}