#include "card_widget.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>

// Modern card style constants
namespace CardStyle {
    const QString CARD_NORMAL = R"(
        QFrame {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #2d3748, stop:1 #1a202c);
            border-radius: 12px;
            border: 1px solid #3d4852;
        }
        QLabel {
            color: #e2e8f0;
            background: transparent;
            border: none;
        }
    )";
    
    const QString CARD_HOVER = R"(
        QFrame {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #3d4a5c, stop:1 #252d3a);
            border-radius: 12px;
            border: 1px solid #4299e1;
        }
        QLabel {
            color: #e2e8f0;
            background: transparent;
            border: none;
        }
    )";
    
    const QString CARD_PRESSED = R"(
        QFrame {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #252d3a, stop:1 #1a202c);
            border-radius: 12px;
            border: 1px solid #63b3ed;
        }
        QLabel {
            color: #e2e8f0;
            background: transparent;
            border: none;
        }
    )";
}

DeviceCardWidget::DeviceCardWidget(int nodeId, QWidget* parent)
    : QFrame(parent), nodeId_(nodeId), isHovered_(false), isPressed_(false)
{
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_Hover, true);
    setMinimumHeight(100);
    
    // Apply modern style
    setStyleSheet(CardStyle::CARD_NORMAL);
    
    // Add drop shadow effect
    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setOffset(0, 4);
    setGraphicsEffect(shadow);
    shadowEffect_ = shadow;

    title_ = new QLabel(QString("Node %1").arg(nodeId_), this);
    title_->setStyleSheet("font-weight:600; font-size:16px; color:#fff;");

    line1_ = new QLabel("—", this);
    line1_->setStyleSheet("font-size:13px; color:#a0aec0;");
    
    line2_ = new QLabel("Click to open", this);
    line2_->setStyleSheet("font-size:12px; color:#718096;");

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(16, 14, 16, 14);
    lay->setSpacing(6);
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
    isPressed_ = true;
    setStyleSheet(CardStyle::CARD_PRESSED);
    animateShadow(8, 100);
    QFrame::mousePressEvent(e);
}

void DeviceCardWidget::mouseReleaseEvent(QMouseEvent* e)
{
    isPressed_ = false;
    if (isHovered_) {
        setStyleSheet(CardStyle::CARD_HOVER);
        animateShadow(20, 150);
    } else {
        setStyleSheet(CardStyle::CARD_NORMAL);
        animateShadow(15, 150);
    }
    
    // Emit clicked only if release is within widget
    if (rect().contains(e->pos())) {
        emit clicked(nodeId_);
    }
    
    QFrame::mouseReleaseEvent(e);
}

void DeviceCardWidget::enterEvent(QEvent* e)
{
    isHovered_ = true;
    if (!isPressed_) {
        setStyleSheet(CardStyle::CARD_HOVER);
        animateShadow(20, 150);
    }
    QFrame::enterEvent(e);
}

void DeviceCardWidget::leaveEvent(QEvent* e)
{
    isHovered_ = false;
    if (!isPressed_) {
        setStyleSheet(CardStyle::CARD_NORMAL);
        animateShadow(15, 150);
    }
    QFrame::leaveEvent(e);
}

void DeviceCardWidget::animateShadow(int targetBlur, int duration)
{
    if (!shadowEffect_) return;
    
    auto* anim = new QPropertyAnimation(shadowEffect_, "blurRadius", this);
    anim->setDuration(duration);
    anim->setEndValue(targetBlur);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
