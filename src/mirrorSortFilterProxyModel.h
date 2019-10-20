#ifndef MIRRORSORTFILTERPROXYMODEL_H
#define MIRRORSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class MirrorSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
        MirrorSortFilterProxyModel(QObject *parent = 0);

        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    protected:
        bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

#endif
