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
bool create_json(char* buffer){

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


    // printf("Server IP = %s", serverIP->valuestring);

    // struct config_file config_file;
    strcpy(config_file->server_ip, server_ip->valuestring);
    strcpy(config_file->source_port_udp, source_port_udp->valuestring);
    strcpy(config_file->dest_port_udp, dest_port_udp->valuestring);
    strcpy(config_file->port_tcp, port_tcp->valuestring);
    strcpy(config_file->payload, payload->valuestring);
    strcpy(config_file->inter_measure_time, inter_measure_time->valuestring);
    strcpy(config_file->no_of_packets, no_of_packets->valuestring);
    strcpy(config_file->ttl, ttl->valuestring);

    // printf("Server IP = %s", config_file->server_ip);

    return json_created;
}

bool get_client_data(int connfd){
    char buffer[2000];

    //char* buffer = malloc(sizeof(*buffer));

    int n;
    
    /*while(1){

        //bzero(buffer, 8000);

        read(connfd, buffer, sizeof(buffer));// read message from client and store it in buffer
        //printf("%s", buffer);
        //printf("From Client: %s\t To Client: ", buffer);
        //bzero(buffer, 8000);
        n=0;
        

        //while((buffer[n++] = getchar())!='\n') // copyserver's message in a buffer
        //;

        //write(connfd, buffer, sizeof(buffer)); // send message to the client

        // cJSON *json = cJSON_Parse(buffer);
        // char* string = cJSON_Print(json);
        // printf("%s", string);
        // if(json == NULL){
        //     printf("---------------");
        //     printf("Error parsing json string:%s\n", cJSON_GetErrorPtr());
        //     printf("---------------");
        //     exit(1);
        // }
        // const cJSON *serverIP = cJSON_GetObjectItemCaseSensitive(json, "serverIP");

        // printf("Server IP = %s", serverIP->valuestring);
         
        if(strncmp("exit", buffer, 4)==0){
            printf("Server Exit....\n");
            break;
        }
    }*/

    bzero(buffer, sizeof(buffer));
    read(connfd, buffer, sizeof(buffer));// read message from client and store it in buffer

    
    // char* ptr = malloc(strlen(buffer)+1);
    
    // strcpy(ptr, buffer);
    // printf("%s",buffer);

    // printf("%s", buffer);
    bool json_created = create_json(buffer);

    bzero(buffer, sizeof(buffer));
    strcpy(buffer, "Server received Config file");
    write(connfd, buffer, sizeof(buffer));

    
    // printf("pointer->%s", ptr);
    // return strdup(ptr);
    // return ptr;
    // cJSON *json = cJSON_Parse(buffer);
    
    // if(json == NULL){
    //     printf("---------------");
    //     printf("Error parsing json string:%s\n", cJSON_GetErrorPtr());
    //     printf("---------------");
    //     exit(1);
    // }

    // const cJSON *serverIP = cJSON_GetObjectItemCaseSensitive(json, "serverIP");

    // printf("Server IP = %s", serverIP->valuestring);
    
    return json_created;
}



bool tcp_connection(char* cmd_line_arg){
    // printf()
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);//ipv4/6, connection_oriented/connection_less, protocol

    if(sockfd == -1){
        printf("Error in socket creation\n");
        exit(0);
    }else{
        printf("Socket successfully created\n");
    }
    
    bzero(&servaddr, sizeof(servaddr)); //set I guess elements of struct bzero to 0

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //assigns s_addr = 0.0.0.0
    int port = atoi(cmd_line_arg);
    servaddr.sin_port = htons(port);

    if((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0){
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

    
    // bool isConnected =false;
    // char* buffer = malloc(1000);
    bool json_created =  get_client_data(connfd);
    printf("After get client method");
    // if(sizeof(buffer)>0)
    //     isConnected = true;
    
    // bool json_created = create_json(buffer);
    // printf("After create json");

    // cJSON *json = cJSON_Parse(buffer);
    
    // const cJSON *serverIP = cJSON_GetObjectItem(json, "serverIP");

    // printf("Server IP = %s", serverIP->valuestring);

    close(sockfd);

    return json_created;
}





void udp_connection(){
    char buffer[100];
    char* message = "Hello Client";
    int listenfd, len;
    struct sockaddr_in serv_addr, cli_addr;
    bzero(&serv_addr, sizeof(serv_addr));

    listenfd = socket(AF_INET, SOCK_DGRAM, 0);//UDP Socket

    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int port = atoi(config_file->dest_port_udp);
    serv_addr.sin_port = htons(port);
    serv_addr.sin_family = AF_INET;

    
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    len = sizeof(cli_addr);
    
    
    int n = recvfrom(listenfd, buffer, sizeof(buffer), MSG_WAITALL, (struct sockaddr*)&cli_addr, &len);
    buffer[n] = '\0';
    printf("%s", buffer);
    

    //sendto(listenfd, message, 1000, 0, (struct sockaddr*)&cli_addr, sizeof(cli_addr));
}

void sig_handler(int sig_num){
    printf("loss");
    flag =1;
    //exit(0);
}

long int low_entropy(int listenfd, struct sockaddr_in cli_addr,int len){

    struct timeval t1, t2;

    // int packet[998];
    char packet[atoi(config_file->payload)];

    for(int i =0;i<atoi(config_file->no_of_packets) && flag == 0;i++){
        //printf("i = %d\n",i);
        
        
        int n = recvfrom(listenfd, packet, sizeof(packet), MSG_WAITALL, (struct sockaddr*)&cli_addr, &len);

        if(i==0 && n>0){
            gettimeofday(&t1, NULL);
            signal(SIGALRM, sig_handler);

            alarm(10);
        }

        // printf("Packet->%d\n", i);
        // packet[n] = '\0';
        if(i>0 && n>0)
            gettimeofday(&t2, NULL);
    }

    //printf("Outside for loop");

    long int time_diff = (t2.tv_sec - t1.tv_sec)*1000000+(t2.tv_usec - t1.tv_usec);
    printf("TD of l data is %ld", time_diff);
    return time_diff;
}

long int high_entropy(int listenfd, struct sockaddr_in cli_addr,int len){
    struct timeval t1, t2;

    char packet[atoi(config_file->payload)];
    flag = 0;//Check whether this will work 

    for(int i =0;i<atoi(config_file->no_of_packets) && flag == 0;i++){
        //printf("i = %d\n",i);
        int n = recvfrom(listenfd, packet, sizeof(packet), MSG_WAITALL, (struct sockaddr*)&cli_addr, &len);
        // printf("Packet->%d\n", i);

        if(i==0 && n>0){
            gettimeofday(&t1, NULL);
            signal(SIGALRM, sig_handler);
            alarm(10);
        }

        // packet[n] = '\0';
        if(i>0 && n>0){
            gettimeofday(&t2, NULL);
        }

        /*if(i%1000 == 0)
        {
            printf("HEPacket");
        }*/
    }

    long int time_diff = (t2.tv_sec - t1.tv_sec)*1000000+(t2.tv_usec - t1.tv_usec);
    printf("TD for HE data is %ld", time_diff);

    return time_diff;
}
long int udp_packets(){

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

    printf("Before Low Entropy method");
    long int delta_low = low_entropy(listenfd, cli_addr,len);

    
    long int delta_high = high_entropy(listenfd, cli_addr,len);

    return abs(delta_high - delta_low);
    
    /*if(abs(delta_high - delta_low)>100000){//time in micro seconds
        printf("Compression Detected");
        strcpy(findings, "Compression Detected");
    }else{
        printf("No Compression Detected");
        strcpy(findings, "No Compression Detected");
    }

    return findings;*/
}

void send_findings(int connfd, char* findings){

    // printf("Findings->%s", findings);
    printf("Message->%s", findings);
    write(connfd, findings, strlen(findings));//strlen, fprintf(CONNFD, FINDINGS)

}
void post_probing(char* cmd_line_arg, char* findings){
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);//ipv4/6, connection_oriented/connection_less, protocol

    if(sockfd == -1){
        printf("Error in socket creation\n");
        exit(0);
    }else{
        printf("Socket successfully created\n");
    }
    
    bzero(&servaddr, sizeof(servaddr)); //set I guess elements of struct bzero to 0

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //assigns s_addr = 0.0.0.0
    int port = atoi(cmd_line_arg);
    servaddr.sin_port = htons(port);

    int sock, optval = 1;

    if((sock=socket(AF_INET, SOCK_STREAM, 0))<0){
        perror("Couldn't create TCP socket");
        abort();
    }

    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))<0){
        perror("couldn't reuse the address");
        abort();
    }
    if((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0){
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
void main(int argc, char **argv){

    config_file = malloc(sizeof(*config_file));

    if(argc<2){
        printf("Please provide command line arguments");
        return;
    }

    // bool config_received = 
    bool json_created = tcp_connection(argv[1]);
    printf("config file sent");
    // printf("Dest Port -> %s\n", config_file->dest_port_udp);
    // udp_connection();
    //char* findings;
    // sleep(5);
    long int time_difference = udp_packets();

    /*char findings[100];
    bzero(findings, sizeof(findings));
    if(time_difference>100000){//time in micro seconds
        //printf("Compression Detected");
        strcpy(findings, "Compression Detected");
    }else{
        //printf("No Compression Detected");
        strcpy(findings, "No Compression Detected");
    }*/

    // sleep(5);
    
    //post_probing(argv[1], findings);
}