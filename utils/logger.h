#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QMutex>
#include <QDateTime>
#include <QDebug>

/**
 * @brief Log levels for the logging system
 */
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

/**
 * @brief A thread-safe logging utility class for core and app components
 * 
 * This logger supports multiple log levels, file output, and provides
 * contextual information (timestamp, source, level) in all log messages.
 */
class Logger
{
public:
    static Logger& instance();
    
    /**
     * @brief Initialize the logger with optional file output
     * @param logFilePath Path to the log file (empty for console only)
     * @param minLevel Minimum log level to output
     */
    void init(const QString& logFilePath = QString(), 
              LogLevel minLevel = LogLevel::Debug);
    
    /**
     * @brief Set the minimum log level
     * @param level Minimum level to output
     */
    void setMinLevel(LogLevel level);
    
    /**
     * @brief Get the current minimum log level
     */
    LogLevel minLevel() const;
    
    /**
     * @brief Log a message at the specified level
     * @param level Log level
     * @param source Source component name
     * @param message Log message
     */
    void log(LogLevel level, const QString& source, const QString& message);
    
    /**
     * @brief Convenience methods for different log levels
     */
    void debug(const QString& source, const QString& message);
    void info(const QString& source, const QString& message);
    void warning(const QString& source, const QString& message);
    void error(const QString& source, const QString& message);
    void critical(const QString& source, const QString& message);
    
    /**
     * @brief Flush log buffer to file
     */
    void flush();
    
    /**
     * @brief Close the log file
     */
    void close();

private:
    Logger() = default;
    ~Logger();
    
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    QString levelToString(LogLevel level) const;
    QString formatMessage(LogLevel level, const QString& source, const QString& message) const;
    
    QFile logFile_;
    QMutex mutex_;
    LogLevel minLevel_ = LogLevel::Debug;
    bool initialized_ = false;
    bool fileEnabled_ = false;
};

// Convenience macros for logging
#define LOG_DEBUG(source, msg) Logger::instance().debug(source, msg)
#define LOG_INFO(source, msg) Logger::instance().info(source, msg)
#define LOG_WARNING(source, msg) Logger::instance().warning(source, msg)
#define LOG_ERROR(source, msg) Logger::instance().error(source, msg)
#define LOG_CRITICAL(source, msg) Logger::instance().critical(source, msg)

#endif // LOGGER_H
