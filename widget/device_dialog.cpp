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

#include <QJsonObject>
#include <QJsonDocument>

DeviceDialog::DeviceDialog(int nodeId, JsonRpcClient* rpc, QWidget* parent)
    : QDialog(parent), nodeId_(nodeId), rpc_(rpc)
{
    setWindowTitle(QString("Device: RelayGD427   Node: %1").arg(nodeId_));

    setModal(true);
    resize(520, 320);

    setStyleSheet(R"(
    QDialog { background: #0f141a; }
    QLabel { background:#1f232a; color: #e6edf3; font-size: 14px; }
    QPushButton { padding: 10px 14px; border-radius: 10px; background:#1f232a; color:#e6edf3; }
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

    auto* title = new QLabel(QString("Device: RelayGD427   Node: %1").arg(nodeId_), this);
    title->setStyleSheet("font-size:16px;font-weight:700;color:#ffffff;");

    statusLabel_ = new QLabel("Loading...", this);
    statusLabel_->setWordWrap(true);                 // 防止被截断
    statusLabel_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    chBox_ = new QComboBox(this);
    chBox_->setMinimumWidth(80);
    chBox_->addItems({"CH0","CH1","CH2","CH3"});


    auto* btnQuery = new QPushButton("Query", this);
    btnQuery->setObjectName("primary");


    auto* btnClose = new QPushButton("Close", this);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::close);

    // 顶部控制条：CH + Query + Close
    auto* topRow = new QHBoxLayout;
    topRow->addWidget(new QLabel("Channel:", this));
    topRow->addWidget(chBox_);
    topRow->addStretch(1);
    topRow->addWidget(btnQuery);
    topRow->addWidget(btnClose);

    // 状态卡片（更规整）
    auto* card = new QFrame(this);
    card->setObjectName("card");
    auto* cardLay = new QVBoxLayout(card);
    cardLay->setContentsMargins(14, 12, 14, 12);
    cardLay->addWidget(new QLabel("Status", this));
    cardLay->addWidget(statusLabel_);

    // 控制按钮
    auto* btnStop = new QPushButton("STOP", this);
    btnStop->setObjectName("danger");
    auto* btnFwd  = new QPushButton("FORWARD", this);
    btnFwd->setObjectName("primary");
    auto* btnRev  = new QPushButton("REVERSE", this);
    btnRev->setObjectName("primary");

    auto* btnRow = new QHBoxLayout;
    btnRow->addWidget(btnStop);
    btnRow->addWidget(btnFwd);
    btnRow->addWidget(btnRev);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 14, 16, 14);
    root->setSpacing(12);
    root->addWidget(title);
    root->addLayout(topRow);
    root->addWidget(card);
    root->addLayout(btnRow);

    // 连接按钮
    connect(btnQuery, &QPushButton::clicked, this, [this]{ refresh(); });
    connect(btnStop, &QPushButton::clicked, this, [this]{ sendControl("stop"); });
    connect(btnFwd,  &QPushButton::clicked, this, [this]{ sendControl("fwd"); });
    connect(btnRev,  &QPushButton::clicked, this, [this]{ sendControl("rev"); });
    connect(chBox_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int){ refresh(); });

    timer_ = new QTimer(this);
    timer_->setInterval(1500);
    connect(timer_, &QTimer::timeout, this, &DeviceDialog::refresh);
    timer_->start();

    refresh();
}

void DeviceDialog::refresh()
{
    if (!rpc_) return;

    const int ch = chBox_->currentIndex();

    if (statusInFlight_ && ch == lastStatusCh_) return;

    statusInFlight_ = true;
    lastStatusCh_ = ch;

    // 不要每次都 setText("Loading...")，只在第一次或超时很久时显示
    if (statusLabel_->text().isEmpty() || statusLabel_->text() == "Loading...") {
        statusLabel_->setText("Loading...");
    }

    QJsonObject params{{"node", nodeId_}, {"ch", ch}};
    rpc_->callAsync("relay.status", params,
        [this, ch](const QJsonValue& result, const QJsonObject& error){
            if (ch != lastStatusCh_) return;

            statusInFlight_ = false;

            if (!error.isEmpty()) {
                statusLabel_->setText(QString("RPC error: %1").arg(error.value("message").toString()));
                return;
            }

            const QString txt = QString::fromUtf8(
                QJsonDocument(result.toObject()).toJson(QJsonDocument::Indented)
            );

            // 内容没变化就不 setText（减少重绘闪烁）
            if (statusLabel_->text() != txt)
                statusLabel_->setText(txt);
        },
        1500
    );
}

void DeviceDialog::sendControl(const QString& action)
{
    if (!rpc_) return;

    const int ch = chBox_->currentIndex();

    QJsonObject params{{"node", nodeId_}, {"ch", ch}, {"action", action}};
    setEnabled(false);
    rpc_->callAsync("relay.control", params,
        [this](const QJsonValue& result, const QJsonObject& error){
            setEnabled(true);

            if (!error.isEmpty()) {
                statusLabel_->setText(QString("Control failed: %1").arg(error.value("message").toString()));
                return;
            }

            // ctrl refresh
            refresh();
        },
        1500
    );
}

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
