#ifndef UTBASEWIDGET_H
#define UTBASEWIDGET_H

#include <QWidget>
#include <QLabel>

class UTBaseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UTBaseWidget(QWidget *parent = 0);
    ~UTBaseWidget();

    inline void set_guide(QString qstr) {p_guide_->setText(qstr);}
    inline void set_warning(QString qstr) {p_warning_->setText(qstr);}

protected:
    QLabel *p_warning_;
    QLabel *p_guide_;

signals:

public slots:
};

#endif // UTBASEWIDGET_H
