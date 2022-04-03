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

#include "tox/tox_net.h"
#include "tox/tox_logger.h"

#include "usbrelay/usb_relay.h"

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

//using namespace sql;

QUuidEx Application::_applId;
volatile bool Application::_stop = false;
std::atomic_int Application::_exitCode = {0};

Application::Application(int& argc, char** argv)
    : QCoreApplication(argc, argv)
{
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

//    readConfParams();
//    loadSettings();
//    netInterfacesUpdate();

    return true;
}

void Application::deinit()
{
//    if (!_eventLogId.isNull())
//    {
//        db::postgres::Driver::Ptr dbcon = pgpool().connect();
//        QSqlQuery q {dbcon->createResult()};

//        QDateTime humanLost = QDateTime::currentDateTime();
//        sql::exec(q, "UPDATE EVENT_LOG SET HUMAN_LOST = $1 WHERE ID = $2",
//                     humanLost, _eventLogId);
//    }

//    _threadIds.lock([](const std::vector<pid_t>& tids) {
//        for (pid_t tid : tids)
//            pgpool().abortOperation(tid);
//    });
//    while (!_threadIds.empty())
//        usleep(100*1000);

//    { //Block for QMutexLocker
//        QMutexLocker locker {&_framesCacheLock}; (void) locker;
//        _framesCache.clear();
//    }

//    _videoSaver.store(nullptr);
//    for (video::Saver2* saver : _videoSavers)
//        if (!saver->stop(30*1000 /*30 сек*/))
//        {
//            log_info_m << "Stop thread 'VideoSaver'. Timeout expired"
//                          ", thread will be terminated";
//            saver->terminate();
//        }
}

//void Application::sendSettings()
//{
//    Message::Ptr m = createMessage(_settings);
//    detect::siz().message(m);
//}

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

//void Application::socketConnected(pproto::SocketDescriptor socketDescript)
//{
//    Message::Ptr m = createMessage(_settings);
//    m->appendDestinationSocket(socketDescript);
//    tcp::listener().send(m);

//    m = createMessage(_usbRelayStatus);
//    m->appendDestinationSocket(socketDescript);
//    tcp::listener().send(m);

//    ++_terminalConnectCount;
//}

//void Application::socketDisconnected(pproto::SocketDescriptor /*socketDescript*/)
//{
//    if (--_terminalConnectCount < 0)
//        _terminalConnectCount = 0;

//    if (_terminalConnectCount == 0)
//    {
//        QMutexLocker locker {&_videoEventAnswersLock}; (void) locker;
//        _videoEventAnswers.clear();
//    }
//}

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

//void Application::readConfParams()
//{
//    config::base().getValue("show_debug_time", _showDebugTime);
//}

//void Application::netInterfacesUpdate()
//{
//    if (_netInterfacesTimer.elapsed() > 15*1000 /*15 сек*/)
//    {
//        network::Interface::List nl = network::getInterfaces();
//        _netInterfaces.swap(nl);
//    }
//}

void Application::loadSettings()
{
//    // Закладка "Детектирование"
//    _settings.detection.percentThreshold = QPair{1, 70};
//    config::state().getValue("settings.detection.percent_threshold",
//                             _settings.detection.percentThreshold);

//    _settings.detection.saturateTime = QPair{2, 2.0};
//    config::state().getValue("settings.detection.saturate_time",
//                             _settings.detection.saturateTime);

//    _settings.detection.protectExclude.clear();

//    bool value = false;
//    config::state().getValue("settings.detection.protect_exclude.helmet", value);
//    if (value)
//        _settings.detection.protectExclude.insert(Protector::Helmet);

//    value = false;
//    config::state().getValue("settings.detection.protect_exclude.vest", value);
//    if (value)
//        _settings.detection.protectExclude.insert(Protector::Vest);

//    value = false;
//    config::state().getValue("settings.detection.protect_exclude.gloves", value);
//    if (value)
//        _settings.detection.protectExclude.insert(Protector::Gloves);

//    value = false;
//    config::state().getValue("settings.detection.protect_exclude.boots", value);
//    if (value)
//        _settings.detection.protectExclude.insert(Protector::Boots);

//    _settings.detection.glovesPair = true;
//    config::state().getValue("settings.detection.gloves_pair",
//                             _settings.detection.glovesPair);

//    _settings.detection.bootsPair = true;
//    config::state().getValue("settings.detection.boots_pair",
//                             _settings.detection.bootsPair);

//    // Закладка "Журнал"
//    _settings.journal.keepHistory = QPair{1, 10};
//    config::state().getValue("settings.journal.keep_history",
//                             _settings.journal.keepHistory);

//    _settings.journal.saveVideo = true;
//    config::state().getValue("settings.journal.save_video",
//                             _settings.journal.saveVideo);
}

void Application::saveSettings()
{
//    // Закладка "Детектирование"
//    config::state().setValue("settings.detection.percent_threshold",
//                             _settings.detection.percentThreshold);
//    config::state().setValue("settings.detection.saturate_time",
//                             _settings.detection.saturateTime);

//    config::state().setValue("settings.detection.protect_exclude.helmet",
//                             _settings.detection.protectExclude.contains(Protector::Helmet));
//    config::state().setValue("settings.detection.protect_exclude.vest",
//                             _settings.detection.protectExclude.contains(Protector::Vest));
//    config::state().setValue("settings.detection.protect_exclude.gloves",
//                             _settings.detection.protectExclude.contains(Protector::Gloves));
//    config::state().setValue("settings.detection.protect_exclude.boots",
//                             _settings.detection.protectExclude.contains(Protector::Boots));

//    config::state().setValue("settings.detection.gloves_pair",
//                             _settings.detection.glovesPair);
//    config::state().setValue("settings.detection.boots_pair",
//                             _settings.detection.bootsPair);

//    // Закладка "Журнал"
//    config::state().setValue("settings.journal.keep_history",
//                             _settings.journal.keepHistory);
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
