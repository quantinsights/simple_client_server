#include <stdio.h>
#include <iostream>         
#include <stdlib.h>         // For exit()
#include <string.h>         // For memset
#include <unistd.h>         // For close()
#include <arpa/inet.h>      // For socket functions and structures
#include <sys/socket.h>

#define PORT 8080

int main()
{
    int server_fd, new_socket;      // File descriptors for the server and new client connection
    int backlog = SOMAXCONN;        // Maximum number of pending connections in listen() API

    struct sockaddr_in address;     // Structure to hold server's address information
    int addrlen = sizeof(address);
    char recv_buffer[1024] = {0};   // Buffer to store received data
    const char* hi = "Hey there, I am the server!";     //Message to send to the client
    
    // 1. Create a TCP Socket using IPv4 protocol (AF_INET) and stream type SOCK_STREAM
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("\nFailed to create a socket to listen to!");
        exit(EXIT_FAILURE);
    }

    // 2. Configure the server address
    // Set address family to IpV4 (AF_INET)
    address.sin_family = AF_INET;

    // Listen to all available network interfaces and specify the port number
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);     //Convert port to network byte order (big-endian)

    // 3. Bind the socket to IP address and port
    if(bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0)
    {
        perror("\nFailed to bind socket to IP address and port!");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 4. Listen for incoming connections
    // The second argument defines the backlog (number of pending connections allowed)
    if(listen(server_fd, backlog) < 0)
    {
        perror("\nListen failed...");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("\nListening on port : %d\n", PORT);

    // 5. Accept a connection from the client
    // This call blocks until a client connects
    if((new_socket == accept(server_fd, (struct sockaddr*) &address, (socklen_t*) addrlen)) < 0)
    {
        perror("\nAccept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("\nConnection accepted.");

    // 6. Read the data sent by the client. This example reads upto 1024 bytes.
    int valread = read(new_socket, recv_buffer, 1024);
    printf("\nReceived: %s\n", recv_buffer);

    // 7. Send a message back to the client
    send(new_socket, hi, strlen(hi), 0);
    printf("\nI waved hi! to the client.\n");


    // 8. Close the client socket when done.
    close(new_socket);

    // 9. CLose the server socket as well
    close(server_fd);
    return 0;
}