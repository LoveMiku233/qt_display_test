#include "app_context.h"
#include "rpc/json_rpc_client.h"

AppContext& AppContext::instance()
{
    static AppContext ctx;
    return ctx;
}

// will not be init here!!!
void AppContext::init(QObject* parentForServices)
{
    if (rpc_) return;
    rpc_ = new JsonRpcClient(parentForServices);

}
