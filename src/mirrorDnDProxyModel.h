// Copyright 2019 José María Castelo Ares

// This file is part of qGetMirrorList.

// qGetMirrorList is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// qGetMirrorList is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with qGetMirrorList.  If not, see <https://www.gnu.org/licenses/>.

#ifndef MIRRORDNDPROXYMODEL_H
#define MIRRORDNDPROXYMODEL_H

#include <QAbstractProxyModel>

// Inspired by QIdentityProxyModel
class MirrorDnDProxyModel : public QAbstractProxyModel
{
    Q_OBJECT

    public:
        MirrorDnDProxyModel(QObject *parent = nullptr);

        QModelIndex index(int row, int column, const QModelIndex &parent) const override;
        QModelIndex parent(const QModelIndex &index) const override;
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;
        QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
        QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
        void setSourceModel(QAbstractItemModel *sourceModel) override;

    signals:
        void sendDnDRowMapping(QList<QPair<int, int>> rowMap);

    public slots:
        void setVerticalSectionsList(int oldCount, int newCount);
        void resetVerticalSectionsList(int logicalIndex);
        void reorderVerticalSectionsList(int logicalIndex, int oldVisualIndex, int newVisualIndex);

    private slots:
        void sourceRowsAboutToBeInserted(const QModelIndex &parent, int start, int end);
        void sourceRowsInserted(const QModelIndex &parent, int start, int end);
        void sourceRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
        void sourceRowsRemoved(const QModelIndex &parent, int start, int end);
        void sourceLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint);
        void sourceLayoutChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint);
        void sourceModelAboutToBeReset();
        void sourceModelReset();

    private:
        QList<QPersistentModelIndex> layoutChangePersistentIndexes;
        QModelIndexList proxyIndexes;
        QList<int> verticalSectionsOrder;
        void setRowMapping(int oldVisualIndex, int newVisualIndex);
};

#endif
