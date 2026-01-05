#include "group_page.h"
#include "ui_group_page.h"
#include "app_context.h"
#include "rpc/json_rpc_client.h"
#include "utils/glass_style.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QInputDialog>
#include <QMessageBox>

GroupPage::GroupPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GroupPage)
{
    ui->setupUi(this);

    // 应用玻璃拟态风格样式
    setStyleSheet(GlassStyle::getFullPageStyle());

    // Connect signals
    connect(ui->btnRefresh, &QPushButton::clicked, this, &GroupPage::onRefreshGroups);
    connect(ui->btnCreateGroup, &QPushButton::clicked, this, &GroupPage::onCreateGroup);
    connect(ui->btnDeleteGroup, &QPushButton::clicked, this, &GroupPage::onDeleteGroup);
    connect(ui->btnAddDevice, &QPushButton::clicked, this, &GroupPage::onAddDevice);
    connect(ui->btnRemoveDevice, &QPushButton::clicked, this, &GroupPage::onRemoveDevice);
    connect(ui->listGroups, &QListWidget::itemClicked, this, &GroupPage::onGroupSelected);

    // Initial load
    loadGroups();
}

GroupPage::~GroupPage() {
    delete ui;
}

void GroupPage::loadGroups() {
    ui->listGroups->clear();
    
    auto* rpc = AppContext::instance().rpc();
    if (!rpc) {
        ui->labelStatus->setText("RPC未连接");
        return;
    }

    auto resp = rpc->call("group.list", QJsonObject());
    if (!resp.isObject()) {
        ui->labelStatus->setText("响应格式错误");
        return;
    }
    
    auto respObj = resp.toObject();
    if (respObj.contains("error")) {
        ui->labelStatus->setText("加载组失败");
        return;
    }

    if (respObj.contains("result")) {
        auto result = respObj["result"].toObject();
        auto groups = result["groups"].toArray();
        
        for (const auto& g : groups) {
            auto group = g.toObject();
            int groupId = group["groupId"].toInt();
            QString name = group["name"].toString();
            int deviceCount = group["deviceCount"].toInt();
            
            QString itemText = QString("ID:%1 - %2 (%3个设备)")
                                   .arg(groupId)
                                   .arg(name)
                                   .arg(deviceCount);
            
            auto* item = new QListWidgetItem(itemText);
            item->setData(Qt::UserRole, groupId);
            ui->listGroups->addItem(item);
        }
        
        ui->labelStatus->setText(QString("共 %1 个组").arg(groups.size()));
    }
}

void GroupPage::loadGroupDevices(int groupId) {
    ui->listDevices->clear();
    
    auto* rpc = AppContext::instance().rpc();
    if (!rpc) return;

    auto resp = rpc->call("group.list", QJsonObject());
    if (resp.isObject()) {
        auto respObj = resp.toObject();
        if (respObj.contains("result")) {
            auto result = respObj["result"].toObject();
            auto groups = result["groups"].toArray();
            
            for (const auto& g : groups) {
                auto group = g.toObject();
                if (group["groupId"].toInt() == groupId) {
                    auto devices = group["devices"].toArray();
                    
                    for (const auto& d : devices) {
                        int nodeId = d.toInt();
                        QString itemText = QString("设备节点: %1").arg(nodeId);
                        auto* item = new QListWidgetItem(itemText);
                        item->setData(Qt::UserRole, nodeId);
                        ui->listDevices->addItem(item);
                    }
                    break;
                }
            }
        }
    }
}

void GroupPage::onRefreshGroups() {
    loadGroups();
    ui->listDevices->clear();
    currentGroupId = -1;
}

void GroupPage::onCreateGroup() {
    bool ok;
    int groupId = QInputDialog::getInt(this, "创建新组", "输入组ID:", 1, 1, 999, 1, &ok);
    if (!ok) return;
    
    QString name = QInputDialog::getText(this, "创建新组", "输入组名称:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    auto* rpc = AppContext::instance().rpc();
    if (!rpc) return;

    QJsonObject params;
    params["groupId"] = groupId;
    params["name"] = name;
    
    auto resp = rpc->call("group.create", params);
    if (resp.isObject()) {
        auto respObj = resp.toObject();
        if (respObj.contains("error")) {
            auto error = respObj["error"].toObject();
            QMessageBox::warning(this, "错误", error["message"].toString());
        } else {
            QMessageBox::information(this, "成功", "组创建成功");
            loadGroups();
        }
    }
}

void GroupPage::onDeleteGroup() {
    if (currentGroupId < 0) {
        QMessageBox::warning(this, "警告", "请先选择一个组");
        return;
    }

    auto reply = QMessageBox::question(this, "确认", "确定要删除此组吗？");
    if (reply != QMessageBox::Yes) return;

    auto* rpc = AppContext::instance().rpc();
    if (!rpc) return;

    QJsonObject params;
    params["groupId"] = currentGroupId;
    
    auto resp = rpc->call("group.delete", params);
    if (resp.isObject()) {
        auto respObj = resp.toObject();
        if (respObj.contains("error")) {
            auto error = respObj["error"].toObject();
            QMessageBox::warning(this, "错误", error["message"].toString());
        } else {
            QMessageBox::information(this, "成功", "组已删除");
            currentGroupId = -1;
            loadGroups();
        }
    }
}

void GroupPage::onAddDevice() {
    if (currentGroupId < 0) {
        QMessageBox::warning(this, "警告", "请先选择一个组");
        return;
    }

    bool ok;
    int nodeId = QInputDialog::getInt(this, "添加设备", "输入设备节点ID:", 1, 1, 255, 1, &ok);
    if (!ok) return;

    auto* rpc = AppContext::instance().rpc();
    if (!rpc) return;

    QJsonObject params;
    params["groupId"] = currentGroupId;
    params["node"] = nodeId;
    
    auto resp = rpc->call("group.addDevice", params);
    if (resp.isObject()) {
        auto respObj = resp.toObject();
        if (respObj.contains("error")) {
            auto error = respObj["error"].toObject();
            QMessageBox::warning(this, "错误", error["message"].toString());
        } else {
            QMessageBox::information(this, "成功", "设备已添加到组");
            loadGroupDevices(currentGroupId);
            loadGroups(); // Refresh to update device count
        }
    }
}

void GroupPage::onRemoveDevice() {
    if (currentGroupId < 0) {
        QMessageBox::warning(this, "警告", "请先选择一个组");
        return;
    }

    auto* item = ui->listDevices->currentItem();
    if (!item) {
        QMessageBox::warning(this, "警告", "请先选择一个设备");
        return;
    }

    int nodeId = item->data(Qt::UserRole).toInt();

    auto* rpc = AppContext::instance().rpc();
    if (!rpc) return;

    QJsonObject params;
    params["groupId"] = currentGroupId;
    params["node"] = nodeId;
    
    auto resp = rpc->call("group.removeDevice", params);
    if (resp.isObject()) {
        auto respObj = resp.toObject();
        if (respObj.contains("error")) {
            auto error = respObj["error"].toObject();
            QMessageBox::warning(this, "错误", error["message"].toString());
        } else {
            QMessageBox::information(this, "成功", "设备已从组中移除");
            loadGroupDevices(currentGroupId);
            loadGroups(); // Refresh to update device count
        }
    }
}

void GroupPage::onGroupSelected(QListWidgetItem* item) {
    if (!item) return;
    
    currentGroupId = item->data(Qt::UserRole).toInt();
    loadGroupDevices(currentGroupId);
    
    ui->labelStatus->setText(QString("选中组 ID: %1").arg(currentGroupId));
}
