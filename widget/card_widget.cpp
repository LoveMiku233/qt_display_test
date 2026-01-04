/**
 * @file card_widget.cpp
 * @brief 设备卡片组件实现
 * 
 * 实现现代化的设备信息卡片，支持悬停和点击动画效果，
 * 包含阴影效果和渐变背景样式。
 */

#include "card_widget.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>

/**
 * @brief 卡片样式命名空间
 * 
 * 定义卡片组件的样式常量，包括阴影配置和不同状态下的样式
 */
namespace CardStyle {
    // 阴影配置常量
    const int SHADOW_BLUR_NORMAL = 15;     // 普通状态阴影模糊半径
    const int SHADOW_BLUR_HOVER = 20;      // 悬停状态阴影模糊半径
    const int SHADOW_BLUR_PRESSED = 8;     // 按下状态阴影模糊半径
    const int SHADOW_OPACITY = 80;         // 阴影透明度
    const int SHADOW_OFFSET_Y = 4;         // 阴影Y轴偏移
    const int SHADOW_ANIM_DURATION = 150;  // 阴影动画持续时间(毫秒)
    
    // 普通状态样式
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
    
    // 悬停状态样式
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
    
    // 按下状态样式
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

/**
 * @brief 构造函数，初始化设备卡片
 * @param nodeId 设备节点ID
 * @param parent 父控件指针
 * 
 * 创建卡片布局，设置样式和阴影效果
 */
DeviceCardWidget::DeviceCardWidget(int nodeId, QWidget* parent)
    : QFrame(parent), nodeId_(nodeId), isHovered_(false), isPressed_(false)
{
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_Hover, true);
    setMinimumHeight(100);
    
    // 应用现代化样式
    setStyleSheet(CardStyle::CARD_NORMAL);
    
    // 添加阴影效果
    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(CardStyle::SHADOW_BLUR_NORMAL);
    shadow->setColor(QColor(0, 0, 0, CardStyle::SHADOW_OPACITY));
    shadow->setOffset(0, CardStyle::SHADOW_OFFSET_Y);
    setGraphicsEffect(shadow);
    shadowEffect_ = shadow;

    // 创建标题标签
    title_ = new QLabel(QString("节点 %1").arg(nodeId_), this);
    title_->setStyleSheet("font-weight:600; font-size:16px; color:#fff;");

    // 创建信息行标签
    line1_ = new QLabel("—", this);
    line1_->setStyleSheet("font-size:13px; color:#a0aec0;");
    
    line2_ = new QLabel("点击查看详情", this);
    line2_->setStyleSheet("font-size:12px; color:#718096;");

    // 设置布局
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(16, 14, 16, 14);
    lay->setSpacing(6);
    lay->addWidget(title_);
    lay->addWidget(line1_);
    lay->addWidget(line2_);
    lay->addStretch(1);
}

/**
 * @brief 设置卡片标题
 * @param t 标题文本
 */
void DeviceCardWidget::setTitle(const QString& t)
{
    title_->setText(t);
}

/**
 * @brief 设置卡片摘要信息（使用结构体）
 * @param st 设备状态结构体
 */
void DeviceCardWidget::setSummary(const RelayStatusUi& st)
{
    if (!st.ok) {
        line1_->setText("离线 / 无数据");
        line2_->setText("");
        return;
    }
    line1_->setText(QString("通道%1  电流=%2A").arg(st.ch).arg(st.currentA, 0, 'f', 2));
    line2_->setText(QString("模式=%1  缺相=%2").arg(st.mode).arg(st.phaseLost ? "是" : "否"));
}

/**
 * @brief 设置卡片摘要文本
 * @param l1 第一行文本
 * @param l2 第二行文本
 */
void DeviceCardWidget::setSummaryText(const QString& l1, const QString& l2)
{
    line1_->setText(l1);
    line2_->setText(l2);
}

/**
 * @brief 鼠标按下事件处理
 * @param e 鼠标事件
 */
void DeviceCardWidget::mousePressEvent(QMouseEvent* e)
{
    isPressed_ = true;
    setStyleSheet(CardStyle::CARD_PRESSED);
    animateShadow(CardStyle::SHADOW_BLUR_PRESSED, CardStyle::SHADOW_ANIM_DURATION);
    QFrame::mousePressEvent(e);
}

/**
 * @brief 鼠标释放事件处理
 * @param e 鼠标事件
 */
void DeviceCardWidget::mouseReleaseEvent(QMouseEvent* e)
{
    isPressed_ = false;
    if (isHovered_) {
        setStyleSheet(CardStyle::CARD_HOVER);
        animateShadow(CardStyle::SHADOW_BLUR_HOVER, CardStyle::SHADOW_ANIM_DURATION);
    } else {
        setStyleSheet(CardStyle::CARD_NORMAL);
        animateShadow(CardStyle::SHADOW_BLUR_NORMAL, CardStyle::SHADOW_ANIM_DURATION);
    }
    
    // 仅在控件内释放时发出点击信号
    if (rect().contains(e->pos())) {
        emit clicked(nodeId_);
    }
    
    QFrame::mouseReleaseEvent(e);
}

/**
 * @brief 鼠标进入事件处理
 * @param e 事件
 */
void DeviceCardWidget::enterEvent(QEvent* e)
{
    isHovered_ = true;
    if (!isPressed_) {
        setStyleSheet(CardStyle::CARD_HOVER);
        animateShadow(CardStyle::SHADOW_BLUR_HOVER, CardStyle::SHADOW_ANIM_DURATION);
    }
    QFrame::enterEvent(e);
}

/**
 * @brief 鼠标离开事件处理
 * @param e 事件
 */
void DeviceCardWidget::leaveEvent(QEvent* e)
{
    isHovered_ = false;
    if (!isPressed_) {
        setStyleSheet(CardStyle::CARD_NORMAL);
        animateShadow(CardStyle::SHADOW_BLUR_NORMAL, CardStyle::SHADOW_ANIM_DURATION);
    }
    QFrame::leaveEvent(e);
}

/**
 * @brief 执行阴影动画
 * @param targetBlur 目标模糊半径
 * @param duration 动画持续时间(毫秒)
 */
void DeviceCardWidget::animateShadow(int targetBlur, int duration)
{
    if (!shadowEffect_) return;
    
    auto* anim = new QPropertyAnimation(shadowEffect_, "blurRadius", this);
    anim->setDuration(duration);
    anim->setEndValue(targetBlur);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
