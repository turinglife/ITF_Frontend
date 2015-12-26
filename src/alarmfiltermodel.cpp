#include "alarmfiltermodel.h"

AlarmFilterModel::AlarmFilterModel(QObject *parent) : QSortFilterProxyModel(parent)
{

}

AlarmFilterModel::~AlarmFilterModel()
{

}

QDateTime AlarmFilterModel::datetime_min() const
{
    return datetime_min_;
}

QDateTime AlarmFilterModel::datetime_max() const
{
    return datetime_max_;
}

void AlarmFilterModel::set_datetime_min(const QDateTime &datetime)
{
    datetime_min_ = datetime;
    invalidateFilter();
}

void AlarmFilterModel::set_datetime_max(const QDateTime &datetime)
{
    datetime_max_ = datetime;
    invalidateFilter();
}

bool AlarmFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex index0 = sourceModel()->index(source_row, 0, source_parent);
    QModelIndex index1 = sourceModel()->index(source_row, 1, source_parent);
    QModelIndex index2 = sourceModel()->index(source_row, 2, source_parent);
    QModelIndex index3 = sourceModel()->index(source_row, 4, source_parent);

//    return (sourceModel()->data(index0).toString().contains(filterRegExp())
//            || sourceModel()->data(index1).toString().contains(filterRegExp()))
//            && DatetimeInRange(sourceModel()->data(index2).toDateTime());
    return (sourceModel()->data(index0).toString().contains(filterRegExp())
            || sourceModel()->data(index2).toString().contains(filterRegExp())
            || sourceModel()->data(index3).toString().contains(filterRegExp()))
            && DatetimeInRange(sourceModel()->data(index1).toDateTime());
}

bool AlarmFilterModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant data_left = sourceModel()->data(left);
    QVariant data_right = sourceModel()->data(right);
    if (data_left.type() == QVariant::DateTime) {
        return data_left.toDateTime() < data_right.toDateTime();
    } else {
        QString str_left = data_left.toString();
        QString str_right = data_right.toString();
        return (QString::localeAwareCompare(str_left, str_right) < 0);
    }
}

bool AlarmFilterModel::DatetimeInRange(const QDateTime &datetime) const
{
    return (!datetime_min_.isValid() || datetime >= datetime_min_)
            && (!datetime_max_.isValid() || datetime <= datetime_max_);
}

