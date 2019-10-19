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
#include <QWidget>
#include <QTableView>
#include <QListView>
#include <QGroupBox>
#include <QPushButton>
#include <QListWidget>
#include <QItemSelectionModel>
#include <QCheckBox>
#include <QFileDialog>

class MainWindow : public QWidget
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = 0);

    private slots:
        void enableWidgets();
        void affectWholeRow(const QItemSelection &selected, const QItemSelection &deselected);
        void selectMirrors(const QItemSelection &selected, const QItemSelection &deselected);
        void showAllMirrors();
        void filterByCountry(const QItemSelection &selected, const QItemSelection &deselected);
        void filterByHttp(int state);
        void filterByHttps(int state);
        void filterByRsync(int state);
        void filterByIPv4(int state);
        void filterByIPv6(int state);
        void statusOKCB(int state);
        void statusKOCB(int state);
        void rankMirrorList();
        void rankingError(QProcess::ProcessError error);
        void updateMirrorList();
        void updateFinished(int exitCode, QProcess::ExitStatus exitStatus);
        void updateMirrorListError(QProcess::ProcessError error);
        void openSaveDialog();
        void about();

    private:
        MirrorModel *mirrorModel;
        QTableView *tableView;
        QItemSelectionModel *selectionModelTableView;

        CountryModel *countryModel;
        QListView *listView;
        QItemSelectionModel *selectionModelListView;
        QStringList countryList;

        void createMirrorActionsGroubBox();
        void createMirrorTableGroupBox();

        QGroupBox *mirrorActionsGroupBox;
        QGroupBox *mirrorTableGroupBox;
        
        QPushButton *getMirrorListButton;
        QPushButton *saveMirrorListButton;
        QPushButton *rankMirrorListButton;
        QPushButton *showAllMirrorsButton;
        QPushButton *updateMirrorListButton;
        QPushButton *aboutButton;

        QCheckBox *httpCheckBox;
        QCheckBox *httpsCheckBox;
        QCheckBox *rsyncCheckBox;
        QCheckBox *ipv4CheckBox;
        QCheckBox *ipv6CheckBox;
        QCheckBox *statusOKCheckBox;
        QCheckBox *statusKOCheckBox;

        QFileDialog *saveMirrorListDialog;
        QDialog *waitForRankingDialog;
        QPushButton *cancelRankingButton;
};

#endif
