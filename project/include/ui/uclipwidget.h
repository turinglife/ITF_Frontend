/****************************************************************************
 *
 *  Copyright (C) 2015 CUHK. All rights reserved.
 *  Author: Liping Zhang
 *
 * ****************************************************************************/

#ifndef UCLIPWIDGET_H
#define UCLIPWIDGET_H

#include <QLabel>

/**
 * @brief The UClipWidget class
 */
class UClipWidget : public QLabel
{
    Q_OBJECT
public:
    explicit UClipWidget(QWidget *parent = 0);
    ~UClipWidget();

    void set_curr_type(const std::string &type);
    void set_curr_name(const std::string &name);
    void set_start_enabled(bool flag);
    void set_stop_enabled(bool flag);
    void set_clear_enabled(bool flag);
    void set_countingsetting_enabled(bool flag);

signals:
    void Command(QString, QString);
    void RemoveTaskFromBoard();
    void CountingSetting();
    void ShowAnalyzeBoard();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QEvent *);

private slots:
    void onCustomContextMenuRequested(const QPoint &pos);

private:
    QAction *start_;
    QAction *stop_;
    QAction *clear_;
    QAction *countingsetting_;

    QWidget *control_;

    std::string curr_type_;
    std::string curr_name_;
};

#endif // UCLIPWIDGET_H
