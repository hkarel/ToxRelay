#pragma once

#include "toxcore/tox.h"
#include <QtCore>

/**
  Используется для безопасного вызова tox-функций из разных потоков
*/
struct ToxGlobalLock
{
    ToxGlobalLock();
    ~ToxGlobalLock();
    static QMutex mutex;
};

// Возвращают имя друга
QString getToxFriendName(Tox* tox, uint32_t friendNumber);
QString getToxFriendName(Tox* tox, const QByteArray& publicKey);

// Возвращают статус-сообщение
QString getToxFriendStatusMsg(Tox* tox, uint32_t friendNumber);

// Возвращает PublicKey друга
QByteArray getToxFriendKey(Tox* tox, uint32_t friendNumber);

// Возвращает номер друга
uint32_t getToxFriendNum(Tox* tox, const QByteArray& publicKey);

// Возвращает свой PublicKey
QByteArray getToxSelfPublicKey(Tox* tox);

// Возвращает статус подключения друга
TOX_CONNECTION getFriendConnectStatus(Tox* tox, uint32_t friendNumber);

//// Отправляем сообщение Message через tox-механизм пользовательских сообщений
//bool sendToxLosslessMessage(Tox* tox, uint32_t friendNumber,
//                            const communication::Message::Ptr&);

//// Читает сообщение Message из tox-механизма пользовательских сообщений
//const communication::Message::Ptr readToxMessage(Tox* tox, uint32_t friendNumber,
//                                                 const uint8_t* data, size_t length);

char* toString(TOX_CONNECTION);
