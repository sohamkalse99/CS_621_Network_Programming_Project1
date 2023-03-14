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

// #define PORT 8080
#define SA struct sockaddr

struct config_file{
    char serverIP[50];
    char sourcePortUDP[10];
    char destPortUDP[10];
    char portTCP[10];
};

char* get_client_data(int connfd){
    char buffer[1024];

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

    read(connfd, buffer, sizeof(buffer));// read message from client and store it in buffer

    char* ptr = malloc(strlen(buffer)+1);
    strcpy(ptr, buffer);
    return ptr;
    // cJSON *json = cJSON_Parse(buffer);
    
    // if(json == NULL){
    //     printf("---------------");
    //     printf("Error parsing json string:%s\n", cJSON_GetErrorPtr());
    //     printf("---------------");
    //     exit(1);
    // }

    // const cJSON *serverIP = cJSON_GetObjectItemCaseSensitive(json, "serverIP");

    // printf("Server IP = %s", serverIP->valuestring);
    
}

void create_json(char* buffer){

    cJSON *json = cJSON_Parse(buffer);
    
    if(json == NULL){
        printf("---------------");
        printf("Error parsing json string:%s\n", cJSON_GetErrorPtr());
        printf("---------------");
        exit(1);
    }

    const cJSON *serverIP = cJSON_GetObjectItemCaseSensitive(json, "serverIP");
    // printf("Server IP = %s", serverIP->valuestring);

    struct config_file config_file;
    strcpy(config_file.serverIP, serverIP->valuestring);
    printf("Server IP = %s", config_file.serverIP);
}

void main(int argc, char **argv){

    if(argc<2){
        printf("Please provide command line arguments");
        return;
    }

    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);//ipv4/6, connection_oriented/connection_less, protocol

    if(sockfd<0){
        printf("Error in socket creation\n");
        exit(0);
    }else{
        printf("Socket successfully created\n");
    }
    
    bzero(&servaddr, sizeof(servaddr)); //set I guess elements of struct bzero to 0

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //assigns s_addr = 0.0.0.0
    int port = atoi(argv[1]);
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
        printf("Server Listening");

    int len_cli = sizeof(cli);

    connfd = accept(sockfd, (SA*)&cli, &len_cli); // Used to accept incoming request from client.
    
    if(connfd<0){
        printf("Server accept failed\n");
        exit(0);
    }else
        printf("Client accepted by the server\n");

    char* buffer = get_client_data(connfd);

    create_json(buffer);
    // cJSON *json = cJSON_Parse(buffer);
    
    // const cJSON *serverIP = cJSON_GetObjectItem(json, "serverIP");

    // printf("Server IP = %s", serverIP->valuestring);

    close(sockfd);

}