# Клиенто-серверное приложение

## 1. Установка Make, если он еще не установлен на вашем компьютере
   Для установка CMake потребуется выолнить следующие шаги в консоли.
  ```
   sudo apt-get update
   sudo apt-get install cmake 
   ```
## 2. Само скачивание проекта
   ```
   git clone https://github.com/Karina-Zakieva/Laba
   ```
## 3. Перейдем в папку, которую мы создали путем копирования другой папки
   После клонирования репозитория порции в директорию, которую он создал
   ```
   cd client_server
   ```
## 4. Использование команды cmake для генерации файлов проекта
   ```
   cmake -S . -B build
   ```
## 5. Использование команды make для сборки проекта
   ```
   make -C build
   ```
# После успешной сборки проекта с помощью CMake и Make вы можете запустить сервер и клиент из командной строки
## 6. Запускаем сервер
   ```
   ./build/server 8080 save
   ```
## 7. Запускаем клиента
   ```
   ./build/client 127.0.0.1 8080 file.txt
   ```
Данный запуск подразумевает в себе, что сервер запущен на локальном хосте (127.0.0.1) на порту 8080, и происходит отправка нами файла «file.txt» на сервер
Стоит помнить, что сначала нужно запустить сервер перед запускомм самого клиента. Одно из важных, в чем стоит убедиться, что клиент отправляет файл, который действительно существует в вашем текущем рабочем каталоге.