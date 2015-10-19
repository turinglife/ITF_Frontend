/****************************************************************************
 *
 *  Copyright (C) 2015 CUHK. All rights reserved.
 *  Author: Liping Zhang
 *
 * ****************************************************************************/

#ifndef UCAMERAMANAGER_H
#define UCAMERAMANAGER_H

#include <QWidget>

class QTableView;
class QLineEdit;
class QPushButton;
class QLabel;

/**
 * @brief The UCameraManager class
 */
class UCameraManager : public QWidget
{
    Q_OBJECT
public:
    explicit UCameraManager(QWidget *parent = 0);
    ~UCameraManager();

    void Clear();

    QTableView *tableview_;
    QLineEdit *cameraname_;
    QLineEdit *address_;
    QLineEdit *host_;
    QLineEdit *port_;
    QLineEdit *username_;
    QLineEdit *password_;

    QLabel *lab_icon_;
    QLabel *lab_title_;
    QPushButton *btn_menu_close_;
    QPushButton *btn_select_;
    QPushButton *btn_add_;
    QPushButton *btn_delete_;

public slots:

private:

};

#endif // UCAMERAMANAGER_H
