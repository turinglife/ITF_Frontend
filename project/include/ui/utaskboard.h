/****************************************************************************
 *
 *  Copyright (C) 2015 CUHK. All rights reserved.
 *  Author: Liping Zhang
 *
 * ****************************************************************************/

#ifndef UTASKBOARD_H
#define UTASKBOARD_H

#include <QWidget>

class UClipWidget;
class QLayout;

/**
 * @brief The UTaskBoard class
 */
class UTaskBoard : public QWidget
{
    Q_OBJECT
public:
    explicit UTaskBoard(QWidget *parent = 0);
    ~UTaskBoard();

    void set_channel(int , UClipWidget*);
    void clear_channel(int);
//    void free_channel(int);

signals:

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void ShowChannel16();
    void ShowChannel9();

private:
    void RemoveLayout();

    bool clip_max_;
//    bool available;
    QList<UClipWidget *> clipwidget_;
    QList<QLayout *> lay_;
    QHash<int, UClipWidget *> clipwidget_hash_;
};

#endif // UTASKBOARD_H
