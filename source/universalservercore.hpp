#ifndef UNIVERSALSERVER_CORE_UNIT_HPP_
#define UNIVERSALSERVER_CORE_UNIT_HPP_

#include <string>
/*
 * @class код для сервера выходит одинаковым во много, для того, чтобы не писать и переписывать например
 *        инициализацию по много раз проще написать ядро указатель на который буду передавать в сервер
 * */
class UniversalServerCore
{
private:

public:
    /*
     * @brief чтобы не забыли
     * */
    virtual ~UniversalServerCore() = default;

    /*
     * @brief так как решение универсальное, то в некоторых сценариях возможно нужна будет инициализация
     * */
    virtual void Init() = 0;

    /*
     * @brief метод для того чтобы вызвать при подтверждении приема сообщения
     * @param 1 - сокет
     * @param 2 - имя пользователя ( возможно использовании во внутренней логики )
     * @param 3 - сообщение, это закодированная строка, в ней может быть все что угодно.
     * */
    virtual void operator()( const int, const std::string&, const std::string& ) = 0;

    /*
     * @brief запуск каго-то внутреннего действия
     * */
    virtual void Launch() = 0;

    /*
     * @brief остановка какого-то внутреннего действия
     * */
    virtual void Stop() = 0;
};

#endif  // UNIVERSALSERVER_CORE_UNIT_HPP_
