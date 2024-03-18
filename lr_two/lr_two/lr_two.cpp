#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <ctime>
#include <iomanip>
#include <csignal>
#include <thread>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const int MAX_THREADS = 10;
const int BUFFER_SIZE = 1024;

volatile bool terminate_flag = false;

std::string generate_unique_filename(const std::string& save_path, const std::string& original_filename) {
    time_t current_time = time(nullptr);
    struct tm* time_info = localtime(&current_time);

    std::ostringstream new_filename;
    new_filename << save_path << "/file_"
        << std::setw(2) << std::setfill('0') << time_info->tm_year + 1900
        << std::setw(2) << std::setfill('0') << time_info->tm_mon + 1
        << std::setw(2) << std::setfill('0') << time_info->tm_mday
        << std::setw(2) << std::setfill('0') << time_info->tm_hour
        << std::setw(2) << std::setfill('0') << time_info->tm_min
        << std::setw(2) << std::setfill('0') << time_info->tm_sec
        << ".txt";

    return new_filename.str();
}

void handle_client(int client_socket, const std::string& save_path) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // Получение информации о файле
    recv(client_socket, buffer, sizeof(buffer), 0);
    std::string file_info = buffer;

    // Получение данных от клиента
    std::ostringstream received_data;
    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        received_data.write(buffer, bytes_received);
    }

    // Создание полного пути и уникального имени для сохранения файла
    std::istringstream file_info_stream(file_info);
    std::string file_name, original_file_name;
    file_info_stream >> original_file_name >> file_name;
    std::string full_path = generate_unique_filename(save_path, file_name);

    // Сохранение данных в файл
    std::ofstream outfile(full_path, std::ios::binary);
    if (outfile.is_open()) {
        outfile << received_data.str();
        outfile.close();
        std::cout << "File saved successfully: " << full_path << "\n";
    }
    else {
        std::cerr << "Error saving file.\n";
    }

    close(client_socket);
}

// Обработчик сигнала SIGTERM и SIGHUP
void signal_handler(int signum) {
    if (signum == SIGTERM || signum == SIGHUP) {
        std::cout << "Received signal " << signum << ". Terminating...\n";
        terminate_flag = true;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <save_directory>\n";
        return 1;
    }

    int port = std::atoi(argv[1]);
    std::string save_directory(argv[2]);

    // Проверка и создание директории, если её нет
    if (mkdir(save_directory.c_str(), 0777) == -1 && errno != EEXIST) {
        std::cerr << "Error creating save directory.\n";
        return 1;
    }

    // Регистрация обработчика сигнала
    signal(SIGTERM, signal_handler);
    signal(SIGHUP, signal_handler);

    // Создание демона
    pid_t pid, sid;
    pid = fork();
    if (pid < 0) {
        std::cerr << "Error forking process.\n";
        return 1;
    }
    if (pid > 0) {
        // Родительский процесс завершает работу
        return 0;
    }
    umask(0); // Установка маски файлов

    // Создание новой сессии
    sid = setsid();
    if (sid < 0) {
        std::cerr << "Error creating new session.\n";
        return 1;
    }

    // Закрытие стандартных дескрипторов ввода/вывода/ошибок
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error creating socket.\n";
        return 1;
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Error binding socket.\n";
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, 10) == -1) {
        std::cerr << "Error listening on socket.\n";
        close(server_socket);
        return 1;
    }

    while (!terminate_flag) {
        sockaddr_in client_address{};
        socklen_t client_addr_len = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_addr_len);

        if (client_socket == -1) {
            std::cerr << "Error accepting connection.\n";
            continue;
        }

        std::thread(handle_client, client_socket, save_directory).detach();
    }

    close(server_socket);
    return 0;
}
