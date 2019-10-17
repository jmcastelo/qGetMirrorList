#include "countryModel.h"

CountryModel::CountryModel(QObject *parent) : QAbstractListModel(parent)
{
    connect(&theMirrorManager, &MirrorManager::countryListReady, this, &CountryModel::setCountryList);
}

void CountryModel::getCountryList()
{
    theMirrorManager.getCountryList();
}

void CountryModel::setCountryList(QList<Country> cl)
{
    beginResetModel();
    countryList = cl;
    endResetModel();
}

int CountryModel::rowCount(const QModelIndex &parent) const
{
    return countryList.size();
}

QVariant CountryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= countryList.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        return countryList.at(index.row()).name;
    } else if (role == Qt::DecorationRole) {
        return countryList.at(index.row()).flag;
    } else {
        return QVariant();
    }
}

QVariant CountryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        return QString("Country");
    } else {
        return QStringLiteral("%1").arg(section + 1);
    }
}
