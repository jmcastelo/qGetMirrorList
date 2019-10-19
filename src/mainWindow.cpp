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

#include "mainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    // Model and table view associated with the mirrors
    mirrorModel = new MirrorModel(this);

    tableView = new QTableView;
    tableView->setModel(mirrorModel);
    tableView->setSelectionMode(QAbstractItemView::MultiSelection);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    selectionModelTableView = tableView->selectionModel();

    // Model and list view associated with the countries
    countryModel = new CountryModel(this);

    listView = new QListView;
    listView->setModel(countryModel);
    listView->setSelectionMode(QAbstractItemView::MultiSelection);
    listView->setResizeMode(QListView::Adjust);

    selectionModelListView = listView->selectionModel();
    
    countryList.clear();

    createMirrorActionsGroubBox();
    createMirrorTableGroupBox();

    QGridLayout *mainLayout = new QGridLayout(this);

    mainLayout->addWidget(mirrorActionsGroupBox, 0, 0);
    mainLayout->addWidget(mirrorTableGroupBox, 0, 1);

    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 10);

    setLayout(mainLayout);

    setWindowTitle(tr("Arch Linux - qGetMirrorList"));
   
    QPixmap pixmap;
    pixmap.load(":/images/logos/logo-small.png");

    setWindowIcon(QIcon(pixmap));

    setGeometry(50, 50, 1100, 700);

    // File dialog
    saveMirrorListDialog = new QFileDialog(this);

    saveMirrorListDialog->setAcceptMode(QFileDialog::AcceptSave);
    saveMirrorListDialog->setFileMode(QFileDialog::AnyFile);
    saveMirrorListDialog->setViewMode(QFileDialog::Detail);

    // Wait for ranking dialog
    waitForRankingDialog = new QDialog(this, Qt::Dialog);
    
    waitForRankingDialog->setModal(true);
    waitForRankingDialog->setWindowTitle("Action");
    waitForRankingDialog->setWindowFlags((waitForRankingDialog->windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint);

    QVBoxLayout *waitLayout = new QVBoxLayout;

    QLabel *waitLabel = new QLabel("Ranking selected mirrors.\nPlease wait...");
    cancelRankingButton = new QPushButton("Cancel");
    cancelRankingButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    waitLayout->addWidget(waitLabel);
    waitLayout->addWidget(cancelRankingButton);

    waitForRankingDialog->setLayout(waitLayout);

    // Connections    
    connect(getMirrorListButton, &QPushButton::clicked, mirrorModel, &MirrorModel::getMirrorList);
    connect(getMirrorListButton, &QPushButton::clicked, countryModel, &CountryModel::getCountryList);
    connect(saveMirrorListButton, &QPushButton::clicked, this, &MainWindow::openSaveDialog);
    connect(saveMirrorListDialog, &QFileDialog::fileSelected, mirrorModel, &MirrorModel::saveMirrorList);
    connect(rankMirrorListButton, &QPushButton::clicked, this, &MainWindow::rankMirrorList);
    connect(updateMirrorListButton, &QPushButton::clicked, this, &MainWindow::updateMirrorList);
    connect(showAllMirrorsButton, &QPushButton::clicked, this, &MainWindow::showAllMirrors);
    connect(mirrorModel, &MirrorModel::mirrorListSet, this, &MainWindow::enableWidgets);
    connect(selectionModelTableView, &QItemSelectionModel::selectionChanged, this, &MainWindow::affectWholeRow);
    connect(selectionModelTableView, &QItemSelectionModel::selectionChanged, this, &MainWindow::selectMirrors);
    connect(selectionModelListView, &QItemSelectionModel::selectionChanged, this, &MainWindow::filterByCountry); 
    connect(httpCheckBox, &QCheckBox::stateChanged, this, &MainWindow::filterByHttp);
    connect(httpsCheckBox, &QCheckBox::stateChanged, this, &MainWindow::filterByHttps);
    connect(rsyncCheckBox, &QCheckBox::stateChanged, this, &MainWindow::filterByRsync);
    connect(ipv4CheckBox, &QCheckBox::stateChanged, this, &MainWindow::filterByIPv4);
    connect(ipv6CheckBox, &QCheckBox::stateChanged, this, &MainWindow::filterByIPv6);
    connect(statusOKCheckBox, &QCheckBox::stateChanged, this, &MainWindow::statusOKCB);
    connect(statusKOCheckBox, &QCheckBox::stateChanged, this, &MainWindow::statusKOCB);
    connect(mirrorModel, &MirrorModel::rankingMirrors, waitForRankingDialog, &QDialog::open);
    connect(mirrorModel, &MirrorModel::rankingMirrorsError, this, &MainWindow::rankingError);
    connect(mirrorModel, &MirrorModel::rankingMirrorsEnd, waitForRankingDialog, &QDialog::done);
    connect(mirrorModel, &MirrorModel::rankingMirrorsCancelled, waitForRankingDialog, &QDialog::done);
    connect(cancelRankingButton, &QPushButton::clicked, mirrorModel, &MirrorModel::cancelRankingMirrorList); 
    connect(mirrorModel, &MirrorModel::updateMirrorListFinished, this, &MainWindow::updateFinished);
    connect(mirrorModel, &MirrorModel::updateMirrorListError, this, &MainWindow::updateMirrorListError);
    connect(aboutButton, &QPushButton::clicked, this, &MainWindow::about);
}

void MainWindow::createMirrorActionsGroubBox()
{
    mirrorActionsGroupBox = new QGroupBox("Actions and Filters");

    QGridLayout *buttonsLayout = new QGridLayout;
    
    getMirrorListButton = new QPushButton("Get mirror list");
    saveMirrorListButton = new QPushButton("Save selected");
    rankMirrorListButton = new QPushButton("Rank selected");
    showAllMirrorsButton = new QPushButton("Show all");
    updateMirrorListButton = new QPushButton("Update");
    aboutButton = new QPushButton("About");
    
    QPixmap pixmap;
    pixmap.load(":/images/icons/key.png");
    updateMirrorListButton->setIcon(QIcon(pixmap));

    getMirrorListButton->setToolTip("Fetch all available mirrors from https://www.archlinux.org/mirrorlist/");
    saveMirrorListButton->setToolTip("Save select mirrors to chosen file");
    rankMirrorListButton->setToolTip("Rank selected mirrors by speed with 'rankmirrors' utility");
    showAllMirrorsButton->setToolTip("Show all mirrors");
    updateMirrorListButton->setToolTip("Update '/etc/pacman.d/mirrorlist' with selected mirrors as root");

    // Set buttons disabled before user gets mirror list
    saveMirrorListButton->setDisabled(true);
    rankMirrorListButton->setDisabled(true);
    showAllMirrorsButton->setDisabled(true);
    updateMirrorListButton->setDisabled(true);
    
    buttonsLayout->addWidget(getMirrorListButton, 0, 0);
    buttonsLayout->addWidget(showAllMirrorsButton, 0, 1);
    buttonsLayout->addWidget(rankMirrorListButton, 1, 0);
    buttonsLayout->addWidget(saveMirrorListButton, 1, 1); 
    buttonsLayout->addWidget(updateMirrorListButton, 2, 0);
    buttonsLayout->addWidget(aboutButton, 2, 1);

    QVBoxLayout *layout = new QVBoxLayout;

    layout->addLayout(buttonsLayout);

    QGroupBox *protocolsGroupBox = new QGroupBox("Protocols");

    httpCheckBox = new QCheckBox("http", this);
    httpsCheckBox = new QCheckBox("https", this);
    rsyncCheckBox = new QCheckBox("rsync", this);

    httpCheckBox->setCheckState(Qt::Checked);
    httpsCheckBox->setCheckState(Qt::Checked);
    rsyncCheckBox->setCheckState(Qt::Checked);

    httpCheckBox->setDisabled(true);
    httpsCheckBox->setDisabled(true);
    rsyncCheckBox->setDisabled(true);

    QHBoxLayout *protocolsLayout = new QHBoxLayout;

    protocolsLayout->addWidget(httpCheckBox);
    protocolsLayout->addWidget(httpsCheckBox);
    protocolsLayout->addWidget(rsyncCheckBox);

    protocolsGroupBox->setLayout(protocolsLayout);

    layout->addWidget(protocolsGroupBox);

    QGroupBox *IPversionGroupBox = new QGroupBox(tr("IP Version"));

    ipv4CheckBox = new QCheckBox("IPv4", this);
    ipv6CheckBox = new QCheckBox("IPv6", this);

    ipv4CheckBox->setTristate(true);
    ipv6CheckBox->setTristate(true);
    
    ipv4CheckBox->setCheckState(Qt::PartiallyChecked);
    ipv6CheckBox->setCheckState(Qt::PartiallyChecked);

    ipv4CheckBox->setDisabled(true);
    ipv6CheckBox->setDisabled(true);

    QHBoxLayout *ipLayout = new QHBoxLayout;

    ipLayout->addWidget(ipv4CheckBox);
    ipLayout->addWidget(ipv6CheckBox);

    IPversionGroupBox->setLayout(ipLayout);

    layout->addWidget(IPversionGroupBox);

    QGroupBox *statusGroupBox = new QGroupBox(tr("Status"));

    statusOKCheckBox = new QCheckBox("Updated", this);
    statusKOCheckBox = new QCheckBox("Outdated", this);

    statusOKCheckBox->setCheckState(Qt::Checked);
    statusKOCheckBox->setCheckState(Qt::Checked);

    statusOKCheckBox->setDisabled(true);
    statusKOCheckBox->setDisabled(true);

    QHBoxLayout *statusLayout = new QHBoxLayout;

    statusLayout->addWidget(statusOKCheckBox);
    statusLayout->addWidget(statusKOCheckBox);

    statusGroupBox->setLayout(statusLayout);

    layout->addWidget(statusGroupBox);

    QLabel *listViewLabel = new QLabel("Countries");
    layout->addWidget(listViewLabel);

    layout->addWidget(listView);

    mirrorActionsGroupBox->setLayout(layout);
}

void MainWindow::createMirrorTableGroupBox()
{
    mirrorTableGroupBox = new QGroupBox(tr("Mirrors"));
    
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tableView);

    mirrorTableGroupBox->setLayout(layout);
}

// Once user gets mirror list, enable buttons and checkboxes
void MainWindow::enableWidgets()
{
    if (!saveMirrorListButton->isEnabled()) {
        saveMirrorListButton->setEnabled(true);
    }
    if (!rankMirrorListButton->isEnabled()) {
        rankMirrorListButton->setEnabled(true);
    }
    if (!showAllMirrorsButton->isEnabled()) {
        showAllMirrorsButton->setEnabled(true);
    }
    if (!updateMirrorListButton->isEnabled()) {
        updateMirrorListButton->setEnabled(true);
    }
    if (!httpCheckBox->isEnabled()) {
        httpCheckBox->setEnabled(true);
    }
    if (!httpsCheckBox->isEnabled()) {
        httpsCheckBox->setEnabled(true);
    }
    if (!rsyncCheckBox->isEnabled()) {
        rsyncCheckBox->setEnabled(true);
    }
    if (!ipv4CheckBox->isEnabled()) {
        ipv4CheckBox->setEnabled(true);
    }
    if (!ipv6CheckBox->isEnabled()) {
        ipv6CheckBox->setEnabled(true);
    }
    if (!statusOKCheckBox->isEnabled()) {
        statusOKCheckBox->setEnabled(true);
    }
    if (!statusKOCheckBox->isEnabled()) {
        statusKOCheckBox->setEnabled(true);
    }
}

// When mirror table cell pressed, select whole row
void MainWindow::affectWholeRow(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndex firstColumn, lastColumn;

    QModelIndexList items = selected.indexes();

    for (const QModelIndex &index : qAsConst(items)) {
        firstColumn = mirrorModel->index(index.row(), 0, QModelIndex());
        lastColumn = mirrorModel->index(index.row(), 5, QModelIndex());
        
        QItemSelection selection(firstColumn, lastColumn);
        
        selectionModelTableView->select(selection, QItemSelectionModel::Select);
    }

    items = deselected.indexes();

    for (const QModelIndex &index : qAsConst(items)) {
        firstColumn = mirrorModel->index(index.row(), 0, QModelIndex());
        lastColumn = mirrorModel->index(index.row(), 5, QModelIndex());
        
        QItemSelection selection(firstColumn, lastColumn);
        
        selectionModelTableView->select(selection, QItemSelectionModel::Deselect);
    }        
}

void MainWindow::selectMirrors(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList items = selected.indexes();

    for (const QModelIndex &index : qAsConst(items)) {
       mirrorModel->selectMirror(index.row());
    }

    items = deselected.indexes();

    for (const QModelIndex &index : qAsConst(items)) {
        mirrorModel->deselectMirror(index.row());
    }
}

void MainWindow::filterByCountry(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList items = selected.indexes();

    for (const QModelIndex &index : qAsConst(items)) {
        mirrorModel->filter.countryList.append(index.data().toString());
    }

    items = deselected.indexes();

    for (const QModelIndex &index : qAsConst(items)) {
        mirrorModel->filter.countryList.removeOne(index.data().toString());
    }

    mirrorModel->filterMirrorList();
}

void MainWindow::filterByHttp(int state)
{
    if (state == Qt::Unchecked &&
        httpsCheckBox->checkState() == Qt::Unchecked &&
        rsyncCheckBox->checkState() == Qt::Unchecked) {
            httpCheckBox->setCheckState(Qt::Checked);
    } else {
        if (state == Qt::Checked) {
            mirrorModel->filter.protocolList.append("http");
        } else {
            mirrorModel->filter.protocolList.removeOne("http");
        }
        mirrorModel->filterMirrorList();
    }
}

void MainWindow::filterByHttps(int state)
{
    if (state == Qt::Unchecked &&
        httpCheckBox->checkState() == Qt::Unchecked &&
        rsyncCheckBox->checkState() == Qt::Unchecked) {
            httpsCheckBox->setCheckState(Qt::Checked);
    } else {
        if (state == Qt::Checked) {
            mirrorModel->filter.protocolList.append("https");
        } else {
            mirrorModel->filter.protocolList.removeOne("https");
        }
        mirrorModel->filterMirrorList();
    }
}

void MainWindow::filterByRsync(int state)
{
    if (state == Qt::Unchecked &&
        httpCheckBox->checkState() == Qt::Unchecked &&
        httpsCheckBox->checkState() == Qt::Unchecked) {
            rsyncCheckBox->setCheckState(Qt::Checked);
    } else {
        if (state == Qt::Checked) {
            mirrorModel->filter.protocolList.append("rsync");
        } else {
            mirrorModel->filter.protocolList.removeOne("rsync");
        }
        mirrorModel->filterMirrorList();
    }
}

void MainWindow::filterByIPv4(int state)
{
    if (state == Qt::Unchecked && ipv6CheckBox->checkState() == Qt::Unchecked) {
        ipv4CheckBox->setCheckState(Qt::Checked);
    } else {
        mirrorModel->filter.ipv4 = state;
        mirrorModel->filterMirrorList();
    }
}

void MainWindow::filterByIPv6(int state)
{
    if (state == Qt::Unchecked && ipv4CheckBox->checkState() == Qt::Unchecked) {
        ipv6CheckBox->setCheckState(Qt::Checked);
    } else {
        mirrorModel->filter.ipv6 = state;
        mirrorModel->filterMirrorList();
    }
}

void MainWindow::statusOKCB(int state)
{
    if (state == Qt::Unchecked && statusKOCheckBox->checkState() == Qt::Unchecked) {
        statusOKCheckBox->setCheckState(Qt::Checked);
    } else {
        if (state == Qt::Checked) {
            mirrorModel->filter.statusOK = true;
        } else {
            mirrorModel->filter.statusOK = false;
        }
        mirrorModel->filterMirrorList();
    }
}

void MainWindow::statusKOCB(int state)
{
    if (state == Qt::Unchecked && statusOKCheckBox->checkState() == Qt::Unchecked) {
        statusKOCheckBox->setCheckState(Qt::Checked);
    } else {
        if (state == Qt::Checked) {
            mirrorModel->filter.statusKO = true;
        } else {
            mirrorModel->filter.statusKO = false;
        }
        mirrorModel->filterMirrorList();
    }
}

void MainWindow::showAllMirrors()
{
    // Set least restrictive filters and filter mirror list
    mirrorModel->filter.countryList.clear();
    mirrorModel->filter.protocolList.clear();
    mirrorModel->filter.protocolList.append("http");
    mirrorModel->filter.protocolList.append("https");
    mirrorModel->filter.ipv4 = 1;
    mirrorModel->filter.ipv6 = 1;
    mirrorModel->filter.statusOK = true;
    mirrorModel->filter.statusKO = true;

    mirrorModel->filterMirrorList();

    // Set filter checkboxes accordingly
    httpCheckBox->setCheckState(Qt::Checked);
    httpsCheckBox->setCheckState(Qt::Checked);
    ipv4CheckBox->setCheckState(Qt::PartiallyChecked);
    ipv6CheckBox->setCheckState(Qt::PartiallyChecked);
    statusOKCheckBox->setCheckState(Qt::Checked);
    statusKOCheckBox->setCheckState(Qt::Checked);

    selectionModelListView->clearSelection();
}

void MainWindow::openSaveDialog()
{
    if(!selectionModelTableView->hasSelection()) {
        QMessageBox::critical(this, tr("Error"), tr("No mirrors selected.\nPlease select at least one mirror."));
    } else {
        saveMirrorListDialog->open();
    }
}

void MainWindow::rankMirrorList()
{
    QModelIndexList indexList = selectionModelTableView->selectedRows(0);

    if(indexList.size() < 2) {
        QMessageBox::critical(this, tr("Error"), tr("Please select at least two mirrors."));
    } else {
        mirrorModel->rankMirrorList();
    }
}

void MainWindow::rankingError(QProcess::ProcessError error)
{
    if (error == QProcess::FailedToStart) {
        QMessageBox::critical(this, tr("Error"), tr("Binary 'rankmirrors' nor found!"));
    } else if (error == QProcess::Crashed) {
        QMessageBox::critical(this, tr("Error"), tr("Ranking mirrors cancelled."));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Error ranking mirrors."));
    }
}

void MainWindow::updateMirrorList()
{
    if(!selectionModelTableView->hasSelection()) {
        QMessageBox::critical(this, tr("Error"), tr("No mirrors selected.\nPlease select at least one mirror."));
    } else {
        mirrorModel->updateMirrorList();
    }
    
}

void MainWindow::updateFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitCode == 0) {
        QMessageBox::information(this, tr("Action"), tr("'/etc/pacman.d/mirrorlist' successfully updated."));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Update failure."));
    }
}

void MainWindow::updateMirrorListError(QProcess::ProcessError error)
{
    if (error == QProcess::FailedToStart) {
        QMessageBox::critical(this, tr("Error"), tr("Binary 'pkexec' nor found!"));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Error updating '/etc/pacman.d/mirrorlist'"));
    }
}

void MainWindow::about()
{
    QMessageBox *aboutBox = new QMessageBox(this);
    
    aboutBox->setWindowTitle("About");
    
    aboutBox->setIconPixmap(QPixmap(":/images/logos/logo-big.png"));
    
    QStringList lines;
    lines.append("Arch Linux - qGetMirrorList 0.1\n");
    lines.append("Get and manipulate Arch Linux pacman mirror list.\n");
    lines.append("Based on the online pacman Mirrorlist Generator:");
    lines.append(" https://www.archlinux.org/mirrorlist/\n");
    lines.append("Dependencies:");
    lines.append(" Qt >= 5.13.1");
    lines.append(" pacman-contrib >= 1.1.0-1");
    lines.append(" polkit >= 0.116-2");

    QString text = lines.join("\n");

    aboutBox->setText(text);

    aboutBox->setInformativeText("Copyright 2019 Jose Maria Castelo\nLicense: GPLv3");
    
    aboutBox->exec();
}
