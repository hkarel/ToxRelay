#include "toxrelay_appl.h"

#include "shared/spin_locker.h"
#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/config/appl_conf.h"
#include "shared/config/logger_conf.h"
#include "shared/qt/logger_operators.h"
#include "shared/qt/version_number.h"

#include "pproto/commands/base.h"
#include "pproto/commands/pool.h"
#include "pproto/serialize/functions.h"

#include "commands/commands.h"
#include "commands/error.h"

#include "toxcore/tox.h"
#include "tox/tox_net.h"
#include "tox/tox_func.h"
#include "tox/tox_logger.h"

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

    #undef FUNC_REGISTRATION
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

    //ba = s.toUtf8();
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

    log_debug_m << "Tox mesage: " << toxMessage.text
                << ". " << ToxFriendLog(toxNet().tox(), toxMessage.friendNumber);

    toxMessage.text = QString("Answer: ") + toxMessage.text;

    Message::Ptr m = createMessage(toxMessage);
    toxNet().message(m);
}
