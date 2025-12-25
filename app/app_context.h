#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include <QObject>

class JsonRpcClient;

class AppContext : public QObject
{
    Q_OBJECT
public:
    static AppContext& instance();

    void init(QObject* parentForServices = nullptr);

    JsonRpcClient* rpc() const { return rpc_; }

private:
    AppContext() = default;

    JsonRpcClient* rpc_ = nullptr;
};

#endif // APP_CONTEXT_H
