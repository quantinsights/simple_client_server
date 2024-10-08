#include <stdio.h>
#include <iostream>         
#include <stdlib.h>         // For exit()
#include <string.h>         // For memset
#include <unistd.h>         // For close()
#include <arpa/inet.h>      // For socket functions and structures
#include <sys/socket.h>
#include <fstream>          // For file I/O

#define PORT 8080

/* When client sends a large amount of data
*/
void receive_large_data(int client_socket)
{
    constexpr int buffer_size = 1024;
    char buffer[buffer_size];
    ssize_t bytes_received {0};
    ssize_t total_bytes_received {0};

    //Open a file to write the data
    std::ofstream output_file("received_data.txt", std::ios::out | std::ios::binary);

    if(!output_file.is_open())
    {
        std::cerr << "\nFailed to open file for writing." << std::endl;
        close(client_socket);
        return;
    }

    // Loop to receiving data until all is received or the connection is closed.
    while(true)
    {
        bytes_received = recv(client_socket, buffer, buffer_size, 0);

        if(bytes_received > 0){
            total_bytes_received += bytes_received;
            std::cout << "\nReceived: " << bytes_received << " bytes, "
                    << " total bytes: " << total_bytes_received << std::endl;
            
            // Write the received data to the file
            output_file.write(buffer, bytes_received);
        }else if(bytes_received == 0)
        {
            //Connection closed by the client
            std::cout << "Connection closed by the client. Total bytes received: " << total_bytes_received << " bytes" << std::endl;
            break;
        }
        else{
            //Error occurred during recv()
            std::cerr << "\nError in recv()!" << std::endl;
            break;
        }
    }

    // Close the file after receving all the data
    output_file.close();

    //Close the socket after receiving all the data
    close(client_socket);
}

//Wrapper function to get errormsg from errno
std::string get_error_message(int error_number)
{
    
}

int main()
{
    int server_fd, new_socket;      // File descriptors for the server and new client connection
    int backlog = 10;        // Maximum number of pending connections in listen() API

    struct sockaddr_in address;     // Structure to hold server's address information
    socklen_t addrlen = sizeof(address);
    char recv_buffer[1024] = {};   // Buffer to store received data
    const char* hi = "Hey there, thanks for reaching out. I am the server!";     //Message to send to the client
    
    // 1. Create a TCP Socket using IPv4 protocol (AF_INET) and stream type SOCK_STREAM
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        std::cout << "Failed to create socket. errno : " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    // 2. Configure the server address
    // Set address family to IpV4 (AF_INET)
    address.sin_family = AF_INET;

    // Listen to all available network interfaces and specify the port number
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);     //Convert port to network byte order (big-endian)

    // Set socket options to reuse the address and port
    const int enable = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(int)) < 0)
    {
        std::cerr << "\nsetsockopt(SO_REUSEADDR) failed";
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 3. Bind the socket to IP address and port
    if(bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0)
    {
        std::cout << "\nFailed to bind to specified address and port. errno : " << errno << std::endl;
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

    // This while loop will handle multiple client connections (serially). accept() is a blocking
    // call. The accept() function shall extract the first connection on the queue of pending connections, 
    // create a new socket with the same socket type protocol and address family as the specified socket, 
    // and allocate a new file descriptor for that socket. 

    while(true)
    {
        // 5. Accept a connection from the client
        new_socket = accept(server_fd, (struct sockaddr*) &address, (socklen_t*) &addrlen);
        if(new_socket < 0)
        {
            std::cout << "Failed to accept connection from the client. errno : " << errno << std::endl;
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        std::cout << "\nConnection accepted.";
        //Receive large data from the client and write it to a file
        receive_large_data(new_socket);
    }

    close(server_fd);
    return 0;
}