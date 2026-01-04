#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include <QObject>

class JsonRpcClient;

/**
 * @brief Application context singleton for managing global services
 * 
 * Provides centralized access to shared services like the RPC client,
 * with proper initialization status tracking and error handling.
 */
class AppContext : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Get the singleton instance
     */
    static AppContext& instance();

    /**
     * @brief Initialize the application context
     * @param parentForServices Parent object for service ownership
     */
    void init(QObject* parentForServices = nullptr);

    /**
     * @brief Check if context is initialized
     */
    bool isInitialized() const;
    
    /**
     * @brief Get the RPC client
     * @return Pointer to the RPC client (may be nullptr if not initialized)
     */
    JsonRpcClient* rpc() const { return rpc_; }

private:
    AppContext() = default;

    JsonRpcClient* rpc_ = nullptr;
    bool initialized_ = false;
};

#endif // APP_CONTEXT_H
