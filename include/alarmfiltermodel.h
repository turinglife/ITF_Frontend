#ifndef ALARMFILTERMODEL_H
#define ALARMFILTERMODEL_H

#include <QSortFilterProxyModel>
#include <QDateTime>

class AlarmFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit AlarmFilterModel(QObject *parent = 0);
    ~AlarmFilterModel();

    QDateTime datetime_min() const;
    QDateTime datetime_max() const;

    void set_datetime_min(const QDateTime &datetime);
    void set_datetime_max(const QDateTime &datetime);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
    bool DatetimeInRange(const QDateTime &datetime) const;

    QDateTime datetime_min_;
    QDateTime datetime_max_;
};

#endif // ALARMFILTERMODEL_H
