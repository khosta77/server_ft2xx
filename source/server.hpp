#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <iostream>
#include <vector>
#include <string>
#include <format>
#include <cstdint>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <deque>
#include <functional>
#include <exception>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>

#include "proto/server.pb.h"

#include "universalservercore.hpp"
#include "universalservermethods.hpp"


class Server : protected UniversalServerMethods
{
private:
    const int MAX_CLIENTS = 1024;

    std::unique_ptr<int[]> clients_fds_ = std::make_unique<int[]>( MAX_CLIENTS );
    std::unique_ptr<struct pollfd[]> fds_ = std::make_unique<struct pollfd[]>( ( MAX_CLIENTS + 1 ) );

    std::unordered_map<int, std::string> clients_name_;

    std::string ip_;
    int port_;
    int server_fd_;
    int nfds_ = 1;
    
    struct sockaddr_in server_addr_;
    struct sockaddr_in client_addr_;
    socklen_t client_len_ = sizeof(client_addr_);
    
    std::unique_ptr<UniversalServerCore> core_;

    /*
     *  @brief launchServer - запускает сервер
     * */
    void launchServer();

    /*
     * @brief settingsFileDescriptor - настройка файлогового дескриптора
     * */
    void settingsFileDescriptor();

    /*
     * @brief checkingSocketsOnNewConnect - проверяет новые подключения к серверу
     * */
    void checkingSocketsOnNewConnect();

    /*
     * @brief ifMessageEmptyCloseSocket - Метод для закрытия сокетов, если сообщения пустые
     * */
    bool ifMessageEmptyCloseSocket( const int );
    
    /*
     * @brief get_WhoAmI_Info - надо проверить при первом подключении, что, тот с кем хотим
     *                          работать имеет имя
     * @see Диаграмму С4
     * */
    bool get_WhoAmI_Info( const int, std::string& );

    /*
     * @brief processTheRequest - когда проверки доходят до этого метода можно быть увереным,
     *                            что у нас нормальное сообщение, единственное в чем может остатся косяк,
     *                            это некорректное будет от protobuf
     * */
    void processTheRequest( const int, std::string& );

    /*
     * @brief checkingSocketsOnNewContent - проверяет какие сообщения пришли от сокетов пользователей
     * */
    void checkingSocketsOnNewContent();

public:
    Server( const std::string&, const int&,  std::unique_ptr<UniversalServerCore> );
    Server( const Server& ) = delete;
    Server( Server&& ) = delete;
    Server& operator=( const Server& ) = delete;
    Server& operator=( Server&& ) = delete;
    ~Server();

    int run();
};

#endif  // SERVER_HPP_
