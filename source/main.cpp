#include <iostream>

#include "ModuleFT232RL.hpp"

int main()
{
#if 0
    FT_HANDLE ftHandle;
    FT_STATUS ftStatus;
    DWORD RxBytes;
    char *buffer;
    DWORD BytesReceived;

    // Открытие устройства
    ftStatus = FT_Open(0, &ftHandle);
    if (ftStatus != FT_OK) {
        printf("Ошибка открытия устройства\n");
        return 1;
    }

    // Настройка скорости передачи данных
    ftStatus = FT_SetBaudRate(ftHandle, 9600);
    if (ftStatus != FT_OK) {
        printf("Ошибка настройки скорости передачи данных\n");
        FT_Close(ftHandle);
        return 1;
    }

    // Проверка наличия данных
    ftStatus = FT_GetQueueStatus(ftHandle, &RxBytes);
    if (ftStatus != FT_OK) {
        printf("Ошибка проверки очереди\n");
        FT_Close(ftHandle);
        return 1;
    }

    if (RxBytes > 0) {
        buffer = (char *)malloc(RxBytes);
        if (buffer == NULL) {
            printf("Ошибка выделения памяти\n");
            FT_Close(ftHandle);
            return 1;
        }

        // Чтение данных
        ftStatus = FT_Read(ftHandle, buffer, RxBytes, &BytesReceived);
        if (ftStatus != FT_OK) {
            printf("Ошибка чтения данных\n");
            free(buffer);
            FT_Close(ftHandle);
            return 1;
        }

        // Обработка данных
        printf("Получено %u байт: %s\n", BytesReceived, buffer);

        free(buffer);
    } else {
        printf("Данных нет\n");
    }

    // Закрытие устройства
    FT_Close(ftHandle);
#endif
    ModuleFT232RL module_;
    std::cout << module_ << std::endl;
    std::vector<uint32_t> frame = { 1, 2, 4, 5, 6, 7, 8 };
    module_.writeData32( frame );
    std::vector<uint32_t> retFrame( frame.size(), 0 );
    module_.readData32( retFrame );
    for( const auto it : retFrame )
        std::cout << it << ' ';
    std::cout << std::endl;
    return 0;
}
