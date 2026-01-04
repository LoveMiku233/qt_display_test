#ifndef GROUP_PAGE_H
#define GROUP_PAGE_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class GroupPage;
}

class GroupPage : public QWidget
{
    Q_OBJECT
public:
    explicit GroupPage(QWidget *parent = nullptr);
    ~GroupPage();

private slots:
    void onRefreshGroups();
    void onCreateGroup();
    void onDeleteGroup();
    void onAddDevice();
    void onRemoveDevice();
    void onGroupSelected(QListWidgetItem* item);

private:
    Ui::GroupPage *ui;
    int currentGroupId = -1;

    void loadGroups();
    void loadGroupDevices(int groupId);
};

#endif // GROUP_PAGE_H
