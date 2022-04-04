#pragma once

#include "shared/simple_timer.h"
#include "shared/steady_timer.h"

#include "pproto/func_invoker.h"

#include <QtCore>
#include <QCoreApplication>
#include <QTcpServer>
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

    static void stop() {_stop = true;}
    static bool isStopped() {return _stop;}

signals:
    // Используется для отправки сообщения в пределах программы
    void internalMessage(const pproto::Message::Ptr&);

public slots:
    void stop(int exitCode);
    void message(const pproto::Message::Ptr&);

    void usbRelayAttached();
    void usbRelayDetached();
    void usbRelayChanged(int relayNumber);

private slots:
    void httpNewConnection();
    void httpReadyRead();
    void httpDisconnected();
    void httpSocketError(QAbstractSocket::SocketError);

private:
    Q_OBJECT
    void timerEvent(QTimerEvent* event) override;

    void command_FriendRequest(const Message::Ptr&);
    void command_ToxMessage(const Message::Ptr&);

    void loadSettings();
    void saveSettings();

private:
    static QUuidEx _applId;
    static volatile bool _stop;
    static std::atomic_int _exitCode;

    int _stopTimerId = {-1};

    QTcpServer* _httpConnector;
    FunctionInvoker _funcInvoker;
};
