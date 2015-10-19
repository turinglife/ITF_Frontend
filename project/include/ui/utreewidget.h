/****************************************************************************
 *
 *  Copyright (C) 2015 CUHK. All rights reserved.
 *  Author: Liping Zhang
 *
 * ****************************************************************************/

#ifndef UTREEWIDGET_H
#define UTREEWIDGET_H

#include <QTreeWidget>
/**
 * @brief The UTreeWidget class
 */
class UTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit UTreeWidget(QWidget *parent = 0);
    ~UTreeWidget();

    void set_channel_status(int, bool);

signals:
    void CreateGroup();
    void DeleteGroup();
    void CreateTask();
    void DeleteTask();
    void SelectChannel(QString, int);

private slots:
    void onCustomContextMenuRequested(const QPoint &pos);

private:
    QAction *create_group_;
    QAction *delete_group_;
    QAction *create_task_;
    QAction *delete_task_;

    QList<QAction *> channels_;
};

#endif // UTREEWIDGET_H
