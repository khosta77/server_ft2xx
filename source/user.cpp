#include <cmath>
#include <tuple>
#include <ctime>
#include <vector>
#include <string>
#include <format>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <random>
#include <atomic>
#include <thread>
#include <limits>
#include <iostream>
#include <exception>
#include <algorithm>
#include <unordered_map>
#include <condition_variable>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "proto/server.pb.h"

#include "universalservermethods.hpp"

void currentTime( std::ostream& os )
{
    std::time_t now = std::time(nullptr);
    std::tm* local_time = std::localtime(&now);

    os << "["
       << std::setfill('0') << std::setw(2) << local_time->tm_mday << "-"
       << std::setfill('0') << std::setw(2) << (local_time->tm_mon + 1) << "-"
       << std::setfill('0') << std::setw(4) << (local_time->tm_year + 1900) << " "
       << std::setfill('0') << std::setw(2) << local_time->tm_hour << ":"
       << std::setfill('0') << std::setw(2) << local_time->tm_min << ":"
       << std::setfill('0') << std::setw(2) << local_time->tm_sec
       << "]: ";
}

class Writer : public UniversalServerMethods
{
private:
    const std::string my_name_;

    int sock_;
    struct sockaddr_in addr;

    void socketInit();

    void fillAddress( const std::string&, const int& );

    void connectInit();

    void buildBridge();

    std::string packMessage( const uint32_t id, const std::string& logFrame );

public:
    Writer( const std::string& IP, const int& PORT, const std::string& MY_NAME = "UserFT2xx" ) : my_name_(MY_NAME)
    {
        socketInit();
        fillAddress( IP, PORT );
        connectInit();
        buildBridge();
    }
    
    ~Writer()
    {
        close(sock_);
    }

    void write( const std::string& text )
    {
        const std::string serverMessage = packMessage( 200, text );
        writeToSock( sock_, serverMessage );
    }
};

int main()
{
    Writer wr( "127.0.0.1", 34100 );
    while( true )
    {
        currentTime( std::cout );
        std::string message;
        std::getline( std::cin, message );
        wr.write( message );
    }
    return 0;
}

void Writer::socketInit()
{
    sock_ = socket( AF_INET, SOCK_STREAM, 0 );
    if( sock_ < 0 )
        throw std::runtime_error( ( "listener = " + std::to_string(sock_) ) );
}

void Writer::fillAddress( const std::string& IP, const int& PORT )
{
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP.c_str());
}

void Writer::connectInit()
{
    if( connect( sock_, ( struct sockaddr* )&addr, sizeof(addr) ) < 0 )
        throw std::runtime_error( "connect" );
}

void Writer::buildBridge()
{
    ServerInteraction::WhoWantsToTalkToMe wwtttm_;
    wwtttm_.set_whoami( my_name_ );

    std::string msg;
    wwtttm_.SerializeToString( &msg );
    writeToSock( sock_, msg );
    msg.clear();
}

std::string Writer::packMessage( const uint32_t id, const std::string& logFrame )
{
    std::string msg = "";
    ServerInteraction::Message message_;
    message_.set_id( id );
    message_.set_text( logFrame );
    message_.SerializeToString( &msg );
    return msg;
}

