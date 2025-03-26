#ifndef SOURCE_MODULE_FT232RL_HPP_
#define SOURCE_MODULE_FT232RL_HPP_

#include <cassert>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <cmath>

extern "C"
{
#include "ftd2xx.h"
};

class ModuleFT232RL
{
    const int device_id_;

    // http://microsin.net/programming/pc/ftdi-d2xx-functions-api.html - ds
	FT_HANDLE ftHandle;
	FT_STATUS ftStatus;
	DWORD EventDWord;
	DWORD TxBytes;
	DWORD RxBytes;
	DWORD BytesReceived;
    DWORD BytesWritten;

	int bauderate = 9600;

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

	bool ftStatusException( const int &module)
    {
		if (ftStatus == FT_OK)
			return true;
		switch (module) {
			case 0: {  // FT_Open
				printf("---> Ошибка FT_Open()\n");
				break;
			}
			case 1: {  // FT_SetBaudRate
				printf("---> Ошибка FT_SetBaudRate, BaudeRate остался %d\n", bauderate);
				break;
			}
			case 2: {  // FT_Write
				printf("---> Ошибка FT_Write, передача не прошла\n");
				break;
			}
			case 3: {
				printf("---> Ошибка FT_Read, передача не прошла\n");
				break;
			}
			case 4: {
				printf("---> Ошибка FT_GetDeviceInfoDetail, информации о девайсе не получено\n");
				break;
			}
			case 5: {
				printf("---> Ошибка FT_CreateDeviceInfoList, не удалось определить количество подключенных устройств\n");
                break;
			}
            case 6: {
                printf("---> Ошибка FT_GetStatus, возникли траблы\n");
                break;
            }
			default: {
				printf("!!!> Ошибка в неизвестном методе: %d\n", module);
			}
		};
		return false;
	}

	bool getDeviceInfo()
    {
		DWORD numDevs;
		bool buffer = false;

		ftStatus = FT_CreateDeviceInfoList(&numDevs);
		buffer = ftStatusException(5);
		
        if( !buffer )
			return buffer;
		
        if( ( device_id_ < 0 ) && ( device_id_ >= numDevs ) )
			return false;
		
        ftStatus = FT_GetDeviceInfoDetail(
            device_id_,
            &device_info_[device_id_].flags_,
            &device_info_[device_id_].type_,
            &device_info_[device_id_].id_,
			&device_info_[device_id_].locId_,
            device_info_[device_id_].serialNumber_,
            device_info_[device_id_].description_, 
			&device_info_[device_id_].ftHandleTemp_
        );
		
        return ftStatusException(4);
	}

    template<typename T>
    bool writeData( T *df, const size_t &size )
    {
		uint8_t buffer_array[( sizeof(T) * size )];
		memcpy(buffer_array, df, (sizeof(T) * size));
        ftStatus = FT_Write( ftHandle, &buffer_array[0], (sizeof(uint16_t) * size), &BytesWritten );
		return ftStatusException(2);
	}

    template<typename T>
	bool readData( T *df, const size_t &size )
    {
		uint8_t buffer_array[( sizeof(T) * size )];
		bool buffer = false;
		ftStatus = FT_Read( ftHandle, &buffer_array[0], ( sizeof(T) * size ), &BytesReceived );
		buffer = ftStatusException(3);
		if (buffer)
			memcpy(df, buffer_array, ( size * sizeof(T) ) );
		return buffer;
	}

public:
	ModuleFT232RL( const int &dev_id = 0 ) : device_id_(dev_id)
    {
		bool buffer = getDeviceInfo();
		if (!buffer)
			throw;
		ftStatus = FT_Open( device_id_, &ftHandle );
		buffer = ftStatusException(0);
	}

	~ModuleFT232RL()
    {
		FT_Close(ftHandle);
	}

	bool setBaudRate( const int &br = 9600 )
    {
	    ftStatus = FT_SetBaudRate(ftHandle, br);
		bool buffer = ftStatusException(1);
		bauderate = (buffer) ? br : bauderate;
		return buffer;
	}
    
    /** @brief setCharacteristics - задание характеристик передачи
     *  @param wordlenght - количество бит на слово (фрейм) - тут должно быть значение FT_BITS_8 или FT_BITS_7
     *  @param stopbit - количество стоп-бит - должно быть FT_STOP_BITS_1 или FT_STOP_BITS_2
     *  @param parity - четность - должно быть FT_PARITY_NONE, FT_PARITY_ODD, FT_PARITY_EVEN, \
     *                  FT_PARITY_MARK или FT_PARITY SPACE
     * */
    bool setCharacteristics(
        const UCHAR &wordlenght = FT_BITS_8,
        const UCHAR &stopbit = FT_STOP_BITS_1,
        const UCHAR &parity = FT_PARITY_NONE
    )
    {
        ftStatus = FT_SetDataCharacteristics( ftHandle, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE );
        return ftStatusException(10);
    }

	/* @brief функция отправляет данные на устройство. В примере использования FT_Read были еще статусы
     *        какие-то и задержки, в общем они не работали
	 * @param df - массив, который отправляем
	 * @param size - размеры отправляемого массива
	 * */
	bool writeData8( uint8_t *df, const size_t &size ) { return writeData<uint8_t>( df, size ); }
    bool writeData16( uint16_t *df, const size_t &size ) { return writeData<uint16_t>( df, size ); }
    bool writeData32( uint32_t *df, const size_t &size ) { return writeData<uint32_t>( df, size ); }

	/* @brief функция читает данные с устройства. В примере использования FT_Read были еще статусы
     *        какие-то и задержки, в общем они не работали
	 * @param df - массив, который считаем
	 * @param size - размеры считываемого массива
	 * */
	bool readData8(uint8_t *df, const size_t &size) { return readData<uint8_t>( df, size ); }
	bool readData16( uint16_t *df, const size_t &size ) { return readData<uint16_t>( df, size ); }
	bool readData32( uint32_t *df, const size_t &size ) { return readData<uint32_t>( df, size ); }

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
