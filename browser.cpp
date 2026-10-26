#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

int main() {
    std::string url;
    std::cout << "Enter website address (e.g., example.com): ";
    std::cin >> url;

    // Manually resolve the domain name to an IP address using system structures
    struct hostent* server = gethostbyname(url.c_str());
    if (server == nullptr) {
        std::cerr << "Error: Could not resolve host." << std::endl;
        return 1;
    }

    // Initialize raw network address structures in memory
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr)); // Manually clear memory space
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80); // HTTP Port
    std::memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // Create the raw socket descriptor
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }

    // Connect to the remote hardware server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed." << std::endl;
        close(sock);
        return 1;
    }

    // Construct the raw HTTP string
    std::string request = "GET / HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";
    send(sock, request.c_str(), request.length(), 0);

    // RAW MEMORY CONTROL: We must create a specific hardware byte buffer (1KB chunk)
    char buffer[1024]; 
    ssize_t bytes_received;

    // We manually read the bytes straight into our hardware array chunk by chunk
    while ((bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0'; 
        std::cout << buffer;           
    }

    // Manually close the hardware socket to free up the OS resource
    close(sock); 
    return 0;
}
