/*****************************************************************************
  Требование надежности коммуникаций: однажды назначенный идентификатор коман-
  ды не должен более меняться.
*****************************************************************************/

#pragma once

#include "pproto/commands/base.h"

namespace pproto {
namespace command {

//----------------------------- Список команд --------------------------------

/**
  Команда используется для отправки tox-сообщения
*/
extern const QUuidEx ToxMessage;

/**
  Запрос от другого пользователя на добавление в друзья
*/
extern const QUuidEx FriendRequest;

/**
  Удалить друга
*/
extern const QUuidEx RemoveFriend;


} // namespace command

//---------------- Структуры данных используемые в сообщениях ----------------

namespace data {

struct ToxMessage : Data<&command::ToxMessage,
                          Message::Type::Command>
{
    quint32 friendNumber = {UINT32_MAX}; // Числовой идентификатор друга
    QString text;

    DECLARE_B_SERIALIZE_FUNC
};

struct FriendRequest : Data<&command::FriendRequest,
                             Message::Type::Command>
{
    quint32    friendNumber = {UINT32_MAX};
    QByteArray publicKey;

    DECLARE_B_SERIALIZE_FUNC
};

struct RemoveFriend : Data<&command::RemoveFriend,
                            Message::Type::Command>
{
    QByteArray publicKey;
    QString    name;

    DECLARE_B_SERIALIZE_FUNC
};


} // namespace data
} // namespace pproto
