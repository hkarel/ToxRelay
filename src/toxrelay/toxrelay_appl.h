#pragma once

#include "shared/simple_timer.h"
#include "shared/steady_timer.h"

#include "pproto/func_invoker.h"

#include <QtCore>
#include <QCoreApplication>
#include <atomic>
#include <chrono>

using namespace std;
using namespace pproto;

class Application : public QCoreApplication
{
public:
    Application(int& argc, char** argv);

    bool init();
    void deinit();

//    void sendSettings();

    static void stop() {_stop = true;}
    static bool isStopped() {return _stop;}

signals:
    // Используется для отправки сообщения в пределах программы
    void internalMessage(const pproto::Message::Ptr&);

public slots:
    void stop(int exitCode);
    void message(const pproto::Message::Ptr&);

//    void socketConnected(pproto::SocketDescriptor);
//    void socketDisconnected(pproto::SocketDescriptor);

    void usbRelayAttached();
    void usbRelayDetached();
    void usbRelayChanged(int relayNumber);

//    void clearHistory();

private:
    Q_OBJECT
    void timerEvent(QTimerEvent* event) override;

    void command_FriendRequest(const Message::Ptr&);
    void command_ToxMessage(const Message::Ptr&);
//    void command_RoiGeometry(const Message::Ptr&);
//    void command_RoiGeometrySave(const Message::Ptr&);
//    void command_ProtectorDetectStatus(const Message::Ptr&);
//    void command_UsbRelayTurnOn(const Message::Ptr&);
//    void command_Settings(const Message::Ptr&);
//    void command_AdmimMode(const Message::Ptr&);
//    void command_SetAdmimPassw(const Message::Ptr&);
//    void command_EventLog(const Message::Ptr&);
//    void command_VideoEventGet(const Message::Ptr&);
//    void command_VideoEventChunk(const Message::Ptr&);

//    void readConfParams();
//    void netInterfacesUpdate();

    void loadSettings();
    void saveSettings();

private:
    static QUuidEx _applId;
    static volatile bool _stop;
    static std::atomic_int _exitCode;

    int _stopTimerId = {-1};
//    int _clearHistoryTimerId = {-1};

    FunctionInvoker _funcInvoker;

//    network::Interface::List _netInterfaces;
//    simple_timer _netInterfacesTimer {0};

//    //QVector<infer::NmsProposal> _proposals;
//    //std::atomic_flag _proposalsLock = ATOMIC_FLAG_INIT;

//    detect::FrameAggregator::Ptr _frameAggr;
//    atomic_flag _frameAggrLock = ATOMIC_FLAG_INIT;

//    // Счетчик для обновления рамок
//    const int _frameDrawLimit = {10};
//    atomic_int _frameDrawCounter = {_frameDrawLimit};

//    // Настройки программы
//    data::Settings _settings;

//    // Счетчик подключенных терминалов
//    volatile int _terminalConnectCount = {0};

    // Статус usb-реле
    //data::UsbRelayStatus _usbRelayStatus;

//    // Идентификатор события логирования
//    QUuidEx _eventLogId;

//    // Информация о статусе детектирования
//    data::ProtectorDetectStatus _protectorDetectStatus;

//    // Список идентификаторов потоков для выполнения sql-запросов
//    trd::ThreadIdList _threadIds;

//    // Отображать отладочное время на кадре
//    bool _showDebugTime = {false};

//    video::Frame::List _framesCache;
//    steady_timer _framesCacheTimer;
//    QMutex _framesCacheLock;

//    lst::List<video::Saver2> _videoSavers;
//    atomic<video::Saver2*> _videoSaver = {nullptr};

//    QMap<QUuidEx /*Message Id*/, bool /*Message answer received*/> _videoEventAnswers;
//    QMutex _videoEventAnswersLock;

//    atomic_bool _clearHistoryFlag = {false};
};
