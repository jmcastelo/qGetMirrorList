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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mirrorModel.h"
#include "countryModel.h"
#include "mirrorSortFilterProxyModel.h"
#include <QWidget>
#include <QTableView>
#include <QListView>
#include <QGroupBox>
#include <QPushButton>
#include <QListWidget>
#include <QItemSelectionModel>
#include <QCheckBox>
#include <QFileDialog>
#include <QProgressBar>

class MainWindow : public QWidget
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = 0);

    private slots:
        void enableWidgets();
        
        void selectMirrors(const QItemSelection &selected, const QItemSelection &deselected);
        void selectAllMirrors(bool state);
        void showAllMirrors();
        void uncheckCornerButton(int r);

        void setUrlColumn(int state);
        void setCountryColumn(int state);
        void setProtocolColumn(int state);
        void setCompletionColumn(int state);
        void setScoreColumn(int state);
        void setSpeedColumn(int state);
        void setSyncColumn(int state);
        void setDelayColumn(int state);
        void setIPv4Column(int state);
        void setIPv6Column(int state);
        void setActiveColumn(int state);
        void setIsosColumn(int state);
        
        void setTableGroupTitle();

        void filterByCountry(const QItemSelection &selected, const QItemSelection &deselected);
        void filterByHttp(int state);
        void filterByHttps(int state);
        void filterByRsync(int state);
        void filterByActive(int state);
        void filterByIsos(int state);
        void filterByIPv4(int state);
        void filterByIPv6(int state);

        void saveMirrorList(const QString file);
        
        void rankMirrorList();
        
        void updateMirrorList();
        void updateFinished(int exitCode, QProcess::ExitStatus exitStatus);
        void updateMirrorListError(QProcess::ProcessError error);
        void showHttpDialog();
        
        void openSaveDialog();
        
        void about();
        
        void mirrorListNetworkError(QNetworkReply::NetworkError error);

    private:
        MirrorModel *mirrorModel;
        QTableView *tableView;
        QItemSelectionModel *selectionModelTableView;
        MirrorSortFilterProxyModel *mirrorProxyModel;

        CountryModel *countryModel;
        QListView *listView;
        QItemSelectionModel *selectionModelListView;
        QStringList countryList;

        void createMirrorActionsGroubBox();
        void createMirrorTableGroupBox();
        void createMirrorColumnSelectGroupBox();

        QPushButton *cornerButton;
        
        QGroupBox *mirrorActionsGroupBox;
        QGroupBox *mirrorTableGroupBox;
        QGroupBox *mirrorColumnSelectGroupBox;
        
        QPushButton *getMirrorListButton;
        QPushButton *saveMirrorListButton;
        QPushButton *rankMirrorListButton;
        QPushButton *showAllMirrorsButton;
        QPushButton *updateMirrorListButton;
        QPushButton *aboutButton;

        QCheckBox *httpCheckBox;
        QCheckBox *httpsCheckBox;
        QCheckBox *rsyncCheckBox;
        QCheckBox *activeCheckBox;
        QCheckBox *isosCheckBox;
        QCheckBox *ipv4CheckBox;
        QCheckBox *ipv6CheckBox;

        QCheckBox *urlColCheckBox;
        QCheckBox *countryColCheckBox;
        QCheckBox *protocolColChechBox;
        QCheckBox *completionColChechBox;
        QCheckBox *scoreColCheckBox;
        QCheckBox *speedColCheckBox;
        QCheckBox *syncColCheckBox;
        QCheckBox *delayColCheckBox;
        QCheckBox *ipv4ColCheckBox;
        QCheckBox *ipv6ColCheckBox;
        QCheckBox *activeColCheckBox;
        QCheckBox *isosColCheckBox;

        QFileDialog *saveMirrorListDialog;
        QDialog *waitForRankingDialog;
        QProgressBar *rankingProgressBar;
};

#endif
