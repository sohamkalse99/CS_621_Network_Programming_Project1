#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include "cJSON.c"
#include "cJSON.h"

// #define PORT 8080
#define SA struct sockaddr

struct config_file{
    
    char client_ip[50];
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
 * 
 * Here conversation between client and server take place
 * 
 * @param sockfd
 * @param file_as_string A character pointer
 * 
 * @return character pointer
*/
char* converse(int sockfd, char *file_as_string){
    char buffer[1024];
    int n;


    bzero(buffer, sizeof(buffer));

    strcpy(buffer, file_as_string);
        
    write(sockfd, buffer, strlen(buffer));
    
    bzero(buffer, sizeof(buffer));
    
    read(sockfd, buffer, sizeof(buffer));
    char* message = malloc(100);
    strcpy(message, buffer);
    return message;

    
    
}

/**
 * Here TCP connection takes place and config file is passed to the server from the client
 * 
 * @param file_as_string A character pointer
 * 
 * @return character pointer
*/
char* tcp_connection(char* file_as_string){
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd==-1){
        printf("socket creation failed..\n");
        exit(0);
    }else  
        printf("Socket created\n");

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(config_file->server_ip); // set ip address of server
    int port = atoi(config_file->port_tcp);
    servaddr.sin_port = htons(port);

    if(connect(sockfd, (SA*)&servaddr, sizeof(servaddr))!=0){
        printf("Connection with the server failed\n");
        exit(0);
    }else
        printf("Connected to the server..\n");

    char* buffer = converse(sockfd, file_as_string);
    

    close(sockfd);

    return buffer;
}



/**
 * Here low entropy packets are created and sent
 * 
 * @param sockfd
 * @param serv_addr
*/
void low_entropy(int sockfd, struct sockaddr_in serv_addr){
    
    uint16_t packet_id = 0; //initialize packet_id to zero

    unsigned char packet[atoi(config_file->payload)];

    // char first_bit = 0;
    // char sec_bit = 0;
    for(int i =0;i<atoi(config_file->no_of_packets);i++){
        memset(packet, 0, atoi(config_file->payload)*sizeof(packet[0])); //initialize all elements of packet with 0 ie low entropy

        packet[0] = (i>>8) & 0xFF;
        packet[1] = i & 0xFF;
        
        

        // usleep(150);//250 milisec
        int packet_sent = sendto(sockfd, packet, atoi(config_file->payload), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        if(packet_sent<0){
            printf("Error in LE packet id %d\n", packet_id);
            exit(1);
        }
        
    }
}

/**
 * Here high entropy packets are created and sent
 * 
 * @param sockfd
 * @param serv_addr
*/
void 
high_entropy(int sockfd, struct sockaddr_in serv_addr){
    //Read data from file and store in an array to create payload

    int ptr = 0;
    unsigned char packet[atoi(config_file->payload)];

    FILE *fp = fopen("HighEntropyData","r");

    if(fp == NULL){
        perror("Failed to open file");
    }

    
    
    fgets(packet, atoi(config_file->payload), fp);

    

    uint16_t packet_id = 0; //initialize packet_id to zero

    

    for(int i =0;i<atoi(config_file->no_of_packets);i++){

        
        packet[0] = (i>>8) & 0xFF;
        packet[1] = i & 0xFF;
        
        // usleep(150); //200 milisec
        int packet_sent = sendto(sockfd, packet, atoi(config_file->payload), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        if(packet_sent<0){
            printf("Error in HE packet id %d\n", packet_id);
            exit(1);
        }

    }
    fclose(fp);
}

/**
 * Here low and high entropy UDP packets are created and sent to the server
*/
void 
udp_packets(){

    struct sockaddr_in serv_addr, cli_addr;
    bzero(&serv_addr, sizeof(serv_addr)); 

    serv_addr.sin_addr.s_addr = inet_addr(config_file->server_ip);
    int port = atoi(config_file->dest_port_udp);
    serv_addr.sin_port = htons(port);
    serv_addr.sin_family = AF_INET;
    
    memset(&cli_addr, 0, sizeof(cli_addr));

    cli_addr.sin_addr.s_addr = inet_addr(config_file->client_ip);
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(atoi(config_file->source_port_udp));

    

    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(bind(sockfd, (struct sockaddr *)&cli_addr, sizeof(cli_addr))<0){
        printf("Bind failed");
        exit(1);
    }

    if(sockfd<0){
        perror("socket");
        exit(1);
    }

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
        printf("\n Connection Failed \n");
        exit(1);
    }

    //set don't fragment bit
    int optval = IP_PMTUDISC_DO;

    if(setsockopt(sockfd, IPPROTO_IP, IP_MTU_DISCOVER, &optval, sizeof(optval))<0){
        perror("setsockopt");
        exit(0);
    }

    low_entropy(sockfd, serv_addr);

    sleep(atoi(config_file->inter_measure_time));

    high_entropy(sockfd, serv_addr);

}

/**
 * 
 * Here information from the server is take in an array
 * @param sockfd
 * @param file_as_string
*/
void 
get_findings(int sockfd, char* file_as_string){
    char findings[100];

    bzero(findings, sizeof(findings));

    read(sockfd, findings, sizeof(findings));

    printf("Findings->%s", findings);
}

/**
 * Here TCP connection takes place and the result is taken from the server and displayed on the console
 * 
 * @param file_as_string
*/
void 
post_probing(char* file_as_string){
    
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd==-1){
        printf("socket creation failed..\n");
        exit(0);
    }else  
        printf("Socket created\n");

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(config_file->server_ip); // set ip address of server
    int port = atoi(config_file->port_tcp);
    servaddr.sin_port = htons(port);

    if(connect(sockfd, (SA*)&servaddr, sizeof(servaddr))!=0){
        printf("Connection with the server failed\n");
        exit(0);
    }else
        printf("Connected to the server..\n");

    get_findings(sockfd, file_as_string);
    
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
    
    char data[1024];

    FILE *jsonFile;
    jsonFile = fopen(argv[1], "r");
    fread(data, 1024, 1, jsonFile);
    
    cJSON *json = cJSON_Parse(data);

    char* file_as_string = cJSON_Print(json);

    const cJSON *client_ip = cJSON_GetObjectItemCaseSensitive(json, "clientIP");
    const cJSON *server_ip = cJSON_GetObjectItemCaseSensitive(json, "serverIP");
    const cJSON *source_port_udp = cJSON_GetObjectItemCaseSensitive(json, "sourcePortUDP");
    const cJSON *dest_port_udp = cJSON_GetObjectItemCaseSensitive(json, "destPortUDP");
    const cJSON *port_tcp = cJSON_GetObjectItemCaseSensitive(json, "portTCP");
    const cJSON *payload = cJSON_GetObjectItemCaseSensitive(json, "payload");
    const cJSON *inter_measure_time = cJSON_GetObjectItemCaseSensitive(json, "interMeasureTime");
    const cJSON *no_of_packets = cJSON_GetObjectItemCaseSensitive(json, "noOfPackets");
    const cJSON *ttl = cJSON_GetObjectItemCaseSensitive(json, "TTL");

    strcpy(config_file->client_ip, client_ip->valuestring);
    strcpy(config_file->server_ip, server_ip->valuestring);
    strcpy(config_file->source_port_udp, source_port_udp->valuestring);
    strcpy(config_file->dest_port_udp, dest_port_udp->valuestring);
    strcpy(config_file->port_tcp, port_tcp->valuestring);
    strcpy(config_file->payload, payload->valuestring);
    strcpy(config_file->inter_measure_time, inter_measure_time->valuestring);
    strcpy(config_file->no_of_packets, no_of_packets->valuestring);
    strcpy(config_file->ttl, ttl->valuestring);
    
    
    if(strcmp(config_file->payload, " ")){
        strcpy(config_file->payload, "1000");
    }
    
    if(strcmp(inter_measure_time->valuestring, " "))
        strcpy(inter_measure_time->valuestring, "15");
    
    if(strcmp(no_of_packets->valuestring, " "))
        strcpy(no_of_packets->valuestring, "6000");

    if(strcmp(ttl->valuestring, " "))
        strcpy(ttl->valuestring, "255");

 
    char* server_msg = tcp_connection(file_as_string);



    if(server_msg){
        udp_packets();
    }


    sleep(8);
    post_probing(file_as_string);
}