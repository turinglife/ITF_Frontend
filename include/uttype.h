#ifndef UTTYPE_H
#define UTTYPE_H

#include "utbasewidget.h"
#include <QComboBox>

class UTType : public UTBaseWidget
{
    Q_OBJECT
public:
    explicit UTType(QWidget *parent = 0);
    ~UTType();

    inline QString task_type() const {return p_tasktype_->currentText().trimmed();}
signals:

public slots:

private:
    QComboBox *p_tasktype_;
};

#endif // UTTYPE_H
