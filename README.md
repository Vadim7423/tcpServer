# tcpServer

Тестовый проект сервера, который принимает запросы от клиентов по tcp, с поддержкой нескольких одновременных подключений. Проект написан с использованием C++17. 

## Инструменты сборки

```bash
sudo apt install git cmake gcc g++ ninja-build
```

## Скачивание проекта

Создать папку, где будет лежать проект и перейти в неё 
```bash
mkdir -p <path_to_dir> && cd <path_to_dir>
```
Скачиваем проект по http через git
```bash
git clone https://github.com/Vadim7423/tcpServer.git .
```

## Сборка проекта (в консоли)

Создаём папку сборки 
```bash
mkdir build
```
Сборка
```bash
cmake .. 
make
```

## Настройки по умолчанию
Host: 127.0.0.1
Port: 12345