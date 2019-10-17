#ifndef COUNTRYMODEL_H
#define COUNTRYMODEL_H

#include "mirrorManager.h"
#include <QAbstractListModel>
#include <QVariant>
#include <QStringList>

class CountryModel : public QAbstractListModel
{
    Q_OBJECT

    public:
        CountryModel(QObject *parent = 0);
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role) const override;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    public slots:
        void getCountryList();

    private slots:
        void setCountryList(QList<Country> cl);

    private:
        QList<Country> countryList;
        MirrorManager theMirrorManager;
};

#endif
