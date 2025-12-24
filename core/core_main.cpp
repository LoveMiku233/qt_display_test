// core/core_main.cpp name=core/core_main.cpp
#include <QCoreApplication>
#include <QHostAddress>
#include <QDebug>

#include "rpc/json_rpc_server.h"
#include "rpc/json_rpc_dispatcher.h"
#include "rpc/rpc_registry.h"

#include "core/core_context.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    CoreContext ctx;
    ctx.init();

    JsonRpcDispatcher dispatcher;
    RpcRegistry reg(&ctx, &dispatcher);
    reg.registerAll();

    JsonRpcServer server(&dispatcher);
    if (!server.listen(QHostAddress::Any, ctx.rpcPort)) {
        qCritical() << "listen failed:" << server.errorString();
        return 1;
    }

    qInfo() << "core_main JSON-RPC listening on" << ctx.rpcPort;
    return app.exec();
}
