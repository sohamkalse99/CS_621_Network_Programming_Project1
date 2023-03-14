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
    char serverIP[50];
    char sourcePortUDP[10];
    char destPortUDP[10];
    char portTCP[50];
};

void converse(int sockfd, char *file_as_string){
    char buffer[800];
    int n;

    //write(sockfd, file_as_string, sizeof(file_as_string));

    bzero(buffer, sizeof(buffer));

    strcpy(buffer, file_as_string);
        
    write(sockfd, buffer, sizeof(buffer));
    
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

void main(int argc, char **argv){

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
    printf("Length of the contents of file are %lu", sizeof(file_as_string));

    struct config_file config_file;

    const cJSON *serverIP = cJSON_GetObjectItemCaseSensitive(json, "serverIP");
    const cJSON *portTCP = cJSON_GetObjectItemCaseSensitive(json, "portTCP");

    strcpy(config_file.serverIP, serverIP->valuestring);
    strcpy(config_file.portTCP, portTCP->valuestring);

    printf("Server IP = %s", config_file.serverIP);

    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd<0){
        printf("socket creation failed..\n");
        exit(0);
    }else  
        printf("Socket created\n");

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(config_file.serverIP); // set ip address of server
    int port = atoi(config_file.portTCP);
    servaddr.sin_port = htons(port);

    if(connect(sockfd, (SA*)&servaddr, sizeof(servaddr))!=0){
        printf("Connection with the server failed\n");
        exit(0);
    }else
        printf("Connected to the server..\n");

    converse(sockfd, file_as_string);
    close(sockfd);
}