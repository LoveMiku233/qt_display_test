#include "app_config.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QStandardPaths>
#include <QFileInfo>
#include <QCoreApplication>

/**
 * @brief 写入文本文件
 * @param path 文件路径
 * @param data 数据内容
 * @param err 错误信息输出
 * @return 是否成功
 */
static bool writeTextFile(const QString& path, const QByteArray& data, QString* err)
{
    // 确保父目录存在
    const QString dirPath = QFileInfo(path).absolutePath();
    QDir().mkpath(dirPath);
    
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (err) *err = QString("打开文件写入失败: %1").arg(f.errorString());
        return false;
    }
    if (f.write(data) != data.size()) {
        if (err) *err = QString("写入失败: %1").arg(f.errorString());
        return false;
    }
    return true;
}

/**
 * @brief 生成默认配置
 */
AppConfig AppConfig::makeDefault()
{
    AppConfig c;
    
    // 界面默认配置
    c.ui_.theme = "dark";
    c.ui_.language = "zh_CN";
    c.ui_.fontSize = 14;
    c.ui_.enableAnimations = true;
    c.ui_.animationDuration = 250;
    
    // RPC默认配置
    c.rpc_.serverHost = "127.0.0.1";
    c.rpc_.serverPort = 12345;
    c.rpc_.reconnectInterval = 5000;
    c.rpc_.autoReconnect = true;
    
    // 日志默认配置
    c.log_.logToConsole = true;
    c.log_.logToFile = false;
    c.log_.logFilePath = "logs/app.log";
    c.log_.logLevel = 0;
    
    return c;
}

/**
 * @brief 获取默认配置文件路径
 */
QString AppConfig::defaultConfigPath()
{
    // 使用应用程序目录以获得可靠的路径
    const QString appDir = QCoreApplication::applicationDirPath();
    return appDir + "/config/app_config.json";
}

/**
 * @brief 从JSON文件加载配置
 */
bool AppConfig::loadFromFile(const QString& path, QString* err)
{
    QFile f(path);
    if (!f.exists()) {
        if (err) *err = "配置文件不存在";
        return false;
    }
    if (!f.open(QIODevice::ReadOnly)) {
        if (err) *err = QString("打开文件失败: %1").arg(f.errorString());
        return false;
    }

    const auto doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isObject()) {
        if (err) *err = "无效的JSON根节点(非对象)";
        return false;
    }
    const QJsonObject root = doc.object();

    // 读取界面配置
    if (root.contains("ui") && root["ui"].isObject()) {
        const auto ui = root["ui"].toObject();
        if (ui.contains("theme")) ui_.theme = ui["theme"].toString(ui_.theme);
        if (ui.contains("language")) ui_.language = ui["language"].toString(ui_.language);
        if (ui.contains("fontSize")) ui_.fontSize = ui["fontSize"].toInt(ui_.fontSize);
        if (ui.contains("enableAnimations")) ui_.enableAnimations = ui["enableAnimations"].toBool(ui_.enableAnimations);
        if (ui.contains("animationDuration")) ui_.animationDuration = ui["animationDuration"].toInt(ui_.animationDuration);
    }

    // 读取RPC配置
    if (root.contains("rpc") && root["rpc"].isObject()) {
        const auto rpc = root["rpc"].toObject();
        if (rpc.contains("serverHost")) rpc_.serverHost = rpc["serverHost"].toString(rpc_.serverHost);
        if (rpc.contains("serverPort")) rpc_.serverPort = quint16(rpc["serverPort"].toInt(int(rpc_.serverPort)));
        if (rpc.contains("reconnectInterval")) rpc_.reconnectInterval = rpc["reconnectInterval"].toInt(rpc_.reconnectInterval);
        if (rpc.contains("autoReconnect")) rpc_.autoReconnect = rpc["autoReconnect"].toBool(rpc_.autoReconnect);
    }

    // 读取日志配置
    if (root.contains("log") && root["log"].isObject()) {
        const auto log = root["log"].toObject();
        if (log.contains("logToConsole")) log_.logToConsole = log["logToConsole"].toBool(log_.logToConsole);
        if (log.contains("logToFile")) log_.logToFile = log["logToFile"].toBool(log_.logToFile);
        if (log.contains("logFilePath")) log_.logFilePath = log["logFilePath"].toString(log_.logFilePath);
        if (log.contains("logLevel")) log_.logLevel = log["logLevel"].toInt(log_.logLevel);
    }

    return true;
}

/**
 * @brief 保存配置到JSON文件
 */
bool AppConfig::saveToFile(const QString& path, QString* err) const
{
    QJsonObject root;

    // 界面配置
    QJsonObject ui;
    ui["theme"] = ui_.theme;
    ui["language"] = ui_.language;
    ui["fontSize"] = ui_.fontSize;
    ui["enableAnimations"] = ui_.enableAnimations;
    ui["animationDuration"] = ui_.animationDuration;
    root["ui"] = ui;

    // RPC配置
    QJsonObject rpc;
    rpc["serverHost"] = rpc_.serverHost;
    rpc["serverPort"] = int(rpc_.serverPort);
    rpc["reconnectInterval"] = rpc_.reconnectInterval;
    rpc["autoReconnect"] = rpc_.autoReconnect;
    root["rpc"] = rpc;

    // 日志配置
    QJsonObject log;
    log["logToConsole"] = log_.logToConsole;
    log["logToFile"] = log_.logToFile;
    log["logFilePath"] = log_.logFilePath;
    log["logLevel"] = log_.logLevel;
    root["log"] = log;

    QJsonDocument doc(root);
    const QByteArray data = doc.toJson(QJsonDocument::Indented);
    return writeTextFile(path, data, err);
}
