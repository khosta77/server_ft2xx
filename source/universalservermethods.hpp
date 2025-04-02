#ifndef UNIVERSALSERVERMETHODS_HPP_
#define UNIVERSALSERVERMETHODS_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>

#include <iostream>
#include <vector>
#include <string>
#include <format>

class UniversalServerMethods
{
private:
    /*
     *  Критическая уязвимость в данной переменной, если она будет слишком большой, то в буфере обработки
     *  сообщений воникнет критический баг: Как работает передача, все происходит безпрерывным потоком из
     *  потока передачи, если скорость приема/передачи/считывания очень большая то если за раз пришло N
     *  сообщений, то он могут за раз считатся, по итогу произойдет их склеивание при большом входном
     *  буфере и там тупо n посылок суммируются и мы в протобуф кидаем мусор не отфильтрованный, там не
     *  обратно сообщение не отправить, ни понять что в целом происходит. Это можно задебажить костылем,
     *  где мы будет поток обрабатывать
     * */
    const size_t MAX_BUFFER_COUNT = 1024;

public:
    UniversalServerMethods() = default;
    ~UniversalServerMethods() = default;

    UniversalServerMethods( const UniversalServerMethods& ) = delete;
    UniversalServerMethods( UniversalServerMethods&& ) = delete;
    UniversalServerMethods& operator=( const UniversalServerMethods& ) = delete;
    UniversalServerMethods& operator=( UniversalServerMethods&& ) = delete;

    std::string readFromSock( const int socket_ ); 
    void writeToSock( const int socket_, std::string msg );

    /*
     * @brief split - дробление всей посылки на малые части -> отдельные сообщения
     * */
    std::vector<std::string> split( const std::string&  );
};

#endif  // UNIVERSALSERVERMETHODS_HPP_
