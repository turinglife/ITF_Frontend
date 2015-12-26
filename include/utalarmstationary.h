#ifndef UTALARMSTATIONARY_H
#define UTALARMSTATIONARY_H

#include "utalarmstrategy.h"

class UTAlarmStationary : public UTAlarmStrategy
{
    Q_OBJECT
public:
    explicit UTAlarmStationary(QWidget *parent = 0);
    ~UTAlarmStationary();

signals:

public slots:
};

#endif // UTALARMSTATIONARY_H
