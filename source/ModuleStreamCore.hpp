#ifndef MODULESTREAMCORE_HPP_
#define MODULESTREAMCORE_HPP_

#include "ModuleFT232RL.hpp"
#include "universalservercore.hpp"
#include "universalservermethods.hpp"

#include "proto/server.pb.h"

class ModuleStreamCore : public UniversalServerCore, public UniversalServerMethods
{
private:
    ModuleFT232RL module_ft2xx_;


    std::atomic<bool> infinit_work_;
    std::thread read_;

    void ThreadFoo()
    {
        while( infinit_work_ )
        {
            std::vector<char> text = module_ft2xx_.read<char>( 5000 );
            std::cout << ">>>:" << std::string( text.begin(), text.end() ) << std::endl;
        }
    }
public:

    ModuleStreamCore( const int id = 0 ) : module_ft2xx_(id), infinit_work_(true)
    {
        read_ = std::thread( &ModuleStreamCore::ThreadFoo, this );
        std::cout << module_ft2xx_ << std::endl;
    }
    
    ~ModuleStreamCore()
    {
        infinit_work_ = false;
        if( read_.joinable() )
            read_.join();
    }

    /*
     * @brief так как решение универсальное, то в некоторых сценариях возможно нужна будет инициализация
     * */
    void Init() override
    {

    }

    /*
     * @brief метод для того чтобы вызвать при подтверждении приема сообщения
     * @param 1 - сокет
     * @param 2 - имя пользователя ( возможно использовании во внутренней логики )
     * @param 3 - сообщение, это закодированная строка, в ней может быть все что угодно.
     * */
    void operator()( [[maybe_unused]] const int, const std::string& source_,
        const std::string& message_ ) override
    {
        ServerInteraction::Message buffer_server_;
        if( !buffer_server_.ParseFromString(message_) )
        {
            const std::string emsg = std::format( "!!!> {} -> not parse: {}\n", source_, message_ );
            throw std::invalid_argument( emsg );
        }

        if( !buffer_server_.IsInitialized() )
        {
            const std::string emsg = std::format( "!!!> {} -> not initialized: {}\n", source_, message_ );
            throw std::logic_error( emsg );
        }

        module_ft2xx_.writeData<char>( std::vector<char>( buffer_server_.text().begin(),
                buffer_server_.text().end() ) );
        std::cout << "<<<:" << buffer_server_.text() << std::endl;
    }

    /*
     * @brief запуск каго-то внутреннего действия
     * */
    void Launch() override
    {

    }

    /*
     * @brief остановка какого-то внутреннего действия
     * */
    void Stop() override
    {

    }
};

#endif  // MODULESTREAMCORE_HPP_
