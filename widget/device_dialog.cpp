/**
 * @file device_dialog.cpp
 * @brief 设备控制对话框实现
 * 
 * 提供设备状态查看和控制功能的模态对话框，
 * 支持查询设备状态和发送控制命令。
 */

#include "device_dialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QShowEvent>
#include <QGuiApplication>
#include <QScreen>
#include <QTimer>

#include "rpc/json_rpc_client.h"
#include "utils/logger.h"

#include <QJsonObject>
#include <QJsonDocument>

// 日志来源标识
static const QString LOG_SOURCE = "设备对话框";

/**
 * @brief 构造函数，初始化设备控制对话框
 * @param nodeId 设备节点ID
 * @param rpc RPC客户端指针
 * @param parent 父控件指针
 */
DeviceDialog::DeviceDialog(int nodeId, JsonRpcClient* rpc, QWidget* parent)
    : QDialog(parent), nodeId_(nodeId), rpc_(rpc)
{
    setWindowTitle(QString("设备: RelayGD427   节点: %1").arg(nodeId_));

    setModal(true);
    resize(520, 320);
    
    LOG_DEBUG(LOG_SOURCE, QString("打开设备对话框，节点: %1").arg(nodeId_));

    // 应用深色主题样式
    setStyleSheet(R"(
    QDialog { background: #0f141a; }
    QLabel { background:#1f232a; color: #e6edf3; font-size: 14px; }
    QPushButton { padding: 10px 14px; border-radius: 10px; background:#1f232a; color:#e6edf3; font-size: 13px; }
    QPushButton:hover { background:#2a303a; }
    QPushButton#danger { background:#8b1d1d; }
    QPushButton#danger:hover { background:#a52525; }
    QPushButton#primary { background:#1d4ed8; }
    QPushButton#primary:hover { background:#2563eb; }
    QFrame#card { background:#1f232a; border: 1px solid rgba(255,255,255,0.08); border-radius: 14px; }
QComboBox {
  padding: 6px 10px;
  border-radius: 8px;
  border: 1px solid rgba(255,255,255,0.12);
  background: #1f232a;
  color: #e6edf3;
  font-size: 13px;
}
QComboBox:hover {
  border: 1px solid rgba(80,160,255,0.9);
}
QComboBox::drop-down {
  width: 26px;
  border: 0px;
}
QComboBox QAbstractItemView {
  background: #0f141a;              /* 下拉列表背景（未选中） */
  color: #e6edf3;                   /* 下拉列表文字颜色 */
  border: 1px solid rgba(255,255,255,0.12);
  selection-background-color: #2563eb; /* 选中项背景色 */
  selection-color: #ffffff;         /* 选中项文字颜色 */
  outline: 0;
}
QComboBox QAbstractItemView::item {
  padding: 8px 10px;
}
QComboBox QAbstractItemView::item:hover {
  background: #2a303a;              /* 鼠标悬停项背景色 */
}
    )");

    // 创建标题标签
    auto* title = new QLabel(QString("设备: RelayGD427   节点: %1").arg(nodeId_), this);
    title->setStyleSheet("font-size:16px;font-weight:700;color:#ffffff;");

    // 创建状态标签
    statusLabel_ = new QLabel("加载中...", this);
    statusLabel_->setWordWrap(true);                 // 防止文本被截断
    statusLabel_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // 创建通道选择下拉框
    chBox_ = new QComboBox(this);
    chBox_->setMinimumWidth(80);
    chBox_->addItems({"通道0","通道1","通道2","通道3"});

    // 创建查询按钮
    auto* btnQuery = new QPushButton("查询", this);
    btnQuery->setObjectName("primary");

    // 创建关闭按钮
    auto* btnClose = new QPushButton("关闭", this);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::close);

    // 顶部控制条布局：通道选择 + 查询 + 关闭
    auto* topRow = new QHBoxLayout;
    topRow->addWidget(new QLabel("通道:", this));
    topRow->addWidget(chBox_);
    topRow->addStretch(1);
    topRow->addWidget(btnQuery);
    topRow->addWidget(btnClose);

    // 状态卡片区域
    auto* card = new QFrame(this);
    card->setObjectName("card");
    auto* cardLay = new QVBoxLayout(card);
    cardLay->setContentsMargins(14, 12, 14, 12);
    cardLay->addWidget(new QLabel("状态信息", this));
    cardLay->addWidget(statusLabel_);

    // 设备控制按钮
    auto* btnStop = new QPushButton("停止", this);
    btnStop->setObjectName("danger");
    auto* btnFwd  = new QPushButton("正转", this);
    btnFwd->setObjectName("primary");
    auto* btnRev  = new QPushButton("反转", this);
    btnRev->setObjectName("primary");

    auto* btnRow = new QHBoxLayout;
    btnRow->addWidget(btnStop);
    btnRow->addWidget(btnFwd);
    btnRow->addWidget(btnRev);

    // 主布局
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 14, 16, 14);
    root->setSpacing(12);
    root->addWidget(title);
    root->addLayout(topRow);
    root->addWidget(card);
    root->addLayout(btnRow);

    // 连接按钮信号
    connect(btnQuery, &QPushButton::clicked, this, [this]{ refresh(); });
    connect(btnStop, &QPushButton::clicked, this, [this]{ sendControl("stop"); });
    connect(btnFwd,  &QPushButton::clicked, this, [this]{ sendControl("fwd"); });
    connect(btnRev,  &QPushButton::clicked, this, [this]{ sendControl("rev"); });
    connect(chBox_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int){ refresh(); });

    // 创建定时刷新器
    timer_ = new QTimer(this);
    timer_->setInterval(1500);
    connect(timer_, &QTimer::timeout, this, &DeviceDialog::refresh);
    timer_->start();

    refresh();
}

/**
 * @brief 刷新设备状态
 * 
 * 通过RPC查询当前通道的设备状态
 */
void DeviceDialog::refresh()
{
    if (!rpc_) return;

    const int ch = chBox_->currentIndex();

    // 避免重复请求
    if (statusInFlight_ && ch == lastStatusCh_) return;

    statusInFlight_ = true;
    lastStatusCh_ = ch;

    // 首次或长时间无响应时显示加载中
    if (statusLabel_->text().isEmpty() || statusLabel_->text() == "加载中...") {
        statusLabel_->setText("加载中...");
    }

    LOG_DEBUG(LOG_SOURCE, QString("查询设备状态，节点: %1, 通道: %2").arg(nodeId_).arg(ch));

    QJsonObject params{{"node", nodeId_}, {"ch", ch}};
    rpc_->callAsync("relay.status", params,
        [this, ch](const QJsonValue& result, const QJsonObject& error){
            if (ch != lastStatusCh_) return;

            statusInFlight_ = false;

            if (!error.isEmpty()) {
                LOG_WARNING(LOG_SOURCE, QString("查询状态失败: %1").arg(error.value("message").toString()));
                statusLabel_->setText(QString("RPC错误: %1").arg(error.value("message").toString()));
                return;
            }

            const QString txt = QString::fromUtf8(
                QJsonDocument(result.toObject()).toJson(QJsonDocument::Indented)
            );

            // 内容没变化时不刷新（减少重绘闪烁）
            if (statusLabel_->text() != txt)
                statusLabel_->setText(txt);
        },
        1500
    );
}

/**
 * @brief 发送控制命令
 * @param action 控制动作（stop/fwd/rev）
 */
void DeviceDialog::sendControl(const QString& action)
{
    if (!rpc_) return;

    const int ch = chBox_->currentIndex();
    
    LOG_INFO(LOG_SOURCE, QString("发送控制命令，节点: %1, 通道: %2, 动作: %3")
        .arg(nodeId_).arg(ch).arg(action));

    QJsonObject params{{"node", nodeId_}, {"ch", ch}, {"action", action}};
    setEnabled(false);
    rpc_->callAsync("relay.control", params,
        [this, action](const QJsonValue& result, const QJsonObject& error){
            Q_UNUSED(result);
            setEnabled(true);

            if (!error.isEmpty()) {
                LOG_ERROR(LOG_SOURCE, QString("控制命令失败: %1").arg(error.value("message").toString()));
                statusLabel_->setText(QString("控制失败: %1").arg(error.value("message").toString()));
                return;
            }
            
            LOG_INFO(LOG_SOURCE, QString("控制命令成功: %1").arg(action));

            // 刷新状态显示
            refresh();
        },
        1500
    );
}

/**
 * @brief 显示事件处理
 * @param e 显示事件
 * 
 * 对话框显示时居中于父窗口或屏幕
 */
void DeviceDialog::showEvent(QShowEvent* e)
{
    QDialog::showEvent(e);

    if (auto* p = parentWidget()) {
        const QRect pr = p->frameGeometry();
        move(pr.center() - rect().center());
    } else if (auto* sc = QGuiApplication::primaryScreen()) {
        const QRect sr = sc->availableGeometry();
        move(sr.center() - rect().center());
    }
}
