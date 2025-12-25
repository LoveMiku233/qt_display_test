#include "card_widget.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>

DeviceCardWidget::DeviceCardWidget(int nodeId, QWidget* parent)
    : QFrame(parent), nodeId_(nodeId)
{
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);
    setStyleSheet("QFrame{background:#222;border-radius:10px;} QLabel{color:#eee;}");

    title_ = new QLabel(QString("Node %1").arg(nodeId_), this);
    title_->setStyleSheet("font-weight:600;font-size:16px;color:#fff;");

    line1_ = new QLabel("—", this);
    line2_ = new QLabel("Click to open", this);
    line2_->setStyleSheet("color:#aaa;");

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(12, 10, 12, 10);
    lay->addWidget(title_);
    lay->addWidget(line1_);
    lay->addWidget(line2_);
    lay->addStretch(1);
}

void DeviceCardWidget::setTitle(const QString& t)
{
    title_->setText(t);
}

void DeviceCardWidget::setSummary(const RelayStatusUi& st)
{
    if (!st.ok) {
        line1_->setText("Offline / no data");
        line2_->setText("");
        return;
    }
    line1_->setText(QString("CH%1  I=%2A").arg(st.ch).arg(st.currentA, 0, 'f', 2));
    line2_->setText(QString("mode=%1  phaseLost=%2").arg(st.mode).arg(st.phaseLost ? "Y" : "N"));
}

void DeviceCardWidget::setSummaryText(const QString& l1, const QString& l2)
{
    line1_->setText(l1);
    line2_->setText(l2);
}

void DeviceCardWidget::mousePressEvent(QMouseEvent* e)
{
    QFrame::mousePressEvent(e);
    emit clicked(nodeId_);
}
