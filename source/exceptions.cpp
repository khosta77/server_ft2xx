#include "exceptions.hpp"

void ModuleFT2xxException::currentTime( std::ostream& os )
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
       << "]";
}

ModuleFT2xxException::ModuleFT2xxException( const unsigned int& code )
{
    std::stringstream ss;
    currentTime( ss );
    if( ftdiErrorCodes_.count( code ) )
    {
        ss << " " << ftdiErrorCodes_[code] << std::endl;
    }
    else
    {
        ss << " HOLY FUCK!!! code = " << code << std::endl;
    }
    m_msg = ss.str();
}

const char* ModuleFT2xxException::what() const noexcept
{
    return m_msg.c_str();
}


