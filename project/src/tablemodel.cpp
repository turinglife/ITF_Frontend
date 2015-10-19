#include "tablemodel.h"

TableModel::TableModel(QObject *parent) : QAbstractTableModel(parent)
{

}

TableModel::~TableModel()
{

}

std::map<std::string, std::string> TableModel::RowElementsAt(int row)
{
    return raw_data_[row];
}

void TableModel::set_raw_data(std::vector<std::map<std::string, std::string> > &raw_data)
{
    beginResetModel();
    raw_data_.clear();
    raw_data_ = raw_data;
    endResetModel();
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    return raw_data_.size();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    if (raw_data_.empty()) {
        return 0;
    }

    std::map<std::string, std::string> mymap = raw_data_[raw_data_.size()-1];

    return mymap.size();
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::TextAlignmentRole) {
//        return int(Qt::AlignRight | Qt::AlignVCenter);
        return int(Qt::AlignCenter);

    } else if (role == Qt::DisplayRole) {

        return ElementAt(index.row(), index.column()).second;
    }

    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return ElementAt(0, section).first;
    } else if (role == Qt::DisplayRole && orientation == Qt::Vertical) {
        return section+1;
    }

    return QVariant();
}

QPair<QString, QString> TableModel::ElementAt(int row, int column) const
{
    QPair<QString, QString> mypair;
    std::map<std::string, std::string> mymap = raw_data_[row];

    for (std::map<std::string, std::string>::iterator it=mymap.begin(); it!=mymap.end(); ++it) {
        if (column-- == 0) {
            mypair.first = QString::fromStdString(it->first);
            mypair.second = QString::fromStdString(it->second);

            return mypair;
        }
    }
}

