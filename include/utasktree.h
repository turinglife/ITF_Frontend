#ifndef UTASKTREE_H
#define UTASKTREE_H

#include <QTreeWidget>

class UTaskTree : public QTreeWidget
{
    Q_OBJECT
public:
    explicit UTaskTree(QWidget *parent = 0);
    ~UTaskTree();

    void set_channel_status(int channel, bool flag);
signals:
    void CreateGroup();
    void DeleteGroup();
    void CreateTask();
    void DeleteTask();
    void SelectChannel(QString, int);

private slots:
    void OnCustomContextMenuRequested(const QPoint &pos);

private:
    QAction *p_create_group_;
    QAction *p_delete_group_;
    QAction *p_create_task_;
    QAction *p_delete_task_;

    QList<QAction *> channels_;
};

#endif // UTASKTREE_H
