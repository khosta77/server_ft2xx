#ifndef EXCEPTIONS_HPP_
#define EXCEPTIONS_HPP_

#include <ctime>
#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <unordered_map>

class ModuleFT2xxException : public std::exception
{
private:
    std::unordered_map<int, std::string> ftdiErrorCodes_ = {
        { 1,  "FT_INVALID_HANDLE" },
        { 2,  "FT_DEVICE_NOT_FOUND" },
        { 3,  "FT_DEVICE_NOT_OPENED" },
        { 4,  "FT_IO_ERROR" },
        { 5,  "FT_INSUFFICIENT_RESOURCES" },
        { 6,  "FT_INVALID_PARAMETER" },
        { 7,  "FT_INVALID_BAUD_RATE" },
        { 8,  "FT_DEVICE_NOT_OPENED_FOR_ERASE" },
        { 9,  "FT_DEVICE_NOT_OPENED_FOR_WRITE" },
        { 10, "FT_FAILED_TO_WRITE_DEVICE" },
        { 11, "FT_EEPROM_READ_FAILED" },
        { 12, "FT_EEPROM_WRITE_FAILED" },
        { 13, "FT_EEPROM_ERASE_FAILED" },
        { 14, "FT_EEPROM_NOT_PRESENT" },
        { 15, "FT_EEPROM_NOT_PROGRAMMED" },
        { 16, "FT_INVALID_ARGS" },
        { 17, "FT_NOT_SUPPORTED" },
        { 18, "FT_OTHER_ERROR" }
    };
    std::string m_msg;

    void currentTime( std::ostream& os );

public:
    explicit ModuleFT2xxException( const unsigned int& code );

    const char* what() const noexcept override;
};

#endif  // EXCEPTIONS_HPP_
