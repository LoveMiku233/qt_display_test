#ifndef SETTING_PAGE_H
#define SETTING_PAGE_H

#include <QWidget>

namespace Ui {
class SettingPage;
}


class SettingPage : public QWidget
{
    Q_OBJECT
public:
    explicit SettingPage(QWidget *parent = nullptr);
    ~SettingPage();

private:
    Ui::SettingPage *ui;

};

#endif // SETTING_PAGE_H
