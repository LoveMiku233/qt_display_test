#ifndef CARD_WIDGET_H
#define CARD_WIDGET_H

#include <QFrame>

class QLabel;

struct RelayStatusUi {
    bool ok = false;
    int node = 0;
    int ch = 0;
    double currentA = 0.0;
    int mode = 0;
    bool phaseLost = false;
};

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

private:
    int nodeId_ = 0;
    QLabel* title_ = nullptr;
    QLabel* line1_ = nullptr;
    QLabel* line2_ = nullptr;
};

#endif // CARD_WIDGET_H
