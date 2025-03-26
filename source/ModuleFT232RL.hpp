#ifndef SOURCE_MODULE_FT232RL_HPP_
#define SOURCE_MODULE_FT232RL_HPP_

#include <cassert>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>

extern "C"
{
#include "ftd2xx.h"
};

#include "exceptions.hpp"

class ModuleFT232RL
{
    unsigned int device_id_;

    // http://microsin.net/programming/pc/ftdi-d2xx-functions-api.html - ds
	FT_HANDLE ftHandle;
	//[[ maybe_unused ]] FT_STATUS ftStatus;
	//[[ maybe_unused ]] DWORD EventDWord;
	//[[ maybe_unused ]] DWORD TxBytes;
	//[[ maybe_unused ]] DWORD RxBytes;
	DWORD BytesReceived;
    DWORD BytesWritten;

	int baudrate_ = 9600;

	struct DeviceInfo
    {
		DWORD flags_;
		DWORD id_;
		DWORD type_;
		DWORD locId_;
	    	
		char serialNumber_[16];
		char description_[64];

		FT_HANDLE ftHandleTemp_;
	} device_info_[3];

	void getDeviceInfo()
    {
		DWORD numDevs;

        if( FT_STATUS code = FT_CreateDeviceInfoList( &numDevs ); code != FT_OK )
            throw ModuleFT2xxException( code );
		
        if( ( ( device_id_ < 0 ) and ( device_id_ >= numDevs ) ) )
            throw ModuleFT2xxException( FT_DEVICE_NOT_FOUND );
		
        FT_STATUS code = FT_GetDeviceInfoDetail(
            device_id_, &device_info_[device_id_].flags_, &device_info_[device_id_].type_,
            &device_info_[device_id_].id_, &device_info_[device_id_].locId_,
            device_info_[device_id_].serialNumber_, device_info_[device_id_].description_,
            &device_info_[device_id_].ftHandleTemp_
        );

        if( code != FT_OK )
            throw ModuleFT2xxException( code );
	}

    template<typename T>
    void writeData( const std::vector<T>& frame_ )
    {
        const size_t SIZE = ( sizeof(T) * frame_.size() );
        std::vector<uint8_t> buffer_( SIZE );

        std::memcpy( buffer_.data(), frame_.data(), SIZE );

        if( FT_STATUS code = FT_Write( ftHandle, buffer_.data(), SIZE, &BytesWritten ); code != FT_OK )
            throw ModuleFT2xxException( code );
	}

    template<typename T>
	void readData( std::vector<T>& frame_ )
    {
        const size_t SIZE = ( sizeof(T) * frame_.size() );
        std::vector<uint8_t> buffer_( SIZE );

        if( FT_STATUS code = FT_Read( ftHandle, buffer_.data(), SIZE, &BytesReceived ); code != FT_OK )
            throw ModuleFT2xxException( code );

        std::memcpy( frame_.data(), buffer_.data(), SIZE );
	}

public:
	ModuleFT232RL( const int &dev_id = 0 ) : device_id_(dev_id)
    {
		getDeviceInfo();

        if( FT_STATUS code = FT_Open( device_id_, &ftHandle ); code != FT_OK )
            throw ModuleFT2xxException( code );

        this->setBaudRate();
	}

	~ModuleFT232RL()
    {
		FT_Close(ftHandle);
	}

    void setBaudRate( const int baudrate = 9600 )
    {
        if( FT_STATUS code = FT_SetBaudRate( ftHandle, baudrate ); code != FT_OK )
            throw ModuleFT2xxException( code );

        baudrate_ = baudrate;
	}
    
    /** @brief setCharacteristics - задание характеристик передачи
     *  @param wordlenght - количество бит на слово (фрейм) - тут должно быть значение FT_BITS_8 или FT_BITS_7
     *  @param stopbit - количество стоп-бит - должно быть FT_STOP_BITS_1 или FT_STOP_BITS_2
     *  @param parity - четность - должно быть FT_PARITY_NONE, FT_PARITY_ODD, FT_PARITY_EVEN, \
     *                  FT_PARITY_MARK или FT_PARITY SPACE
     * */
    void setCharacteristics( const UCHAR &wordlenght = FT_BITS_8, 
        const UCHAR &stopbit = FT_STOP_BITS_1, const UCHAR &parity = FT_PARITY_NONE )
    {
        FT_STATUS code = FT_SetDataCharacteristics( ftHandle, wordlenght, stopbit, parity );
        if( code != FT_OK )
            throw ModuleFT2xxException( code );
    }

	/* @brief функция отправляет данные на устройство. В примере использования FT_Read были еще статусы
     *        какие-то и задержки, в общем они не работали
	 * @param frame_ - массив, который отправляем
	 * */
	void writeData8( const std::vector<uint8_t>& frame_ ) { return writeData<uint8_t>( frame_ ); }
    void writeData16( const std::vector<uint16_t>& frame_ ) { return writeData<uint16_t>( frame_ ); }
    void writeData32( const std::vector<uint32_t>& frame_ ) { return writeData<uint32_t>( frame_ ); }

	/* @brief функция читает данные с устройства. В примере использования FT_Read были еще статусы
     *        какие-то и задержки, в общем они не работали
	 * @param frame_ - массив, который считываем. НАДО ЗАРАНЕЕ ЗНАТЬ ЕГО РАЗМЕР
	 * */
    void readData8( std::vector<uint8_t>& frame_ ) { return readData<uint8_t>( frame_ ); }
	void readData16( std::vector<uint16_t>& frame_ ) { return readData<uint16_t>( frame_ ); }
	void readData32( std::vector<uint32_t>& frame_ ) { return readData<uint32_t>( frame_ ); }

    friend std::ostream& operator<<( std::ostream&, const ModuleFT232RL& );
};

std::ostream& operator<<( std::ostream& os, const ModuleFT232RL& ft_ )
{
    os << std::dec << "Dev " << ft_.device_id_ << ":" << std::endl;
    os << std::hex << "\tFlags = 0x" << ft_.device_info_[ft_.device_id_].flags_ << std::endl;
    os << std::hex << "\tType = 0x" << ft_.device_info_[ft_.device_id_].type_ << std::endl;
    os << std::hex << "\tID = 0x" << ft_.device_info_[ft_.device_id_].id_ << std::endl;
    os << std::hex << "\tLocId = 0x" << ft_.device_info_[ft_.device_id_].locId_ << std::endl;
    os << std::dec << "\tSerialNumber = " << ft_.device_info_[ft_.device_id_].serialNumber_ << std::endl;
    os << std::dec << "\tDescription = " << ft_.device_info_[ft_.device_id_].description_ << std::endl;
    return os;
}

#endif  // SOURCE_MODULE_FT232RL_HPP_
