// client.cpp (обновленный код)

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>

const int BUFFER_SIZE = 1024;

void send_file(const std::string& server_address, int port, const std::string& file_path) {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Error creating socket.\n";
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_address.c_str(), &(server_addr.sin_addr)) <= 0) {
        std::cerr << "Invalid address/Address not supported.\n";
        close(client_socket);
        return;
    }

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Connection failed.\n";
        close(client_socket);
        return;
    }

    // Отправка информации о файле
    std::string file_info = file_path + " " + file_path.substr(file_path.find_last_of('/') + 1);
    send(client_socket, file_info.c_str(), file_info.size(), 0);

    // Отправка файла
    std::ifstream infile(file_path, std::ios::binary);
    if (!infile.is_open()) {
        std::cerr << "Error opening file.\n";
        close(client_socket);
        return;
    }

    char buffer[BUFFER_SIZE];
    while (infile.read(buffer, sizeof(buffer)).gcount() > 0) {
        send(client_socket, buffer, infile.gcount(), 0);
    }

    infile.close();

    close(client_socket);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <server_address> <port> <file_path>\n";
        return 1;
    }

    std::string server_address(argv[1]);
    int port = std::atoi(argv[2]);
    std::string file_path(argv[3]);

    send_file(server_address, port, file_path);

    return 0;
}