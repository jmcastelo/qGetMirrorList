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

#include "mirrorDnDProxyModel.h"
#include "mirrorSortFilterProxyModel.h"

MirrorDnDProxyModel::MirrorDnDProxyModel(QObject *parent) : QAbstractProxyModel(parent) {}

QModelIndex MirrorDnDProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    const QModelIndex sourceParent = mapToSource(parent);
    const QModelIndex sourceIndex = sourceModel()->index(row, column, sourceParent);
    return mapFromSource(sourceIndex);
}

QModelIndex MirrorDnDProxyModel::parent(const QModelIndex &child) const
{
    Q_ASSERT(child.isValid() ? child.model() == this : true);
    const QModelIndex sourceIndex = mapToSource(child);
    const QModelIndex sourceParent = sourceIndex.parent();
    return mapFromSource(sourceParent);
}

int MirrorDnDProxyModel::rowCount(const QModelIndex &parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    return sourceModel()->rowCount(mapToSource(parent));
}

int MirrorDnDProxyModel::columnCount(const QModelIndex &parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    return sourceModel()->columnCount(mapToSource(parent));
}

QModelIndex MirrorDnDProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    MirrorSortFilterProxyModel *mirrorProxyModel = qobject_cast<MirrorSortFilterProxyModel*>(sourceModel());

    if (!mirrorProxyModel || !proxyIndex.isValid()) {
        return QModelIndex();
    }

    // Mapping of rows according to section order
    return mirrorProxyModel->createIndex(verticalSectionsOrder.at(proxyIndex.row()), proxyIndex.column(), proxyIndex.internalPointer());
}

QModelIndex MirrorDnDProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (sourceIndex.isValid()) {
        return createIndex(sourceIndex.row(), sourceIndex.column(), sourceIndex.internalPointer());
    } else {
        return QModelIndex();
    }
}

QVariant MirrorDnDProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return sourceModel()->headerData(section, orientation, role);
}

void MirrorDnDProxyModel::setSourceModel(QAbstractItemModel *newSourceModel)
{
    beginResetModel();

    if (sourceModel()) {
        disconnect(sourceModel(), &QAbstractItemModel::rowsAboutToBeInserted, this, &MirrorDnDProxyModel::sourceRowsAboutToBeInserted);
        disconnect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &MirrorDnDProxyModel::sourceRowsInserted);
        disconnect(sourceModel(), &QAbstractItemModel::rowsAboutToBeRemoved, this, &MirrorDnDProxyModel::sourceRowsAboutToBeRemoved);
        disconnect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &MirrorDnDProxyModel::sourceRowsRemoved);
        disconnect(sourceModel(), &QAbstractItemModel::modelAboutToBeReset, this, &MirrorDnDProxyModel::sourceModelAboutToBeReset);
        disconnect(sourceModel(), &QAbstractItemModel::modelReset, this, &MirrorDnDProxyModel::sourceModelReset);
        disconnect(sourceModel(), &QAbstractItemModel::layoutAboutToBeChanged, this, &MirrorDnDProxyModel::sourceLayoutAboutToBeChanged);
        disconnect(sourceModel(), &QAbstractItemModel::layoutChanged, this, &MirrorDnDProxyModel::sourceLayoutChanged);
    }

    QAbstractProxyModel::setSourceModel(newSourceModel);

    if (sourceModel()) {
        connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeInserted, this, &MirrorDnDProxyModel::sourceRowsAboutToBeInserted);
        connect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &MirrorDnDProxyModel::sourceRowsInserted);
        connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeRemoved, this, &MirrorDnDProxyModel::sourceRowsAboutToBeRemoved);
        connect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &MirrorDnDProxyModel::sourceRowsRemoved);
        connect(sourceModel(), &QAbstractItemModel::modelAboutToBeReset, this, &MirrorDnDProxyModel::sourceModelAboutToBeReset);
        connect(sourceModel(), &QAbstractItemModel::modelReset, this, &MirrorDnDProxyModel::sourceModelReset);
        connect(sourceModel(), &QAbstractItemModel::layoutAboutToBeChanged, this, &MirrorDnDProxyModel::sourceLayoutAboutToBeChanged);
        connect(sourceModel(), &QAbstractItemModel::layoutChanged, this, &MirrorDnDProxyModel::sourceLayoutChanged);
    }

    endResetModel();
}

void MirrorDnDProxyModel::sourceLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint)
{
    QList<QPersistentModelIndex> parents;
    parents.reserve(sourceParents.size());
    for (const QPersistentModelIndex &parent : sourceParents) {
        if (!parent.isValid()) {
            parents << QPersistentModelIndex();
            continue;
        }
        const QModelIndex mappedParent = mapFromSource(parent);
        Q_ASSERT(mappedParent.isValid());
        parents << mappedParent;
    }
    emit layoutAboutToBeChanged(parents, hint);
    const auto proxyPersistentIndexes = persistentIndexList();
    for (const QPersistentModelIndex &proxyPersistentIndex : proxyPersistentIndexes) {
        proxyIndexes << proxyPersistentIndex;
        Q_ASSERT(proxyPersistentIndex.isValid());
        const QPersistentModelIndex srcPersistentIndex = mapToSource(proxyPersistentIndex);
        Q_ASSERT(srcPersistentIndex.isValid());
        layoutChangePersistentIndexes << srcPersistentIndex;
    }
}
void MirrorDnDProxyModel::sourceLayoutChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint)
{
    for (int i = 0; i < proxyIndexes.size(); ++i) {
        changePersistentIndex(proxyIndexes.at(i), mapFromSource(layoutChangePersistentIndexes.at(i)));
    }
    layoutChangePersistentIndexes.clear();
    proxyIndexes.clear();
    QList<QPersistentModelIndex> parents;
    parents.reserve(sourceParents.size());
    for (const QPersistentModelIndex &parent : sourceParents) {
        if (!parent.isValid()) {
            parents << QPersistentModelIndex();
            continue;
        }
        const QModelIndex mappedParent = mapFromSource(parent);
        Q_ASSERT(mappedParent.isValid());
        parents << mappedParent;
    }
    emit layoutChanged(parents, hint);
}

void MirrorDnDProxyModel::sourceModelAboutToBeReset()
{
    beginResetModel();
}

void MirrorDnDProxyModel::sourceModelReset()
{
    endResetModel();
}

void MirrorDnDProxyModel::sourceRowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    Q_ASSERT(parent.isValid() ? parent.model() == sourceModel() : true);

    beginInsertRows(mapFromSource(parent), start, end);
}

void MirrorDnDProxyModel::sourceRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    Q_ASSERT(parent.isValid() ? parent.model() == sourceModel() : true);

    beginRemoveRows(mapFromSource(parent), start, end);
}

void MirrorDnDProxyModel::sourceRowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_ASSERT(parent.isValid() ? parent.model() == sourceModel() : true);

    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)
    endInsertRows();
}

void MirrorDnDProxyModel::sourceRowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_ASSERT(parent.isValid() ? parent.model() == sourceModel() : true);

    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)
    endRemoveRows();
}

// Called when mirror table vertical sections change in number
void MirrorDnDProxyModel::setVerticalSectionsList(int oldCount, int newCount)
{
    Q_UNUSED(oldCount)

    verticalSectionsOrder.clear();

    // Identity mapping
    for (int i = 0; i < newCount; i++) {
        verticalSectionsOrder.append(i);
    }
}

// Called when mirror table horizontal section clicked (when sorting table)
void MirrorDnDProxyModel::resetVerticalSectionsList(int logicalIndex)
{
    Q_UNUSED(logicalIndex)

    // Identity mapping: not to interfere with table sorting
    for (int i = 0; i < verticalSectionsOrder.size(); i++) {
        verticalSectionsOrder[i] = i;
    }
}

// Called when mirror table vertical sections reordered (drag & drop)
void MirrorDnDProxyModel::reorderVerticalSectionsList(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    Q_UNUSED(logicalIndex)

    if (newVisualIndex > oldVisualIndex) {
        for (int i = oldVisualIndex; i < newVisualIndex; i++) {
            verticalSectionsOrder.swapItemsAt(i, i + 1);
        }
    } else if (newVisualIndex < oldVisualIndex) {
        for (int i = oldVisualIndex; i > newVisualIndex; i--) {
            verticalSectionsOrder.swapItemsAt(i, i - 1);
        }
    }

    setRowMapping(oldVisualIndex, newVisualIndex);
}

void MirrorDnDProxyModel::setRowMapping(int oldVisualIndex, int newVisualIndex)
{
    QList<QPair<int, int>> rowMap;
    QPair<int, int> pair;

    if (newVisualIndex > oldVisualIndex) {
        for (int i = oldVisualIndex; i < newVisualIndex; i++) {
            pair.first = i;
            pair.second = i + 1;
            rowMap.append(pair);
        }
    } else if (newVisualIndex < oldVisualIndex) {
        for (int i = oldVisualIndex; i > newVisualIndex; i--) {
            pair.first = i;
            pair.second = i - 1;
            rowMap.append(pair);
        }
    }

    pair.first = newVisualIndex;
    pair.second = oldVisualIndex;
    rowMap.append(pair);

    emit sendDnDRowMapping(rowMap);
}
