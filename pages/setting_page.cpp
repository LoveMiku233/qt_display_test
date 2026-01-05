#include "setting_page.h"
#include "ui_setting_page.h"
#include "app_context.h"
#include "rpc/json_rpc_client.h"
#include "utils/glass_style.h"

#include <QMessageBox>
#include <QJsonArray>

SettingPage::SettingPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingPage)
{
    ui->setupUi(this);
    
    // 应用极简留白风格样式
    setStyleSheet(GlassStyle::getFullPageStyle());
    
    // Connect buttons
    connect(ui->btnSave, &QPushButton::clicked, this, [this](){
        QMessageBox::information(this, "提示", "设置已保存");
    });
    
    connect(ui->btnReset, &QPushButton::clicked, this, [this](){
        ui->spinRpcPort->setValue(12345);
        ui->editCanInterface->setText("can0");
        ui->comboCanBitrate->setCurrentIndex(0);
        ui->checkTripleSampling->setChecked(true);
        QMessageBox::information(this, "提示", "设置已重置为默认值");
    });
    
    // Load device list
    loadDeviceList();
}


SettingPage::~SettingPage() {
    delete ui;
}

void SettingPage::loadDeviceList() {
    ui->listDevices->clear();
    
    auto* rpc = AppContext::instance().rpc();
    if (!rpc) return;
    
    auto resp = rpc->call("relay.nodes", QJsonObject());
    if (resp.isObject()) {
        auto respObj = resp.toObject();
        if (respObj.contains("result")) {
            auto result = respObj["result"].toObject();
            auto nodes = result["nodes"].toArray();
            
            for (const auto& n : nodes) {
                int nodeId = n.toInt();
                ui->listDevices->addItem(QString("Relay Node: %1").arg(nodeId));
            }
        }
    }
}
