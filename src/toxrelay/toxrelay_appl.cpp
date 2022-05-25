#include "toxrelay_appl.h"

#include "shared/spin_locker.h"
#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/config/appl_conf.h"
#include "shared/qt/logger_operators.h"
#include "shared/qt/version_number.h"

#include "pproto/commands/base.h"
#include "pproto/commands/pool.h"
#include "pproto/serialize/functions.h"

#include "commands/commands.h"

#include "toxcore/tox.h"
#include "toxfunc/tox_func.h"
#include "toxfunc/tox_logger.h"
#include "toxfunc/tox_error.h"
#include "toxfunc/pproto_error.h"

#include "tox/tox_net.h"

#include "qrcodegen.hpp"
#include "usbrelay/usb_relay.h"

#include <QTcpSocket>
#include <unistd.h>
#include <sys/stat.h>
#include <string>
#include <thread>

#define log_error_m   alog::logger().error   (alog_line_location, "Application")
#define log_warn_m    alog::logger().warn    (alog_line_location, "Application")
#define log_info_m    alog::logger().info    (alog_line_location, "Application")
#define log_verbose_m alog::logger().verbose (alog_line_location, "Application")
#define log_debug_m   alog::logger().debug   (alog_line_location, "Application")
#define log_debug2_m  alog::logger().debug2  (alog_line_location, "Application")

#define KILL_TIMER(TIMER) {if (TIMER != -1) {killTimer(TIMER); TIMER = -1;}}

using namespace qrcodegen;

QUuidEx Application::_applId;
volatile bool Application::_stop = false;
std::atomic_int Application::_exitCode = {0};

Application::Application(int& argc, char** argv)
    : QCoreApplication(argc, argv)
{
    _httpConnector = new QTcpServer(this);
    chk_connect_a(_httpConnector, &QTcpServer::newConnection,
                  this, &Application::httpNewConnection)

    _stopTimerId = startTimer(1000);
//    _clearHistoryTimerId = startTimer(4*60*60*1000 /*4 часа*/);

//    chk_connect_a(&config::changeChecker(), &config::ChangeChecker::changed,
//                  this, &Application::readConfParams)

    #define FUNC_REGISTRATION(COMMAND) \
        _funcInvoker.registration(command:: COMMAND, &Application::command_##COMMAND, this);

    FUNC_REGISTRATION(ToxMessage)
    FUNC_REGISTRATION(FriendRequest)
    FUNC_REGISTRATION(FriendDisconnect)

    #undef FUNC_REGISTRATION

    using namespace std::placeholders;

    _commandFunc["/"]        = std::bind(&Application::rootCmd,    this, _1);
    _commandFunc["/relay"]   = std::bind(&Application::realayCmd,  this, _1);
    _commandFunc["/friends"] = std::bind(&Application::friendsCmd, this, _1);
}

bool Application::init()
{
//    if (config::state().getValue("application.id", _applId, false))
//    {
//        log_verbose_m << "Read ApplId: " << _applId;
//    }
//    else
//    {
//        _applId = QUuidEx::createUuid();
//        config::state().setValue("application.id", _applId);
//        config::state().saveFile();
//        log_verbose_m << "Generated new ApplId: " << _applId;
//    }
//    loadSettings();

    if (!_httpConnector->listen(QHostAddress::Any, 8088))
    {
        log_error_m << "Start listener of http connection is failed";
        return false;
    }

    return true;
}

void Application::deinit()
{
    _httpConnector->close();
}

void Application::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == _stopTimerId)
    {
        if (_stop)
        {
            KILL_TIMER(_stopTimerId)
//            KILL_TIMER(_clearHistoryTimerId)

            exit(_exitCode);
            return;
        }

//        if (_usbRelayStatus.attached
//            && _usbRelayStatus.turnTimer >= 0)
//        {
//            if (--_usbRelayStatus.turnTimer == -1)
//                usb::relay().toggle(1, false);

//            Message::Ptr m = createMessage(_usbRelayStatus, {Message::Type::Event});
//            tcp::listener().send(m);
//        }

//        // Удаляем остановленные потоки
//        for (int i = 0; i < _videoSavers.count(); ++i)
//            if (!_videoSavers[i].isRunning())
//                _videoSavers.remove(i--);
    }
//    else if (event->timerId() == _clearHistoryTimerId)
//    {
//        clearHistory();
    //    }
}

void Application::stop(int exitCode)
{
    _exitCode = exitCode;
    stop();
}

void Application::message(const pproto::Message::Ptr& message)
{
    // Не обрабатываем сообщения если приложение получило команду на остановку
    if (_stop)
        return;

    if (message->processed())
        return;

    if (lst::FindResult fr = _funcInvoker.findCommand(message->command()))
    {
        if (command::pool().commandIsSinglproc(message->command()))
            message->markAsProcessed();
        _funcInvoker.call(message, fr);
    }
}

void Application::usbRelayAttached()
{
//    _usbRelayStatus.attached = true;
//    _usbRelayStatus.turnTimer = -1;

//    Message::Ptr m = createMessage(_usbRelayStatus, {Message::Type::Event});
//    tcp::listener().send(m);
}

void Application::usbRelayDetached()
{
//    _usbRelayStatus.attached = false;
//    _usbRelayStatus.turnTimer = -1;

//    Message::Ptr m = createMessage(_usbRelayStatus, {Message::Type::Event});
//    tcp::listener().send(m);
}

void Application::usbRelayChanged(int relayNumber)
{
//    QVector<int> states = usb::relay().states();
//    bool turnOn = states[relayNumber - 1];
//    detect::siz().setRelayTurnOn(turnOn);

////    _usbRelayStatus.attached = true;
////    _usbRelayStatus.turnTimer = 10;

////    Message::Ptr m = createMessage(_usbRelayStatus, {Message::Type::Event});
////    tcp::listener().send(m);
}

void Application::httpNewConnection()
{
    QTcpSocket* client = _httpConnector->nextPendingConnection();
    chk_connect_a(client, &QTcpSocket::disconnected,
                  this,   &Application::httpDisconnected)
    chk_connect_a(client, &QTcpSocket::readyRead,
                  this,   &Application::httpReadyRead)
    chk_connect_a(client, qOverload<QAbstractSocket::SocketError>(&QTcpSocket::error),
                  this,   &Application::httpSocketError)
}

std::string toSvgString(const QrCode& qr, int border)
{
//    if (border < 0)
//        throw std::domain_error("Border must be non-negative");
//    if (border > INT_MAX / 2 || border * 2 > INT_MAX - qr.getSize())
//        throw std::overflow_error("Border too large");

    std::ostringstream sb;
    sb << R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1" )";
    sb << R"(viewBox="0 0 45 45" stroke="none" width="200" height="200">)";
    sb << R"(<path d=")";
    for (int y = 0; y < qr.getSize(); ++y)
        for (int x = 0; x < qr.getSize(); ++x)
            if (qr.getModule(x, y))
            {
                if (x != 0 || y != 0)
                    sb << " ";
                sb << "M" << (x + border) << "," << (y + border) << "h1v1h-1z";
            }

    sb << R"(" fill="#000000"/>)";
    sb << R"(</svg>)";
    return sb.str();
}

void Application::httpReadyRead()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());

    QByteArray ba = client->readAll();
    log_debug_m << "Received from http client: " << ba;

    if (!ba.startsWith("GET / "))
    {
        client->write("HTTP/1.0 404 Not found\n\n");
        client->disconnectFromHost();
        return;
    }

    QString s =
        R"(HTTP/1.0 200 Ok
           Content-Type: text/html; charset="utf-8"

           <html>
           <body style="background: white;">
           <div>Tox ID: %1</div>
           %2
           </body>
           </html>
        )";

    QByteArray toxId;
    { //Block for ToxGlobalLock
        ToxGlobalLock toxGlobalLock; (void) toxGlobalLock;
        toxId.resize(TOX_ADDRESS_SIZE);
        tox_self_get_address(toxNet().tox(), (uint8_t*)toxId.constData());
        toxId = toxId.toHex().toUpper();
    }

    std::string qrsvg;
    try
    {
        //QByteArray toxLink = "tox:" + toxId;
        QrCode qr = QrCode::encodeText(toxId.constData(), QrCode::Ecc::LOW);
        qrsvg = toSvgString(qr, 4);
    }
    catch (const std::exception& e)
    {
        qrsvg.clear();
        log_error_m << "Failed create QrCode. Detail: " << e.what();
    }
    catch (...)
    {
        qrsvg.clear();
        log_error_m << "Failed create QrCode. Unknown error";
    }

    s = s.arg(toxId.constData()).arg(qrsvg.c_str());
    log_debug_m << "Sent to http client: " << s;

//    QFile f {"/tmp/1.svg"};
//    f.open(QIODevice::WriteOnly);
//    f.write(qrsvg.c_str());
//    f.close();

    client->write(s.toUtf8());
    client->disconnectFromHost();
}

void Application::httpDisconnected()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    client->deleteLater();
}

void Application::httpSocketError(QAbstractSocket::SocketError error)
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    log_error_m << "Socket error: " << client->errorString();
}

void Application::loadSettings()
{
//    _settings.journal.saveVideo = true;
//    config::state().getValue("settings.journal.save_video",
//                             _settings.journal.saveVideo);
}

void Application::saveSettings()
{
//    config::state().setValue("settings.journal.save_video",
//                             _settings.journal.saveVideo);

//    config::state().saveFile();
}

void Application::command_FriendRequest(const Message::Ptr& message)
{
    data::FriendRequest friendRequest;
    readFromMessage(message, friendRequest);


}

void Application::command_ToxMessage(const Message::Ptr& message)
{
    data::ToxMessage toxMessage;
    readFromMessage(message, toxMessage);

    log_verbose_m << "Tox message recv: " << toxMessage.text
                  << ". " << ToxFriendLog(toxNet().tox(), toxMessage.friendNumber);

    if (_commandPath[toxMessage.friendNumber].isEmpty())
        _commandPath[toxMessage.friendNumber] = "/";

    QString commandPath = _commandPath[toxMessage.friendNumber];
    _commandFunc[commandPath](message);
}

void Application::command_FriendDisconnect(const Message::Ptr& message)
{
    quint32 friendNumber = message->tag();
    _commandPath.remove(friendNumber);
}

bool Application::checkCommand(const QString& cmd, const QString& checkCmd,
                               QChar shortCmd)
{
    if ((shortCmd == QChar(0)) && (cmd.length() == 1))
        shortCmd = checkCmd[0];

    return ((cmd.length() == 1) && (cmd[0] == shortCmd)) || (cmd == checkCmd);
}

void Application::rootCmd(const Message::Ptr& message)
{
    data::ToxMessage toxMessage;
    readFromMessage(message, toxMessage);

    //bool outToLog = true;
    QString text, path;
    QString cmd = toxMessage.text.toLower().trimmed();

    if (checkCommand(cmd, "help"))
    {
        text =
        "help  \t[h] - this help;\n"
        "relay \t[r] - relay state;\n"
        "friends [f] - friends info";
    }
    else if (checkCommand(cmd, "relay"))
    {
        _commandPath[toxMessage.friendNumber] = "/relay";
    }
    else if (checkCommand(cmd, "friends"))
    {
        _commandPath[toxMessage.friendNumber] = "/friends";
    }
    else
        text = "Unknown command. Print 'help' for more info";

    Message::Ptr m;
    if (!text.isEmpty())
    {
        toxMessage.text = text;
        toxMessage.outToLog = false;
        m = createMessage(toxMessage);
        toxNet().message(m);
    }

    path = _commandPath[toxMessage.friendNumber] + " >";
    toxMessage.text = path;
    toxMessage.outToLog = false;
    m = createMessage(toxMessage);
    toxNet().message(m);
}

void Application::realayCmd(const Message::Ptr& message)
{
    static const QRegExp re {R"(^ *([0-8]) *(n|on|f|off))"};

    data::ToxMessage toxMessage;
    readFromMessage(message, toxMessage);

    bool outToLog = true;
    QString cmd, param, text, path;
    cmd = toxMessage.text.toLower();

    if (checkCommand(cmd, "help"))
    {
        text =
        "help  \t[h] - this help;\n"
        "info  \t[i] - relay info;\n"
        "state \t[s] - relay state;\n"
        "quit  \t[q] - to up level;\n"
        "To change relay state use command `X on/off` or `X n/f` where X the number of relay";
        outToLog = false;
    }
    else if (checkCommand(cmd, "info"))
    {
        if (usb::relay().isAttached())
        {
            text =
            "product: %1\n"
            "serial: %2\n"
            "count: %3\n"
            "states: %4";

            QString statesStr;
            QVector<int> states = usb::relay().states();
            for (int i = 0; i < states.count(); ++i)
            {
                statesStr += QString::number(i + 1);
                statesStr += (states[i]) ? " on; " : " off; ";
            }
            text = text.arg(usb::relay().product())
                       .arg(usb::relay().serial())
                       .arg(usb::relay().count())
                       .arg(statesStr);
            outToLog = false;
        }
        else
            text = "Error: USB relay not attached";
    }
    else if (checkCommand(cmd, "state"))
    {
        if (usb::relay().isAttached())
        {
            QString statesStr;
            QVector<int> states = usb::relay().states();
            for (int i = 0; i < states.count(); ++i)
            {
                statesStr += QString::number(i + 1);
                statesStr += (states[i]) ? " on; " : " off; ";
            }
            text = statesStr;
        }
        else
            text = "Error: USB relay not attached";
    }
    else if (re.indexIn(cmd) == 0)
    {
        if (usb::relay().isAttached())
        {
            QString p1 = re.cap(1);
            QString p2 = re.cap(2);
            int relayNumber = p1.toInt();
            if (relayNumber <= usb::relay().count())
            {
                if ((p2 == "on") || (p2 == "n"))
                {
                    usb::relay().toggle(relayNumber, true);
                }
                else if ((p2 == "off") || (p2 == "f"))
                {
                    usb::relay().toggle(relayNumber, false);
                }
                else
                    text = "Unknown command. Print 'help' for more info";
            }
            else
            {
                text = "Error: Failed toggle relay number %1. Number out of range [1..%2]";
                text = text.arg(relayNumber).arg(usb::relay().count());
            }
        }
        else
            text = "Error: USB relay not attached";
    }
    else if (re.indexIn(cmd) == 0)
    {
        if (usb::relay().isAttached())
        {
            int relayNumber = cmd.toInt();
            if (relayNumber <= usb::relay().count())
            {
                if ((param == "on") || (param == "o"))
                {
                    usb::relay().toggle(relayNumber, true);
                }
                else if ((param == "off") || (param == "f"))
                {
                    usb::relay().toggle(relayNumber, false);
                }
                else
                    text = "Unknown command. Print 'help' for more info";
            }
            else
            {
                text = "Error: Failed toggle relay number %1. Number out of range [1..%2]";
                text = text.arg(relayNumber).arg(usb::relay().count());
            }
        }
        else
            text = "Error: USB relay not attached";
    }

    else if (checkCommand(cmd, "quit"))
    {
        path = _commandPath[toxMessage.friendNumber];
        int index = path.lastIndexOf(QChar('/'));
        if (index > 0)
            path.resize(index);
        else
            path = "/";

        _commandPath[toxMessage.friendNumber] = path;
        outToLog = false;
    }
    else
        text = "Unknown command. Print 'help' for more info";

    Message::Ptr m;
    if (!text.isEmpty())
    {
        toxMessage.text = text;
        toxMessage.outToLog = outToLog;
        m = createMessage(toxMessage);
        toxNet().message(m);
    }

    path = _commandPath[toxMessage.friendNumber] + " >";
    toxMessage.text = path;
    toxMessage.outToLog = false;
    m = createMessage(toxMessage);
    toxNet().message(m);

}

void Application::friendsCmd(const Message::Ptr& message)
{
    data::ToxMessage toxMessage;
    readFromMessage(message, toxMessage);

    bool outToLog = true;
    QStringList lst = toxMessage.text.toLower().split(QChar(' '), QString::SkipEmptyParts);

    QString cmd, param, text, path;
    if (lst.count() >= 1)
        cmd = lst[0];

    if (lst.count() >= 2)
        param = lst[1];

    if (checkCommand(cmd, "help"))
    {
        text =
        "help \t[h] - this help;\n"
        "list \t[l] - friends list;\n"
        "add  \t[a] - add friend with PUBLIC_KEY;\n"
        "remove [r] - remove friend with PUBLIC_KEY;\n"
        "lock \t[k] - lock the adding new friends by request (set value on/off);\n"
        "quit \t[q] - to up level";
        outToLog = false;
    }
    else if (checkCommand(cmd, "list"))
    {
        ToxGlobalLock toxGlobalLock; (void) toxGlobalLock;

        // Выводим в лог информацию по подключенным друзьям
        if (uint32_t friendCount = tox_self_get_friend_list_size(toxNet().tox()))
        {
            QVector<uint32_t> fnumbers;
            fnumbers.resize(friendCount);
            tox_self_get_friend_list(toxNet().tox(), fnumbers.data());

            for (uint32_t i = 0; i < friendCount; ++i)
            {
                uint32_t friendNumber = fnumbers[i];
                QString friendName = getToxFriendName(toxNet().tox(), friendNumber);
                QByteArray friendPk = getToxFriendKey(toxNet().tox(), friendNumber);
                TOX_CONNECTION connectStatus = getFriendConnectStatus(toxNet().tox(), friendNumber);

                text += friendName + "\n";
                text += friendPk.toHex().toUpper() + "\n";
                text += QString("Connection: ") + toString(connectStatus) + "\n";
                text += "---\n";
            }
        }
        outToLog = false;
    }
    else if (checkCommand(cmd, "add"))
    {
        if (!param.isEmpty())
        {
            TOX_ERR_FRIEND_ADD err;
            data::MessageError msgerr;

            uint32_t friendNum;
            QByteArray publicKey = QByteArray::fromHex(param.toUtf8());
            { //Block for ToxGlobalLock
                ToxGlobalLock toxGlobalLock; (void) toxGlobalLock;
                friendNum = tox_friend_add_norequest(toxNet().tox(),
                                (uint8_t*)publicKey.constData(), &err);
                (void) friendNum;
            }
            if (!toxError(err, msgerr))
            {
                text = "Friend was successfully added";
                if (!toxNet().saveState())
                    text = "Error: " + error::save_tox_state.description;
            }
            else
                text = "Error: " + msgerr.description;
        }
        else
            text = "Friend public key not set. Use: add PUBLIC_KEY";
    }
    else if (checkCommand(cmd, "remove"))
    {
        if (!param.isEmpty())
        {
            QByteArray publicKey = QByteArray::fromHex(param.toUtf8());
            uint32_t friendNum = getToxFriendNum(toxNet().tox(), publicKey);
            if (friendNum != UINT32_MAX)
            {
                QString friendName = getToxFriendName(toxNet().tox(), friendNum);
                bool result;
                { //Block for ToxGlobalLock
                    ToxGlobalLock toxGlobalLock; (void) toxGlobalLock;
                    result = tox_friend_delete(toxNet().tox(), friendNum, 0);
                }
                if (result)
                {
                    text = "Friend '%1' was removed";
                    text = text .arg(friendName);
                    if (!toxNet().saveState())
                        text = "Error: " + error::save_tox_state.description;
                }
                else
                {
                    text = "Error: failed remove friend '%1'";
                    text = text .arg(friendName);
                }
            }
            else
                text = "Error: friend not found";
        }
        else
            text = "Friend public key not set. Use: remove PUBLIC_KEY";
    }
    else if (checkCommand(cmd, "lock", 'k'))
    {
        if (!param.isEmpty())
        {
            if (param == "on")
            {
                config::state().setValue("lock_add_friends", true);
                config::state().saveFile();
            }
            else if (param == "off")
            {
                config::state().setValue("lock_add_friends", false);
                config::state().saveFile();
            }
            else
            {
                text = "Error: failed set the value %1 to 'lock' param. Use: lock on/off";
                text = text.arg(param);
            }
        }
        else
        {
            bool val = false;
            config::state().getValue("lock_add_friends", val);
            text  = "lock ";
            text += (val) ? "on" : "off";
        }
    }
    else if (checkCommand(cmd, "quit"))
    {
        path = _commandPath[toxMessage.friendNumber];
        int index = path.lastIndexOf(QChar('/'));
        if (index > 0)
            path.resize(index);
        else
            path = "/";

        _commandPath[toxMessage.friendNumber] = path;
        outToLog = false;
    }
    else
        text = "Unknown command. Print 'help' for more info";

    Message::Ptr m;
    if (!text.isEmpty())
    {
        toxMessage.text = text;
        toxMessage.outToLog = outToLog;
        m = createMessage(toxMessage);
        toxNet().message(m);
    }

    path = _commandPath[toxMessage.friendNumber] + " >";
    toxMessage.text = path;
    toxMessage.outToLog = false;
    m = createMessage(toxMessage);
    toxNet().message(m);
}
