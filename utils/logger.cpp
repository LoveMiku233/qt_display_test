#include "logger.h"
#include <QTextStream>
#include <QDir>
#include <QFileInfo>

Logger& Logger::instance()
{
    static Logger logger;
    return logger;
}

Logger::~Logger()
{
    close();
}

void Logger::init(const QString& logFilePath, LogLevel minLevel)
{
    QMutexLocker locker(&mutex_);
    
    if (initialized_) {
        return;
    }
    
    minLevel_ = minLevel;
    
    if (!logFilePath.isEmpty()) {
        // Ensure parent directory exists
        const QString dirPath = QFileInfo(logFilePath).absolutePath();
        QDir().mkpath(dirPath);
        
        logFile_.setFileName(logFilePath);
        if (logFile_.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            fileEnabled_ = true;
            qInfo().noquote() << "[Logger] Log file opened:" << logFilePath;
        } else {
            qWarning().noquote() << "[Logger] Failed to open log file:" << logFilePath 
                                  << "Error:" << logFile_.errorString();
        }
    }
    
    initialized_ = true;
    qInfo().noquote() << "[Logger] Initialized with level:" << levelToString(minLevel_);
}

void Logger::setMinLevel(LogLevel level)
{
    QMutexLocker locker(&mutex_);
    minLevel_ = level;
}

LogLevel Logger::minLevel() const
{
    return minLevel_;
}

QString Logger::levelToString(LogLevel level) const
{
    switch (level) {
        case LogLevel::Debug:    return "DEBUG";
        case LogLevel::Info:     return "INFO";
        case LogLevel::Warning:  return "WARN";
        case LogLevel::Error:    return "ERROR";
        case LogLevel::Critical: return "CRIT";
    }
    return "UNKNOWN";
}

QString Logger::formatMessage(LogLevel level, const QString& source, const QString& message) const
{
    const QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    return QString("[%1] [%2] [%3] %4")
        .arg(timestamp)
        .arg(levelToString(level), -5)
        .arg(source)
        .arg(message);
}

void Logger::log(LogLevel level, const QString& source, const QString& message)
{
    if (level < minLevel_) {
        return;
    }
    
    const QString formatted = formatMessage(level, source, message);
    
    // Output to console using appropriate Qt log function
    switch (level) {
        case LogLevel::Debug:
            qDebug().noquote() << formatted;
            break;
        case LogLevel::Info:
            qInfo().noquote() << formatted;
            break;
        case LogLevel::Warning:
            qWarning().noquote() << formatted;
            break;
        case LogLevel::Error:
        case LogLevel::Critical:
            qCritical().noquote() << formatted;
            break;
    }
    
    // Write to file if enabled
    if (fileEnabled_) {
        QMutexLocker locker(&mutex_);
        QTextStream stream(&logFile_);
        stream << formatted << "\n";
        stream.flush();
    }
}

void Logger::debug(const QString& source, const QString& message)
{
    log(LogLevel::Debug, source, message);
}

void Logger::info(const QString& source, const QString& message)
{
    log(LogLevel::Info, source, message);
}

void Logger::warning(const QString& source, const QString& message)
{
    log(LogLevel::Warning, source, message);
}

void Logger::error(const QString& source, const QString& message)
{
    log(LogLevel::Error, source, message);
}

void Logger::critical(const QString& source, const QString& message)
{
    log(LogLevel::Critical, source, message);
}

void Logger::flush()
{
    if (fileEnabled_) {
        QMutexLocker locker(&mutex_);
        logFile_.flush();
    }
}

void Logger::close()
{
    QMutexLocker locker(&mutex_);
    if (fileEnabled_ && logFile_.isOpen()) {
        logFile_.close();
        fileEnabled_ = false;
    }
    initialized_ = false;
}
