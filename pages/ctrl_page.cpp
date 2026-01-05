#include "ctrl_page.h"
#include "ui_ctrl_page.h"

#include "widget/card_widget.h"
#include "widget/device_dialog.h"
#include "utils/glass_style.h"

#include <QGridLayout>
#include <QTimer>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "rpc/json_rpc_client.h"
#include "app_context.h"

CtrlPage::CtrlPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CtrlPage)
{
    ui->setupUi(this);

    // 应用玻璃拟态风格样式
    setStyleSheet(GlassStyle::getFullPageStyle());

    // get rpc
    rpc_ = AppContext::instance().rpc();

    loadCards();
    // timer 1s
    auto* t = new QTimer(this);
    t->setInterval(200);
    connect(t, &QTimer::timeout, this, &CtrlPage::refreshCards);
    t->start();

    connect(rpc_, &JsonRpcClient::callFinished, this,
            [this](int, const QJsonValue&, const QJsonObject& error){
        if (!error.isEmpty())
            qWarning().noquote() << "RPC error=" << QJsonDocument(error).toJson(QJsonDocument::Compact);
    });

    refreshCards();
}


CtrlPage::~CtrlPage() {
    delete ui;
}


void CtrlPage::loadCards()
{
    QList<int> nodes;
    if (rpc_) {
        auto res = rpc_->call("relay.nodes", QJsonObject{});
        const auto obj = res.toObject();
        const auto arr = obj.value("nodes").toArray();
        for (auto v : arr) nodes.append(v.toInt());
    }

    // empty add 1 2 3
    if (nodes.isEmpty()) nodes = {1,2,3};

    QGridLayout* grid = ui->gridLayout;

    int i = 0;
    for (int nodeId : nodes) {
        auto* card = new DeviceCardWidget(nodeId, this);
        card->setTitle(QString("Relay Node %1").arg(nodeId));

        const int row = i / 2;
        const int col = i % 2;
        grid->addWidget(card, row, col);

        connect(card, &DeviceCardWidget::clicked, this, [this](int node){
            DeviceDialog dlg(node, rpc_, this);
            dlg.exec();
        });

        cards_.append(card);
        i++;
    }

    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);
}

void CtrlPage::refreshCards()
{
    if (!rpc_ || !rpc_->isConnected() || cards_.isEmpty()) return;
    if (refreshInFlight_) return;                // 上一次还没回

    if (refreshIndex_ >= cards_.size()) refreshIndex_ = 0;
    auto* card = cards_[refreshIndex_++];
    const int node = card->nodeId();

    refreshInFlight_ = true;

    QJsonObject params{{"node", node}};
    rpc_->callAsync("relay.statusAll", params,
        [this, card](const QJsonValue& result, const QJsonObject& error){
            refreshInFlight_ = false;

            if (!card) return;

            if (!error.isEmpty()) {
                const QString msg = error.value("message").toString("RPC error");
                card->setSummaryText("Offline / no data", msg);
                return;
            }

            const auto res = result.toObject();
            const bool ok = res.value("ok").toBool(false);
            const auto arr = res.value("channels").toArray();

            if (!ok || arr.isEmpty()) {
                card->setSummaryText("Offline / no data", "");
                return;
            }

            const bool online = res.value("online").toBool(true);
            const int ageMs = res.value("ageMs").toInt(0);
            if (!online) {
                card->setSummaryText("Offline", QString("last seen %1 ms ago").arg(ageMs));
                return;
            }

            double currents[4] = {0,0,0,0};
            int modes[4] = {0,0,0,0};
            bool phaseLost[4] = {false,false,false,false};
            bool anyPhaseLost = false;

            for (const auto& it : arr) {
                if (!it.isObject()) continue;
                const auto o = it.toObject();
                const int ch = o.value("ch").toInt(o.value("channel").toInt(-1));
                if (ch < 0 || ch > 3) continue;
                currents[ch] = o.value("currentA").toDouble(0);
                modes[ch] = o.value("mode").toInt(0);
                phaseLost[ch] = o.value("phaseLost").toBool(false);
                anyPhaseLost = anyPhaseLost || phaseLost[ch];
            }

            const QString l1 = QString("CH0:%1A(m%2)  CH1:%3A(m%4)")
                    .arg(currents[0], 0, 'f', 2).arg(modes[0])
                    .arg(currents[1], 0, 'f', 2).arg(modes[1]);

            const QString l2 = QString("CH2:%1A(m%2)  CH3:%3A(m%4)%5")
                    .arg(currents[2], 0, 'f', 2).arg(modes[2])
                    .arg(currents[3], 0, 'f', 2).arg(modes[3])
                    .arg(anyPhaseLost ? "  PL:Y" : "");

            card->setSummaryText(l1, l2);
        },
        600 // timeout，别太大，避免刷新被卡住
    );
}
