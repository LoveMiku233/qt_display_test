#ifndef DEBUG_PAGE_H
#define DEBUG_PAGE_H

#include <QWidget>

namespace Ui {
class DebugPage;
}

class DebugPage : public QWidget
{
    Q_OBJECT
public:
    explicit DebugPage(QWidget *parent = nullptr);
    ~DebugPage();

private:
    Ui::DebugPage *ui;
};

#endif // DEBUG_PAGE_H
