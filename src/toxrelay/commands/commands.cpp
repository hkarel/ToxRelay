#include "commands.h"
#include "pproto/commands/pool.h"

namespace pproto {
namespace command {

#define REGISTRY_COMMAND_SINGLPROC(COMMAND, UUID) \
    const QUuidEx COMMAND = command::Pool::Registry{UUID, #COMMAND, false};

#define REGISTRY_COMMAND_MULTIPROC(COMMAND, UUID) \
    const QUuidEx COMMAND = command::Pool::Registry{UUID, #COMMAND, true};

REGISTRY_COMMAND_SINGLPROC(ToxMessage,       "839f2186-cb3f-4f15-9edc-caf69bdb3e49")
REGISTRY_COMMAND_SINGLPROC(FriendRequest,    "85ebe56d-68ec-4d5e-9dbf-11b3a66d0ea3")
REGISTRY_COMMAND_SINGLPROC(RemoveFriend,     "e6935f6b-3064-4bdb-91e8-37f6b83fc4b6")

#undef REGISTRY_COMMAND_SINGLPROC
#undef REGISTRY_COMMAND_MULTIPROC

} // namespace command

namespace data {

bserial::RawVector ToxMessage::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << friendNumber;
    stream << text;
    B_SERIALIZE_RETURN
}

void ToxMessage::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> friendNumber;
    stream >> text;
    B_DESERIALIZE_END
}

bserial::RawVector FriendRequest::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << friendNumber;
    stream << publicKey;
    B_SERIALIZE_RETURN
}

void FriendRequest::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> friendNumber;
    stream >> publicKey;
    B_DESERIALIZE_END
}

bserial::RawVector RemoveFriend::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << publicKey;
    stream << name;
    B_SERIALIZE_RETURN
}

void RemoveFriend::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> publicKey;
    stream >> name;
    B_DESERIALIZE_END
}

} // namespace data
} // namespace pproto
