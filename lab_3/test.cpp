#include "cpp-httplib-master/httplib.h"
#include <iostream>

using namespace std;

int main() {

    setlocale(LC_ALL, "RU");
    httplib::Server svr;

    // ��������� POST-������� �� ���� '/buttonClicked'
    svr.Post("/buttonClicked", [](const httplib::Request& req, httplib::Response& res) {
        // ����� ��������� � �������
        cout << "���� ������� ������" << endl;
        });

    // ��������� ����������� ������ �� �������� �������� (��������, index.html)
    svr.set_base_dir("./");

    cout << "������ ������� �� ����� 8080" << endl;
    // ������ ������� �� ����� 8080
    svr.listen("0.0.0.0", 8080);

    return 0;
}