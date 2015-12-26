#ifndef UTNAME_H
#define UTNAME_H

#include "utbasewidget.h"
#include <QLineEdit>

class UTName : public UTBaseWidget
{
    Q_OBJECT
public:
    explicit UTName(QWidget *parent = 0);
    ~UTName();

    inline QString task_name() const {return p_taskname_->text().trimmed();}
signals:

public slots:

private:
    QLineEdit *p_taskname_;
};

#endif // UTNAME_H
