#include "universalservermethods.hpp"

std::string UniversalServerMethods::readFromSock( const int socket_ ) 
{
    std::string receivedData;
    std::vector<char> buffer( MAX_BUFFER_COUNT, 0 );

    while(true)
    {
        int bytesReceived = read( socket_, buffer.data(), MAX_BUFFER_COUNT );

        if (bytesReceived == 0)
        {
        //    throw std::runtime_error( "Соединенние закрыто" );
        //    странно, он выдает ошибку при выходе, короче просто не буду учитывать
            break;
        }

        if( bytesReceived < 0 )
            throw std::runtime_error( "Ошибка при чтении" );

        receivedData.append( buffer.data(), bytesReceived );

        if(
            ( !receivedData.empty() ) and
            ( ( receivedData.back() == '\n' ) and ( *( receivedData.rbegin() + 1 ) == '\n' ) )
        )
            break;
    }

    return receivedData;
}

void UniversalServerMethods::writeToSock( const int socket_, std::string msg )
{
    msg += "\n\n";
    const char* dataPtr = msg.c_str();
    size_t dataSize = msg.length();
    size_t totalSent = 0;

    while( totalSent < dataSize )
    {
        // TODO: Что-то мне не нравится, https://ru.manpages.org/write/2
        int bytesSent = write( socket_, ( dataPtr + totalSent ), ( dataSize - totalSent ) );
        if( bytesSent == -1 )
            throw;  // Ошибка при отправке

        totalSent += bytesSent;
    }
}
