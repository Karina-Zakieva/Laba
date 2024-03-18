#include "cpp-httplib-master/httplib.h"
#include <iostream>

using namespace std;

int main() {

    setlocale(LC_ALL, "RU");
    httplib::Server svr;

    // Обработка POST-запроса по пути '/buttonClicked'
    svr.Post("/buttonClicked", [](const httplib::Request& req, httplib::Response& res) {
        // Вывод сообщения в консоль
        cout << "Было нажатие кнопки" << endl;
        });

    // Установка статических файлов из текущего каталога (например, index.html)
    svr.set_base_dir("./");

    cout << "Запуск сервера на порту 8080" << endl;
    // Запуск сервера на порту 8080
    svr.listen("0.0.0.0", 8080);

    return 0;
}