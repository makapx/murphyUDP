/* CLIENT */
#include <stdio.h>          // standard i/o library
#include <stdlib.h>         // standard library
#include <unistd.h>         // standard Unix interface
#include <sys/socket.h>     // required for socket
#include <arpa/inet.h>      // required for inet_addr
#include <string.h>

#define BUFFER_SIZE 1024

int main(int argc, char ** argv){

    int port = 8080;                    // default port
    char * remoteIP = "127.0.0.1";      // local address

    char username[BUFFER_SIZE];         //username
    char message[BUFFER_SIZE];          //message to send
    char quittingMsg[3] = "bye";        //quit message

    // check if port and remoteIP are given as arguments
    if(argc == 3){
        port = atoi(argv[1]);
        remoteIP = argv[2];
    }

    // init socket descriptor, exit if error. Use SOCK_DGRAM for UDP sockets
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
        perror("Socket creation failed:");
        exit(EXIT_FAILURE);
    }

    // init server address struct
    struct sockaddr_in remoteAddress;
    remoteAddress.sin_family = AF_INET;
    remoteAddress.sin_port = htons(port);
    remoteAddress.sin_addr.s_addr = inet_addr(remoteIP);

    printf("------------ Welcome to the Murphy UDP chat! --------------\n");
    printf("If anything can go wrong, it will. \n");
    
    printf("Please, enter username: ");
    
    if( fgets(username, BUFFER_SIZE, stdin) == NULL ){
        perror("Like I said, it will:");
        exit(EXIT_FAILURE);
    }
    username[strlen(username) - 1] = '\0';

    // greatings
    printf("Hello %s\n", username);
    sendto(sockfd, username, strlen(username) + 1, 0, (struct sockaddr *)&remoteAddress, sizeof(remoteAddress));

    do{
        // send message to server
        printf("Say something nice (or bye for exit): ");
        fgets(message, BUFFER_SIZE, stdin);
        sendto(sockfd, message, strlen(message) + 1, 0, (struct sockaddr *)&remoteAddress, sizeof(remoteAddress));
    }while(strncmp(message, quittingMsg, 3) != 0);

    printf("See you next time!\n");
    printf("---------------------------\n");

    // close socket descriptor
    close(sockfd);
}