/****************************************************************************
 *
 *  Copyright (C) 2015 CUHK. All rights reserved.
 *  Author: Liping Zhang
 *
 * ****************************************************************************/

#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TableModel(QObject *parent = 0);
    ~TableModel();

    std::map<std::string, std::string> RowElementsAt(int row);

    void set_raw_data(std::vector<std::map<std::string, std::string> >&raw_data);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

signals:

private:
    QPair<QString, QString> ElementAt(int row, int column) const;
    std::vector<std::map<std::string, std::string> > raw_data_;
};

#endif // TABLEMODEL_H
