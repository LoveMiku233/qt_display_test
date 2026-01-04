#include "auto_ctrl_page.h"
#include "ui_auto_ctrl_page.h"
#include "app_context.h"
#include "rpc/json_rpc_client.h"

#include <QJsonObject>
#include <QMessageBox>
#include <QDateTime>

AutoCtrlPage::AutoCtrlPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AutoCtrlPage)
{
    ui->setupUi(this);

    autoTimer = new QTimer(this);
    connect(autoTimer, &QTimer::timeout, this, &AutoCtrlPage::onAutoTick);

    // Connect buttons
    connect(ui->btnStart, &QPushButton::clicked, this, &AutoCtrlPage::onStartAuto);
    connect(ui->btnStop, &QPushButton::clicked, this, &AutoCtrlPage::onStopAuto);

    updateStatus();
}

AutoCtrlPage::~AutoCtrlPage() {
    if (autoTimer) {
        autoTimer->stop();
    }
    delete ui;
}

void AutoCtrlPage::onStartAuto() {
    if (autoRunning) return;

    int interval = ui->spinInterval->value();
    if (interval < 1) {
        QMessageBox::warning(this, "错误", "间隔时间必须大于0");
        return;
    }

    autoRunning = true;
    autoTimer->start(interval * 1000);
    
    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(true);
    ui->spinInterval->setEnabled(false);
    
    updateStatus();
    
    QMessageBox::information(this, "提示", "自动控制已启动");
}

void AutoCtrlPage::onStopAuto() {
    if (!autoRunning) return;

    autoRunning = false;
    autoTimer->stop();
    
    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(false);
    ui->spinInterval->setEnabled(true);
    
    updateStatus();
    
    QMessageBox::information(this, "提示", "自动控制已停止");
}

void AutoCtrlPage::onAutoTick() {
    // This is a simple example of auto control
    // In a real implementation, this would contain the actual control logic
    
    auto* rpc = AppContext::instance().rpc();
    if (!rpc) {
        ui->labelStatus->setText("状态: RPC未连接");
        onStopAuto();
        return;
    }

    // Example: Query relay nodes
    auto resp = rpc->call("relay.nodes", QJsonObject());
    
    if (resp.isObject()) {
        auto respObj = resp.toObject();
        if (respObj.contains("result")) {
            auto result = respObj["result"].toObject();
            auto nodes = result["nodes"].toArray();
            
            QString statusText = QString("自动控制运行中 - 检测到 %1 个设备节点")
                                     .arg(nodes.size());
            ui->labelStatus->setText(statusText);
            
            QString logEntry = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") +
                              QString(" - 检测到 %1 个设备").arg(nodes.size());
            ui->textLog->append(logEntry);
            return;
        }
    }
    
    ui->labelStatus->setText("状态: 查询失败");
    
    // Add your automatic control logic here
    // For example: monitor device status and take actions based on conditions
}

void AutoCtrlPage::updateStatus() {
    if (autoRunning) {
        ui->labelStatus->setText("状态: 运行中");
        ui->labelMode->setText("模式: 自动");
    } else {
        ui->labelStatus->setText("状态: 已停止");
        ui->labelMode->setText("模式: 手动");
    }
}
