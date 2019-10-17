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
        void filterMirrorListByCountry(const QItemSelection &selected, const QItemSelection &deselected);
        void httpCB(int state);
        void httpsCB(int state);
        void ipv4CB(int state);
        void ipv6CB(int state);
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
        QCheckBox *ipv4CheckBox;
        QCheckBox *ipv6CheckBox;
        QCheckBox *statusOKCheckBox;
        QCheckBox *statusKOCheckBox;

        QFileDialog *saveMirrorListDialog;
        QDialog *waitForRankingDialog;
        QPushButton *cancelRankingButton;
};

#endif
