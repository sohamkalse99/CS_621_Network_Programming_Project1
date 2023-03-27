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

char* converse(int sockfd, char *file_as_string){
    char buffer[1024];
    int n;

    //write(sockfd, file_as_string, sizeof(file_as_string));

    bzero(buffer, sizeof(buffer));

    strcpy(buffer, file_as_string);
        
    write(sockfd, buffer, strlen(buffer));
    
    bzero(buffer, sizeof(buffer));
    
    read(sockfd, buffer, sizeof(buffer));
    char* message = malloc(100);
    strcpy(message, buffer);
    // printf("Message->%s",message);
    return message;
    // bzero(buffer, sizeof(buffer));

    // read(sockfd, buffer, sizeof(buffer));
    // printf("From Server:%s",buffer);
        
    // if(strncmp(buffer, "exit", 4)==0){
    //     printf("Client Exit..\n");
    //     //break;
    // }
    //while (1)
    //{
        //bzero(buffer, sizeof(buffer));
        //printf("Enter String: ");
        /*n=0;
        while ((file_as_string[n])!= '}'){
            buffer[n] = file_as_string[n];
            n++;
        }
        buffer[n] = '}';*/

        //strcpy(buffer, file_as_string);
        
        //write(sockfd, buffer, sizeof(buffer));
        //write(sockfd, file_as_string, sizeof(file_as_string));
        //bzero(buffer, sizeof(buffer));
        // read(sockfd, buffer, sizeof(buffer));
        // printf("From Server:%s",buffer);
        
        // if(strncmp(buffer, "exit", 4)==0){
        //     printf("Client Exit..\n");
        //     break;
        // }
        
    //}
    
}

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
    
    // printf("%s", buffer);

    close(sockfd);

    return buffer;
}

void udp_connection(){
    
    char buffer[100];
    char* message = "Hello Server";

    int sockfd,n;

    struct sockaddr_in serv_addr;
    // printf("%s", config_file->server_ip);
    // printf("%s", config_file->source_port_udp);
    // printf("%s", config_file->dest_port_udp);
    // printf("%s", config_file->port_tcp);
    // printf("%s", config_file->payload);
    // printf("%s", config_file->inter_measure_time);
    // printf("%s", config_file->ttl);

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_addr.s_addr = inet_addr(config_file->server_ip);
    int port = atoi(config_file->dest_port_udp);
    serv_addr.sin_port = htons(port);
    serv_addr.sin_family = AF_INET;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
        printf("\n Connection Failed \n");
        exit(0);
    }

    sendto(sockfd, message, 1000, 0, (struct sockaddr*)NULL, sizeof(serv_addr));
    printf("Message sent to server");
    //recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
    //puts(buffer);

    close(sockfd);
}

void low_entropy(int sockfd, struct sockaddr_in serv_addr){
    
    uint16_t packet_id = 0; //initialize packet_id to zero

    unsigned char packet[atoi(config_file->payload)];

    // char first_bit = 0;
    // char sec_bit = 0;
    for(int i =0;i<atoi(config_file->no_of_packets);i++){
        memset(packet, 0, atoi(config_file->payload)*sizeof(packet[0])); //initialize all elements of packet with 0 ie low entropy

        packet[0] = (i>>8) & 0xFF;
        packet[1] = i & 0xFF;
        
        /*if(i ==0)
            continue;
        else{
            first_bit++;
            if(first_bit == 0)
                sec_bit++;
        }*/

        // packet[0] = first_bit;
        // packet[1] = sec_bit;
        // memcpy(packet + sizeof(uint16_t), &packet_id, sizeof(uint16_t));

        //printf("Packet id->%d\n", packet_id);
        /*for(int j =0;j<998;j++){
            printf("%d", packet[j]);
        }*/

        // usleep(200);//250 milisec
        int packet_sent = sendto(sockfd, packet, atoi(config_file->payload), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        if(packet_sent<0){
            printf("Error in LE packet id %d\n", packet_id);
            exit(1);
        }
        // packet_id++;
        // bzero(packet, sizeof(packet));
    }
}

void high_entropy(int sockfd, struct sockaddr_in serv_addr){
    //Read data from file and store in an array to create payload

    int ptr = 0;
    unsigned char packet[atoi(config_file->payload)];

    FILE *fp = fopen("HighEntropyData","r");

    if(fp == NULL){
        perror("Failed to open file");
    }

    //fscanf(fp, "%[^\n]", packet);
    /*while (fscanf(fp, "%x", &packet[ptr]) !=EOF)
    {
        ptr++;
        if(ptr>sizeof(packet)){
            break;
        }
    }*/
    
    /*while (!feof(fp))
    {
        fgets(packet, 998, fp);
        printf("First element->%x\n", packet[0]);

    }*/
    
    fgets(packet, atoi(config_file->payload), fp);

    //fseek(fp, 0L, SEEK_END);
    //long int res = ftell(fp);
    //printf("Size of file %ld",res);
    //fread(packet, 998, 1, fp);
    //printf("First element->%x\n", packet[0]);
    //printf("Last element->%x\n", packet[300]);

    uint16_t packet_id = 0; //initialize packet_id to zero

    //char packet[998];

    for(int i =0;i<atoi(config_file->no_of_packets);i++){
        //memset(packet, 0, sizeof(packet)); //initialize all elements of packet with 0 ie low entropy

        //memcpy(packet + sizeof(uint16_t), &packet_id, sizeof(uint16_t));
        
        packet[0] = (i>>8) & 0xFF;
        packet[1] = i & 0xFF;
        
        // usleep(200); //250 milisec
        int packet_sent = sendto(sockfd, packet, atoi(config_file->payload), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        if(packet_sent<0){
            printf("Error in HE packet id %d\n", packet_id);
            exit(1);
        }

        // packet_id++;
    }
    fclose(fp);
}

void udp_packets(){

    struct sockaddr_in serv_addr, cli_addr;
    bzero(&serv_addr, sizeof(serv_addr)); 

    serv_addr.sin_addr.s_addr = inet_addr(config_file->server_ip);
    int port = atoi(config_file->dest_port_udp);
    serv_addr.sin_port = htons(port);
    serv_addr.sin_family = AF_INET;
    
    memset(&cli_addr, 0, sizeof(cli_addr));

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

void get_findings(int sockfd, char* file_as_string){
    char findings[100];

    // char* findings = malloc(100);
    bzero(findings, sizeof(findings));
    // char* findings = malloc(20*sizeof(char*));
    read(sockfd, findings, sizeof(findings));

    printf("Findings->%s", findings);
}


void post_probing(char* file_as_string){
    
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
void main(int argc, char **argv){

    config_file = malloc(sizeof(*config_file));

    if(argc<2){
        printf("Please provide command line arguments");
        return;
    }
    
    char data[1024];
    //printf("%s", argv[1]);
    FILE *jsonFile;
    jsonFile = fopen(argv[1], "r");
    fread(data, 1024, 1, jsonFile);
    // printf("%s", buffer);
    
    // char** x[50] = argv[1];
    cJSON *json = cJSON_Parse(data);

    char* file_as_string = cJSON_Print(json);
    // printf("Length of the contents of file are %lu", sizeof(file_as_string));

    //struct config_file config_file;

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
    
    
    if(strcmp(config_file->payload, " ")){
        strcpy(config_file->payload, "1000");
    }
    
    if(strcmp(inter_measure_time->valuestring, " "))
        strcpy(inter_measure_time->valuestring, "15");
    
    if(strcmp(no_of_packets->valuestring, " "))
        strcpy(no_of_packets->valuestring, "6000");

    if(strcmp(ttl->valuestring, " "))
        strcpy(ttl->valuestring, "255");

    // printf("payload = %s", config_file->payload);
 
    char* server_msg = tcp_connection(file_as_string);

    // int sockfd, connfd;
    // struct sockaddr_in servaddr, cli;

    // sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // if(sockfd<0){
    //     printf("socket creation failed..\n");
    //     exit(0);
    // }else  
    //     printf("Socket created\n");

    // bzero(&servaddr, sizeof(servaddr));

    // servaddr.sin_family = AF_INET;
    // servaddr.sin_addr.s_addr = inet_addr(config_file->serverIP); // set ip address of server
    // int port = atoi(config_file->portTCP);
    // servaddr.sin_port = htons(port);

    // if(connect(sockfd, (SA*)&servaddr, sizeof(servaddr))!=0){
    //     printf("Connection with the server failed\n");
    //     exit(0);
    // }else
    //     printf("Connected to the server..\n");

    // converse(sockfd, file_as_string);
    // close(sockfd);

    //sleep(10);
    // printf("%s", server_msg);
    // sleep(4);
    // udp_connection();

    if(server_msg){
        // udp_connection();
        udp_packets();
    }

    // udp_packets();

    sleep(8);
    post_probing(file_as_string);
}