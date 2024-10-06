#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 100

int main()
{
    //Create a socket 
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket < 0)
    {
        std::cerr << "\nFailed to create a socket";
        return -1;
    }

    // Define the server address
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));         // Clear the structure
    server_address.sin_family = AF_INET;                        // IPv4
    server_address.sin_port = htons(SERVER_PORT);               // Set port number to 8080

    // Convert the IP address from text to the binary form and set the sin_addr field in the server_address struct
    if(inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0)
    {
        std::cerr << "Invalid address or the address is not supported.\n";
        close(client_socket);
        return -1;
    }

    // Connect to the server
    if(connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
    {
        std::cerr << "Connection failed.\n";
        close(client_socket);
        return -1;
    }

    // Send 100 bytes of data to the server
    std::string hi = "Hey there, this is the client trying to reach out to you.";
    ssize_t bytes_sent = send(client_socket, hi.c_str(), hi.length(), 0);

    if(bytes_sent < 0)
    {
        std::cerr << "Failed to send data.\n";
        close(client_socket);
        return -1;
    }

    std::cout << "Sent " << bytes_sent << " bytes to the server." << std::endl;

    // Recieve echo from the server
    char recv_buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(client_socket, recv_buffer, BUFFER_SIZE, 0);
    if(bytes_received < 0)
    {
        std::cerr << "Failed to receive data from server.\n";
        close(client_socket);
        return -1;
    }

    std::cout << "Received " << bytes_received << " bytes from the server : "
              << std::string(recv_buffer, bytes_received) << "\n";

    // Close the socket
    close(client_socket);
    std::cout << "\nConnection closed" << std::endl;
    return 0;
}