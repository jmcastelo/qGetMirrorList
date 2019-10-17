#ifndef MIRRORMODEL_H
#define MIRRORMODEL_H

#include "mirrorManager.h"
#include <QAbstractTableModel>
#include <QVariant>
#include <QString>
#include <QProcess>

class MirrorModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        MirrorModel(QObject *parent = 0);
        
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;
        
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
        
        mirrorFilter filter;
        void filterMirrorList();
        void selectMirror(int index);
        void deselectMirror(int index);
        void rankMirrorList();

    signals:
        void mirrorListSet();
        void rankingMirrors();
        void rankingMirrorsEnd(int r);
        void rankingMirrorsCancelled(int r);
        void rankingMirrorsError(QProcess::ProcessError error);
        void updateMirrorListFinished(int exitCode, QProcess::ExitStatus exitStatus);
        void updateMirrorListError(QProcess::ProcessError error);

    public slots:
        void getMirrorList();
        void saveMirrorList(const QString file);
        void cancelRankingMirrorList();
        void updateMirrorList();

    private slots:
        void setMirrorList(QList<mirror> ml);
        void readRankedMirrorList(int exitCode, QProcess::ExitStatus exitStatus);

    private:
        QList<mirror> mirrorList;
        QList<mirror> selectedMirrorList;
        QProcess rankmirrors;
        QProcess updatemirrorlist;
        MirrorManager theMirrorManager;
        int countSelectedMirrors();
};

#endif
