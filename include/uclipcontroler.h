#ifndef UCLIPCONTROLER_H
#define UCLIPCONTROLER_H

#include <QWidget>
#include <QPushButton>

class UClipControler : public QWidget
{
    Q_OBJECT
public:
    explicit UClipControler(QWidget *parent = 0);
    ~UClipControler();

    QPushButton *p_clear_;
    QPushButton *p_start_analyze_;
    QPushButton *p_stop_analyze_;
    QPushButton *p_show_analysis_;
    QPushButton *p_setting_;
};

#endif // UCLIPCONTROLER_H
