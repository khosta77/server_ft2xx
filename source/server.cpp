#include "server.hpp"

/*
 * @brief launchServer - запускает сервер
 * */
void Server::launchServer()
{
    if( int listenCode = listen( server_fd_, SOMAXCONN ); listenCode )
    {
        close(server_fd_);
        throw std::runtime_error(  std::format( "---> listen FAILURE!!! code: {}\n", listenCode ) );
    }
    // Сокет переведен в режим прослушивания

    std::cout << std::format( "Server launch: {}:{}", ip_, port_ ) << std::endl;
}

/*
 * @brief settingsFileDescriptor - настройка файлогового дескриптора
 * */
void Server::settingsFileDescriptor()
{
    // Настраиваем массив pollfd
    fds_[0].fd = server_fd_;
    fds_[0].events = POLLIN;

    for( int i = 1; i <= MAX_CLIENTS; ++i )
    {
        clients_fds_[i - 1] = -1;
        fds_[i].fd = -1;
    }
}

/*
 * @brief checkingSocketsOnNewConnect - проверяет новые подключения к серверу
 * */
void Server::checkingSocketsOnNewConnect()
{
    if( ( fds_[0].revents & POLLIN ) )
    {
        clients_fds_[nfds_ - 1] = accept(
            server_fd_,
            ( struct sockaddr * )&client_addr_,
            &client_len_
        );

        if( clients_fds_[nfds_ - 1] != -1 )
        {
            std::cout <<
                std::format( "===> New connect: FileDescriptor = {}", clients_fds_[( nfds_ - 1 )] )
                << std::endl;
            fds_[nfds_].fd = clients_fds_[( nfds_ - 1 )];
            fds_[nfds_].events = POLLIN;
            ++nfds_;
        }
        else
            std::cerr << "---> \"accept\" in checkingSocketsOnNewContentOrConnect" << std::endl;
    }
}

/*
 * @brief ifMessageEmptyCloseSocket - Метод для закрытия сокетов, если сообщения пустые
 * */
bool Server::ifMessageEmptyCloseSocket( const int i, std::string& message )
{
    if( !message.empty() )
        return false;

    int buf = fds_[i].fd;
    for( auto client = clients_name_.begin(); client != clients_name_.end(); ++client )
    {
        if( (*client).second == clients_name_[fds_[i].fd] )
        {
            clients_name_.erase( client );
            break;
        }
    }
    close( fds_[i].fd );
    fds_[i].fd = -1;
    std::cout << std::format( "===> User disconnect: FileDescriptor = {}", buf ) << std::endl;
    return true;
}

/*
 * @brief get_WhoAmI_Info - надо проверить при первом подключении, что, тот с кем хотим
 *                          работать имеет имя
 * @see Диаграмму С4
 * */
bool Server::get_WhoAmI_Info( const int i, std::string& message )
{
    if( clients_name_.count( fds_[i].fd ) )
        return false;

     logs::WhoWantsToTalkToMe wwttm_;
    if( !wwttm_.ParseFromString( message ) )
    {
        std::cerr << std::format(
            "!!!> whoami = {}; have some problem with parse: {}", fds_[i].fd, message ) << std::endl;
        return true;
    }

    if( !wwttm_.IsInitialized() )
    {
        std::cerr << std::format(
            "!!!> whoami = {}; have not IsInitialized: {}", fds_[i].fd, message ) << std::endl;
        return true;
    }

    clients_name_[ fds_[i].fd ] = std::move( wwttm_.whoami() );
    return true;
}

/*
 * @brief processTheRequest - когда проверки доходят до этого метода можно быть увереным,
 *                            что у нас нормальное сообщение, единственное в чем может остатся косяк,
 *                            это некорректное будет от protobuf
 * */
void Server::processTheRequest( const int i, std::string& message )
{
    std::string name = clients_name_[fds_[i].fd];

    logs::Log log;
    if( !log.ParseFromString(message) )
    {
        std::cerr << std::format( "!!!> {} -> not parse: {}\n", name, message ) << std::endl;
        return;
    }

    if( !log.IsInitialized() )
    {
        std::cerr << std::format( "!!!> {} -> not initialized: {}\n", name, message ) << std::endl;
        return;
    }

    if( !logger_->push( fds_[i].fd, clients_name_[fds_[i].fd], log ) )
        std::cerr << "4xFUCK!!!!" << std::endl;
}

/*
 * @brief checkingSocketsOnNewContent - проверяет какие сообщения пришли от сокетов пользователей
 * */
void Server::checkingSocketsOnNewContent()
{
    for( int i = 1; i < nfds_; ++i )
    {
        if( fds_[i].revents & POLLIN )
        {
            std::string message = readFromSock( fds_[i].fd );

            if( ifMessageEmptyCloseSocket( i, message ) )
                continue;

            if( get_WhoAmI_Info( i, message ) )
                continue;

            processTheRequest( i, message );
        }
    }
}

Server::Server( const std::string& IP, const int& PORT,  std::unique_ptr<Logger> logger ) : ip_(IP),
    port_(PORT), logger_( std::move( logger ) )
{
    server_fd_ = socket( AF_INET, SOCK_STREAM, 0 );
    if( server_fd_ == -1 )
        throw std::runtime_error( std::format( "---> socket isn`t making!!!\n" ) );

    server_addr_.sin_family = AF_INET;
    server_addr_.sin_addr.s_addr = inet_addr( ip_.c_str() );
    server_addr_.sin_port = htons( port_ );

    // Привязка сокета к адресу
    if( int bindCode = bind( server_fd_, (struct sockaddr *)&server_addr_, sizeof(server_addr_) ); bindCode )
    {
        close(server_fd_);
        throw std::runtime_error( std::format( "---> bind FAILURE!!! code: {}\n", bindCode ) );
    }
}

Server::~Server()
{
    close(server_fd_);
    for( int i = 1; i < nfds_; i++ )
    {
        if (fds_[i].fd != -1)
            close(fds_[i].fd);
    }
}

int Server::run()
{
    launchServer();
    settingsFileDescriptor();

    while( true )
    {
        if( int ret = poll( fds_.get(), nfds_, -1 ); ret == -1 )
        {
            std::cout << "poll error" << std::endl;
            return -1;
        }

        checkingSocketsOnNewConnect();
        checkingSocketsOnNewContent();
        logger_->launch();
    }
    logger_->stop();
    return 0;
}
