#ifndef CARD_WIDGET_H
#define CARD_WIDGET_H

#include <QFrame>

class QLabel;
class QGraphicsDropShadowEffect;

struct RelayStatusUi {
    bool ok = false;
    int node = 0;
    int ch = 0;
    double currentA = 0.0;
    int mode = 0;
    bool phaseLost = false;
};

/**
 * @brief A modern, animated device card widget
 * 
 * Displays device information with hover and click animations,
 * drop shadow effects, and modern styling.
 */
class DeviceCardWidget : public QFrame
{
    Q_OBJECT
public:
    explicit DeviceCardWidget(int nodeId, QWidget* parent=nullptr);

    int nodeId() const { return nodeId_; }
    void setTitle(const QString& t);
    void setSummary(const RelayStatusUi& st);
    void setSummaryText(const QString& l1, const QString& l2);

signals:
    void clicked(int nodeId);

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void enterEvent(QEvent* e) override;
    void leaveEvent(QEvent* e) override;

private:
    void animateShadow(int targetBlur, int duration);
    
    int nodeId_ = 0;
    QLabel* title_ = nullptr;
    QLabel* line1_ = nullptr;
    QLabel* line2_ = nullptr;
    QGraphicsDropShadowEffect* shadowEffect_ = nullptr;
    bool isHovered_ = false;
    bool isPressed_ = false;
};

#endif // CARD_WIDGET_H
