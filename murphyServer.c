/* SERVER */
#include <stdio.h>          // standard i/o library
#include <stdlib.h>         // standard library
#include <unistd.h>         // standard Unix interface
#include <sys/socket.h>     // required for socket
#include <arpa/inet.h>      // required for inet_addr
#include <string.h>

#define MAX_USERS_ALLOWED 10

struct user {
    char username[1024];
    int  IP;
};

int main(int argc, char ** argv){

    int port = 8080;            // default port
    char * myIP = "127.0.0.1";  // local address

    char buffer[1024] = "";

    // check if port and remoteIP are given as arguments
    if(argc == 3){
        port = atoi(argv[1]);
        myIP = argv[2];
    }

    // init socket descriptor, exit if error. Use SOCK_DGRAM for UDP sockets
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
        perror("Socket creation failed:");
        exit(EXIT_FAILURE);
    }

    // init local address struct
    struct sockaddr_in localAddress;
    localAddress.sin_family = AF_INET;
    inet_aton(myIP, &localAddress.sin_addr); 
    localAddress.sin_port = htons(port);
    localAddress.sin_addr.s_addr = INADDR_ANY;

    // init client connection support struct
    struct sockaddr_in userConnection;

    struct user userList[MAX_USERS_ALLOWED];        // online users
    int userCount = 0;                              // online users count

    // init empty user list
    for(int i = 0; i < MAX_USERS_ALLOWED; i++){
        userList[i].IP = 0;
        userList[i].username[0] = '\0';
    }

    // bind socket to port
    if(bind(sockfd, (struct sockaddr *)&localAddress, sizeof(localAddress)) < 0){
        perror("Bind failed:");
        exit(EXIT_FAILURE);
    }

    printf("Murphy server is running on port %d\n", port);
    printf("Waiting for user message....\n");

    while(1){
        recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr * restrict)&userConnection, (socklen_t * restrict)sizeof(userConnection));
        if(strlen(buffer) > 0){
            short int newJoin = 1;

            // remove user from list if she/he left
            if(strncmp(buffer, "bye", 3) == 0){
                for(int i = 0; i < userCount; i++){
                    if(userList[i].IP == userConnection.sin_addr.s_addr){
                        printf("%s left the chat\n", userList[i].username);
                        userList[i].IP = 0;
                        userList[i].username[0] = '\0';
                        userCount--;
                        newJoin = 0;
                        break;
                    }
                }
            }

            // if user is already in list, print his/her message
            for(int i = 0; i < userCount; i++){
                if(userList[i].IP == (userConnection.sin_addr.s_addr)){
                    printf("%s says %s\n", userList[i].username, buffer);
                    newJoin = 0;
                }
            }

            // if user is not in list, add him/her to list and print welcome message
            if(newJoin){
                for(int i = 0; i < MAX_USERS_ALLOWED; i++){
                    if(userList[i].IP == 0){
                        userList[i].IP = userConnection.sin_addr.s_addr;
                        strncpy(userList[i].username, buffer,1024);
                        userCount++;
                        printf("%s joined the chat\n", userList[i].username);
                        break;
                    }
                }
            }
        }
    }
}