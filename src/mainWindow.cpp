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
#include "columns.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    // Model associated with the mirrors
    mirrorModel = new MirrorModel(this);

    // Mirror sort and filter proxy model
    mirrorProxyModel = new MirrorSortFilterProxyModel(this);
    mirrorProxyModel->setSourceModel(mirrorModel);
    mirrorProxyModel->setDynamicSortFilter(true);

    // Table view associated with mirror model through proxy
    tableView = new QTableView;
    tableView->setModel(mirrorProxyModel);
    tableView->setSelectionMode(QAbstractItemView::MultiSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tableView->horizontalHeader()->setSectionsMovable(true);
    tableView->setAlternatingRowColors(true);
    tableView->setShowGrid(false);
    tableView->setSortingEnabled(true);
    tableView->sortByColumn(Columns::country, Qt::AscendingOrder);

    tableView->setColumnHidden(Columns::ipv4, true);
    tableView->setColumnHidden(Columns::active, true);
    tableView->setColumnHidden(Columns::isos, true);

    cornerButton = (QPushButton*)tableView->findChild<QAbstractButton *>();
    cornerButton->setCheckable(true);

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
    createMirrorColumnSelectGroupBox();

    QVBoxLayout *vLayout = new QVBoxLayout;

    vLayout->addWidget(mirrorColumnSelectGroupBox);
    vLayout->addWidget(mirrorTableGroupBox);

    QGridLayout *mainLayout = new QGridLayout(this);

    mainLayout->addWidget(mirrorActionsGroupBox, 0, 0);
    mainLayout->addLayout(vLayout, 0, 1);

    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 10);

    setLayout(mainLayout);

    setWindowTitle(tr("Arch Linux - qGetMirrorList"));
   
    QPixmap pixmap;
    pixmap.load(":/images/logos/logo-small.png");

    setWindowIcon(QIcon(pixmap));

    setGeometry(50, 50, 1400, 700);

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

    waitLayout->addWidget(waitLabel);

    waitForRankingDialog->setLayout(waitLayout);

    // Connections: actions 
    connect(getMirrorListButton, &QPushButton::clicked, mirrorModel, &MirrorModel::getMirrorList);
    connect(getMirrorListButton, &QPushButton::clicked, countryModel, &CountryModel::getCountryList);
    connect(saveMirrorListButton, &QPushButton::clicked, this, &MainWindow::openSaveDialog);
    connect(saveMirrorListDialog, &QFileDialog::fileSelected, this, &MainWindow::saveMirrorList);
    connect(rankMirrorListButton, &QPushButton::clicked, this, &MainWindow::rankMirrorList);
    connect(updateMirrorListButton, &QPushButton::clicked, this, &MainWindow::updateMirrorList);
    connect(showAllMirrorsButton, &QPushButton::clicked, this, &MainWindow::showAllMirrors);
    // Connections: various
    connect(cornerButton, &QPushButton::clicked, this, &MainWindow::selectAllMirrors);
    connect(mirrorModel, &MirrorModel::mirrorListSet, this, &MainWindow::enableWidgets);
    connect(selectionModelTableView, &QItemSelectionModel::selectionChanged, this, &MainWindow::selectMirrors);
    // Connections: columns
    connect(urlColCheckBox, &QCheckBox::stateChanged, this, &MainWindow::setUrlColumn);
    connect(countryColCheckBox, &QCheckBox::stateChanged, this, &MainWindow::setCountryColumn);
    connect(protocolColChechBox, &QCheckBox::stateChanged, this, &MainWindow::setProtocolColumn);
    connect(completionColChechBox, &QCheckBox::stateChanged, this, &MainWindow::setCompletionColumn);
    connect(scoreColCheckBox, &QCheckBox::stateChanged, this, &MainWindow::setScoreColumn);
    connect(speedColCheckBox, &QCheckBox::stateChanged, this, &MainWindow::setSpeedColumn);
    connect(syncColCheckBox, &QCheckBox::stateChanged, this, &MainWindow::setSyncColumn);
    connect(delayColCheckBox, &QCheckBox::stateChanged, this, &MainWindow::setDelayColumn);
    connect(ipv4ColCheckBox, &QCheckBox::stateChanged, this, &MainWindow::setIPv4Column);
    connect(ipv6ColCheckBox, &QCheckBox::stateChanged, this, &MainWindow::setIPv6Column);
    connect(activeColCheckBox, &QCheckBox::stateChanged, this, &MainWindow::setActiveColumn);
    connect(isosColCheckBox, &QCheckBox::stateChanged, this, &MainWindow::setIsosColumn);
    // Connections: filters
    connect(selectionModelListView, &QItemSelectionModel::selectionChanged, this, &MainWindow::filterByCountry); 
    connect(httpCheckBox, &QCheckBox::stateChanged, this, &MainWindow::filterByHttp);
    connect(httpsCheckBox, &QCheckBox::stateChanged, this, &MainWindow::filterByHttps);
    connect(rsyncCheckBox, &QCheckBox::stateChanged, this, &MainWindow::filterByRsync);
    connect(activeCheckBox, &QCheckBox::stateChanged, this, &MainWindow::filterByActive);
    connect(isosCheckBox, &QCheckBox::stateChanged, this, &MainWindow::filterByIsos);
    connect(ipv4CheckBox, &QCheckBox::stateChanged, this, &MainWindow::filterByIPv4);
    connect(ipv6CheckBox, &QCheckBox::stateChanged, this, &MainWindow::filterByIPv6);
    // Connections: ranking
    connect(mirrorModel, &MirrorModel::rankingMirrorsStarted, waitForRankingDialog, &QDialog::open);
    connect(mirrorModel, &MirrorModel::rankingMirrorsFinished, waitForRankingDialog, &QDialog::done);
    connect(mirrorModel, &MirrorModel::rankingMirrorsFinished, this, &MainWindow::uncheckCornerButton);
    // Connections: update & about
    connect(mirrorModel, &MirrorModel::updateMirrorListFinished, this, &MainWindow::updateFinished);
    connect(mirrorModel, &MirrorModel::updateMirrorListError, this, &MainWindow::updateMirrorListError);
    connect(mirrorModel, &MirrorModel::noHttpMirrorSelected, this, &MainWindow::showHttpDialog);
    connect(aboutButton, &QPushButton::clicked, this, &MainWindow::about);
}

void MainWindow::createMirrorActionsGroubBox()
{
    mirrorActionsGroupBox = new QGroupBox("Actions and Filters");

    // Action buttons
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

    getMirrorListButton->setToolTip("Fetch all available mirrors from the Internet"); 
    saveMirrorListButton->setToolTip("Save select mirrors to chosen file");
    rankMirrorListButton->setToolTip("Rank selected mirrors by speed");
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

    // Protocol filters
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

    // IP version filters
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

    // Other filters: active, isos
    QGroupBox *otherGroupBox = new QGroupBox("Other filters");

    activeCheckBox = new QCheckBox("Active", this);
    isosCheckBox = new QCheckBox("ISOs hosted", this);

    activeCheckBox->setTristate(true);
    isosCheckBox->setTristate(true);

    activeCheckBox->setCheckState(Qt::PartiallyChecked);
    isosCheckBox->setCheckState(Qt::PartiallyChecked);

    activeCheckBox->setDisabled(true);
    isosCheckBox->setDisabled(true);

    QHBoxLayout *otherLayout = new QHBoxLayout;

    otherLayout->addWidget(activeCheckBox);
    otherLayout->addWidget(isosCheckBox);

    otherGroupBox->setLayout(otherLayout);

    layout->addWidget(otherGroupBox);

    // Country list
    QLabel *listViewLabel = new QLabel("Countries");
    layout->addWidget(listViewLabel);

    layout->addWidget(listView);

    mirrorActionsGroupBox->setLayout(layout);
}

void MainWindow::createMirrorTableGroupBox()
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tableView);
    
    mirrorTableGroupBox = new QGroupBox("Mirrors");
    mirrorTableGroupBox->setLayout(layout);
}

void MainWindow::createMirrorColumnSelectGroupBox()
{
    urlColCheckBox = new QCheckBox("URL", this);
    countryColCheckBox = new QCheckBox("Country", this);
    protocolColChechBox = new QCheckBox("Protocol", this);
    completionColChechBox = new QCheckBox("Completion %", this);
    scoreColCheckBox = new QCheckBox("Score", this);
    speedColCheckBox = new QCheckBox("Speed (KiB/s)", this);
    syncColCheckBox = new QCheckBox("Last sync", this);
    delayColCheckBox = new QCheckBox("Delay (hh:mm)", this);
    ipv4ColCheckBox = new QCheckBox("IPv4", this);
    ipv6ColCheckBox = new QCheckBox("IPv6", this);
    activeColCheckBox = new QCheckBox("Active", this);
    isosColCheckBox = new QCheckBox("ISOs", this);

    urlColCheckBox->setCheckState(Qt::Checked);
    countryColCheckBox->setCheckState(Qt::Checked);
    protocolColChechBox->setCheckState(Qt::Checked);
    completionColChechBox->setCheckState(Qt::Checked);
    scoreColCheckBox->setCheckState(Qt::Checked);
    speedColCheckBox->setCheckState(Qt::Checked);
    syncColCheckBox->setCheckState(Qt::Checked);
    delayColCheckBox->setCheckState(Qt::Checked);
    ipv4ColCheckBox->setCheckState(Qt::Unchecked);
    ipv6ColCheckBox->setCheckState(Qt::Checked);
    activeColCheckBox->setCheckState(Qt::Unchecked);
    isosColCheckBox->setCheckState(Qt::Unchecked);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(urlColCheckBox);
    layout->addWidget(countryColCheckBox);
    layout->addWidget(protocolColChechBox);
    layout->addWidget(completionColChechBox);
    layout->addWidget(scoreColCheckBox);
    layout->addWidget(speedColCheckBox);
    layout->addWidget(syncColCheckBox);
    layout->addWidget(delayColCheckBox);
    layout->addWidget(ipv4ColCheckBox);
    layout->addWidget(ipv6ColCheckBox);
    layout->addWidget(activeColCheckBox);
    layout->addWidget(isosColCheckBox);

    mirrorColumnSelectGroupBox = new QGroupBox("Columns");
    mirrorColumnSelectGroupBox->setLayout(layout);
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
    if (!activeCheckBox->isEnabled()) {
        activeCheckBox->setEnabled(true);
    }
    if (!isosCheckBox->isEnabled()) {
        isosCheckBox->setEnabled(true);
    }
    if (!ipv4CheckBox->isEnabled()) {
        ipv4CheckBox->setEnabled(true);
    }
    if (!ipv6CheckBox->isEnabled()) {
        ipv6CheckBox->setEnabled(true);
    }

    mirrorProxyModel->setLeastRestrictiveFilter();
}

void MainWindow::uncheckCornerButton(int r)
{
    if (cornerButton->isChecked()) {
        cornerButton->setChecked(false);
    }
}

void MainWindow::selectAllMirrors(bool state)
{
    if (state) {
        // Get 1st column (URLs) indexes of all selected rows
        QModelIndexList indexes = tableView->selectionModel()->selectedRows(0);
        for (int i = 0; i < indexes.size(); i++) {
            mirrorModel->selectMirror(indexes.at(i).data().toString());
        }
    } else {
        // Indiscriminately deselect all mirrors
        mirrorModel->selectAllMirrors(false);
        tableView->clearSelection();
    }
}

void MainWindow::selectMirrors(const QItemSelection &selected, const QItemSelection &deselected)
{
    QString url;

    if (!selected.indexes().isEmpty()) {
        url = selected.indexes().at(0).data().toString();
        mirrorModel->selectMirror(url);
    }
    
    if (!deselected.indexes().isEmpty()) {
        url = deselected.indexes().at(0).data().toString();
        mirrorModel->deselectMirror(url);
    }
}

void MainWindow::setUrlColumn(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::url, true);
    } else {
        tableView->setColumnHidden(Columns::url, false);
    }
}

void MainWindow::setCountryColumn(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::country, true);
    } else {
        tableView->setColumnHidden(Columns::country, false);
    }
}

void MainWindow::setProtocolColumn(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::protocol, true);
    } else {
        tableView->setColumnHidden(Columns::protocol, false);
    }
}

void MainWindow::setCompletionColumn(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::completion_pct, true);
    } else {
        tableView->setColumnHidden(Columns::completion_pct, false);
    }
}

void MainWindow::setScoreColumn(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::score, true);
    } else {
        tableView->setColumnHidden(Columns::score, false);
    }
}

void MainWindow::setSpeedColumn(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::speed, true);
    } else {
        tableView->setColumnHidden(Columns::speed, false);
    }
}

void MainWindow::setSyncColumn(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::last_sync, true);
    } else {
        tableView->setColumnHidden(Columns::last_sync, false);
    }
}

void MainWindow::setDelayColumn(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::delay, true);
    } else {
        tableView->setColumnHidden(Columns::delay, false);
    }
}

void MainWindow::setIPv4Column(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::ipv4, true);
    } else {
        tableView->setColumnHidden(Columns::ipv4, false);
    }
}

void MainWindow::setIPv6Column(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::ipv6, true);
    } else {
        tableView->setColumnHidden(Columns::ipv6, false);
    }
}

void MainWindow::setActiveColumn(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::active, true);
    } else {
        tableView->setColumnHidden(Columns::active, false);
    }
}

void MainWindow::setIsosColumn(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::isos, true);
    } else {
        tableView->setColumnHidden(Columns::isos, false);
    }
}

void MainWindow::filterByCountry(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList items = selected.indexes();

    for (const QModelIndex &index : qAsConst(items)) {
        mirrorProxyModel->appendCountryFilter(index.data().toString());
    }

    items = deselected.indexes();

    for (const QModelIndex &index : qAsConst(items)) {
        mirrorProxyModel->removeCountryFilter(index.data().toString());
    }
}

void MainWindow::filterByHttp(int state)
{
    if (state == Qt::Unchecked &&
        httpsCheckBox->checkState() == Qt::Unchecked &&
        rsyncCheckBox->checkState() == Qt::Unchecked) {
            httpCheckBox->setCheckState(Qt::Checked);
    } else {
        if (state == Qt::Checked) {
            mirrorProxyModel->appendProtocolFilter("http");
        } else {
            mirrorProxyModel->removeProtocolFilter("http");
        }
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
            mirrorProxyModel->appendProtocolFilter("https");
        } else {
            mirrorProxyModel->removeProtocolFilter("https");
        }
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
            mirrorProxyModel->appendProtocolFilter("rsync");
        } else {
            mirrorProxyModel->removeProtocolFilter("rsync");
        }
    }
}

void MainWindow::filterByActive(int state)
{
    mirrorProxyModel->setActiveFilter(state);
}

void MainWindow::filterByIsos(int state)
{
    mirrorProxyModel->setIsosFilter(state);
}

void MainWindow::filterByIPv4(int state)
{
    if (state == Qt::Unchecked && ipv6CheckBox->checkState() == Qt::Unchecked) {
        ipv4CheckBox->setCheckState(Qt::Checked);
    } else {
        mirrorProxyModel->setIPv4Filter(state);
    }
}

void MainWindow::filterByIPv6(int state)
{
    if (state == Qt::Unchecked && ipv4CheckBox->checkState() == Qt::Unchecked) {
        ipv6CheckBox->setCheckState(Qt::Checked);
    } else {
        mirrorProxyModel->setIPv6Filter(state);
    }
}

void MainWindow::showAllMirrors()
{
    mirrorProxyModel->setLeastRestrictiveFilter();

    // Set filter checkboxes accordingly
    httpCheckBox->setCheckState(Qt::Checked);
    httpsCheckBox->setCheckState(Qt::Checked);
    rsyncCheckBox->setCheckState(Qt::Checked);
    activeCheckBox->setCheckState(Qt::PartiallyChecked);
    isosCheckBox->setCheckState(Qt::PartiallyChecked);
    ipv4CheckBox->setCheckState(Qt::PartiallyChecked);
    ipv6CheckBox->setCheckState(Qt::PartiallyChecked);

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

void MainWindow::saveMirrorList(const QString file)
{
    // Get 1st column (URLs) indexes of all selected rows
    QModelIndexList indexes = selectionModelTableView->selectedRows(Columns::url);

    QStringList urls;

    for (int i = 0; i < indexes.size(); i++) {
        urls.append(indexes.at(i).data().toString());
    }

    mirrorModel->saveMirrorList(file, true, urls);
}

void MainWindow::rankMirrorList()
{
    //QModelIndexList indexList = selectionModelTableView->selectedRows(0);

    if(!selectionModelTableView->hasSelection()) {
        QMessageBox::critical(this, tr("Error"), tr("No mirrors selected.\nPlease select at least one mirror."));
    } else {
        mirrorModel->rankMirrorList();
    }
}

void MainWindow::updateMirrorList()
{
    if(!selectionModelTableView->hasSelection()) {
        QMessageBox::critical(this, tr("Error"), tr("No mirrors selected.\nPlease select at least one mirror."));
    } else {
        // Get 1st column (URLs) indexes of all selected rows
        QModelIndexList indexes = tableView->selectionModel()->selectedRows(0);

        QStringList urls;

        for (int i = 0; i < indexes.size(); i++) {
            urls.append(indexes.at(i).data().toString());
        }

        mirrorModel->updateMirrorList(urls);
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

void MainWindow::showHttpDialog()
{
    QMessageBox::critical(this, tr("Error"), tr("No mirror with http or https protocols selected.\nPlease select at least one for Pacman use."));
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
