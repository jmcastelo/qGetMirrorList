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
#include <QMessageBox>
#include <QHeaderView>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    // Data source
    dataSource = new DataSource(this);

    // Model associated with the mirrors
    mirrorModel = new MirrorModel(this);

    // Mirror sort and filter proxy model
    mirrorProxyModel = new MirrorSortFilterProxyModel(this);
    mirrorProxyModel->setSourceModel(mirrorModel);
    mirrorProxyModel->setDynamicSortFilter(true);

    // Mirror drag and drop proxy model
    mirrorDnDProxyModel = new MirrorDnDProxyModel(this);
    mirrorDnDProxyModel->setSourceModel(mirrorProxyModel);

    // Table view associated with mirror model through proxy
    tableView = new QTableView(this);

    tableView->setModel(mirrorDnDProxyModel);

    tableView->setSelectionMode(QAbstractItemView::MultiSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(Columns::url, QHeaderView::Stretch);
    tableView->horizontalHeader()->setSectionsMovable(true);

    tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableView->verticalHeader()->setSectionsMovable(true);
    tableView->verticalHeader()->setSectionsClickable(false);

    tableView->setAlternatingRowColors(true);
    tableView->setShowGrid(false);
    tableView->setSortingEnabled(true);
    tableView->sortByColumn(Columns::country, Qt::AscendingOrder);

    tableView->setColumnHidden(Columns::ipv4, true);
    tableView->setColumnHidden(Columns::active, true);
    tableView->setColumnHidden(Columns::isos, true);

    cornerButton = static_cast<QPushButton*>(tableView->findChild<QAbstractButton *>());
    cornerButton->setCheckable(true);

    selectionModelTableView = tableView->selectionModel();

    // Model and list view associated with the countries
    countryModel = new CountryModel(this);

    listView = new QListView;
    listView->setModel(countryModel);
    listView->setSelectionMode(QAbstractItemView::MultiSelection);
    listView->setResizeMode(QListView::Adjust);

    selectionModelListView = listView->selectionModel();

    // Groups
    createMirrorActionsGroubBox();
    createMirrorTableGroupBox();
    createMirrorColumnSelectGroupBox();

    QVBoxLayout *vLayout = new QVBoxLayout;

    vLayout->addWidget(mirrorColumnSelectGroupBox);
    vLayout->addWidget(mirrorTableGroupBox);

    QGridLayout *mainLayout = new QGridLayout(this);

    statusBar = new QStatusBar(this);
    statusBar->setSizeGripEnabled(false);
    statusBar->showMessage("Please get the latest mirror list");

    statusBarLastCheckLabel = new QLabel(this);
    statusBar->addPermanentWidget(statusBarLastCheckLabel);

    mainLayout->addWidget(mirrorActionsGroupBox, 0, 0);
    mainLayout->addLayout(vLayout, 0, 1);
    mainLayout->addWidget(statusBar, 1, 0, 1, 2);

    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 10);

    setLayout(mainLayout);

    setWindowTitle(tr("Arch Linux - qGetMirrorList"));
   
    QPixmap pixmap;
    pixmap.load(":/images/logos/logo-small.png");

    setWindowIcon(QIcon(pixmap));

    setGeometry(0, 0, 1300, 700);

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

    rankingProgressBar = new QProgressBar(this);
    rankingProgressBar->setMinimum(0);

    cancelRankingButton = new QPushButton("Cancel");
    cancelRankingButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);

    waitLayout->addWidget(waitLabel);
    waitLayout->addWidget(rankingProgressBar);
    waitLayout->addWidget(cancelRankingButton);

    waitForRankingDialog->setLayout(waitLayout);

    // Connections: actions 
    connect(getMirrorListButton, &QPushButton::clicked, dataSource, &DataSource::getSourceData);
    connect(getMirrorListButton, &QPushButton::clicked, this, &MainWindow::showGettingMirrorListStatusMessage);
    connect(dataSource, &DataSource::mirrorListReady, mirrorModel, &MirrorModel::setMirrorList);
    connect(dataSource, &DataSource::countryListReady, countryModel, &CountryModel::setCountryList);
    connect(dataSource, &DataSource::lastCheckReady, this, &MainWindow::setLastCheck);
    connect(saveMirrorListButton, &QPushButton::clicked, this, &MainWindow::openSaveDialog);
    connect(saveMirrorListDialog, &QFileDialog::fileSelected, this, &MainWindow::saveMirrorList);
    connect(rankMirrorListButton, &QPushButton::clicked, this, &MainWindow::rankMirrorList);
    connect(updateMirrorListButton, &QPushButton::clicked, this, &MainWindow::updateMirrorList);
    connect(showAllMirrorsButton, &QPushButton::clicked, this, &MainWindow::showAllMirrors);

    // Connections: various
    connect(cornerButton, &QPushButton::clicked, this, &MainWindow::selectAllMirrors);
    connect(mirrorModel, &MirrorModel::mirrorListSet, this, &MainWindow::enableWidgets);
    connect(mirrorModel, &MirrorModel::mirrorListSet, this, &MainWindow::setTableGroupTitle);
    connect(selectionModelTableView, &QItemSelectionModel::selectionChanged, this, &MainWindow::selectMirrors);

    // Connections: drag and drop
    connect(tableView->verticalHeader(), &QHeaderView::sectionCountChanged, mirrorDnDProxyModel, &MirrorDnDProxyModel::setVerticalSectionsList);
    connect(tableView->verticalHeader(), &QHeaderView::sectionMoved, mirrorDnDProxyModel, &MirrorDnDProxyModel::reorderVerticalSectionsList);
    connect(tableView->horizontalHeader(), &QHeaderView::sectionClicked, mirrorDnDProxyModel, &MirrorDnDProxyModel::resetVerticalSectionsList);
    connect(tableView->verticalHeader(), &QHeaderView::sectionCountChanged, this, &MainWindow::saveVerticalHeaderState);
    connect(mirrorDnDProxyModel, &MirrorDnDProxyModel::verticalSectionsListReordered, this, &MainWindow::restoreVerticalHeaderState);

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
    connect(mirrorModel, &MirrorModel::rankingMirrorsStarted, rankingProgressBar, &QProgressBar::reset);
    connect(mirrorModel, &MirrorModel::rankingMirrorsStarted, this, &MainWindow::showRankingStartedStatusMessage);
    connect(mirrorModel, &MirrorModel::rankingMirrorsFinished, waitForRankingDialog, &QDialog::done);
    connect(mirrorModel, &MirrorModel::rankingMirrorsFinished, this, &MainWindow::showRankingFinishedStatusMessage);
    connect(mirrorModel, &MirrorModel::setProgressBarMax, rankingProgressBar, &QProgressBar::setMaximum);
    connect(mirrorModel, &MirrorModel::setProgressBarValue, rankingProgressBar, &QProgressBar::setValue);
    connect(mirrorModel, &MirrorModel::rankingMirrorsErrors, this, &MainWindow::showRankingErrorsDialog);
    connect(cancelRankingButton, &QPushButton::clicked, mirrorModel, &MirrorModel::cancelRankMirrorList);

    // Connections: update & about
    connect(mirrorModel, &MirrorModel::noHttpMirrorSelected, this, &MainWindow::showHttpDialog);
    connect(mirrorModel, &MirrorModel::updateMirrorListFinished, this, &MainWindow::updateFinished);
    connect(mirrorModel, &MirrorModel::updateMirrorListError, this, &MainWindow::updateMirrorListError);
    connect(aboutButton, &QPushButton::clicked, this, &MainWindow::about);

    // Connections: save dialog
    connect(saveMirrorListDialog, &QDialog::finished, this, &MainWindow::saveDialogFinished);

    // Connections: network errors
    connect(dataSource, &DataSource::networkReplyError, this, &MainWindow::showJsonErrorDialog);
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
    saveMirrorListButton->setToolTip("Save selected mirrors to chosen file");
    rankMirrorListButton->setToolTip("Obtain speed of selected mirrors");
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
    layout->addWidget(speedColCheckBox);
    layout->addWidget(completionColChechBox);
    layout->addWidget(scoreColCheckBox);
    layout->addWidget(syncColCheckBox);
    layout->addWidget(delayColCheckBox);
    layout->addWidget(ipv4ColCheckBox);
    layout->addWidget(ipv6ColCheckBox);
    layout->addWidget(activeColCheckBox);
    layout->addWidget(isosColCheckBox);

    mirrorColumnSelectGroupBox = new QGroupBox("Columns");
    mirrorColumnSelectGroupBox->setLayout(layout);
}

void MainWindow::setLastCheck(QDateTime lastCheck)
{
    lastCheckDateTime = lastCheck;

    QString lastCheckString = lastCheckDateTime.toString(QString("yyyy/MM/dd HH:mm:ss"));

    statusBarLastCheckLabel->setText(QString("Last mirror check: %1").arg(lastCheckString));
    statusBarLastCheckLabel->show();

    statusBar->showMessage("Mirror list fetched successfully", 10000);
}

void MainWindow::showGettingMirrorListStatusMessage()
{
    statusBar->showMessage("Getting mirror list...");
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
    if (!updateMirrorListButton->isEnabled() && QFileInfo::exists("/usr/bin/pkexec")) {
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

void MainWindow::saveVerticalHeaderState(int oldCount, int newCount)
{
    Q_UNUSED(oldCount)
    Q_UNUSED(newCount)
    verticalHeaderState = tableView->verticalHeader()->saveState();
}

void MainWindow::restoreVerticalHeaderState()
{
    tableView->verticalHeader()->restoreState(verticalHeaderState);
    statusBar->showMessage("Drag & drop", 10000);
}

void MainWindow::selectAllMirrors(bool state)
{
    if (!state) {
        tableView->clearSelection();
    }
}

void MainWindow::selectMirrors(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    setTableGroupTitle();
}

void MainWindow::setUrlColumn(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::url, true);
        // Change resize mode for the rest of the columns to stretch all available space
        tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    } else {
        tableView->setColumnHidden(Columns::url, false);
        // Reset resize mode
        tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        tableView->horizontalHeader()->setSectionResizeMode(Columns::url, QHeaderView::Stretch);
    }

    showColumnToggleMessage(state);
}

void MainWindow::setCountryColumn(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::country, true);
    } else {
        tableView->setColumnHidden(Columns::country, false);
    }

    showColumnToggleMessage(state);
}

void MainWindow::setProtocolColumn(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::protocol, true);
    } else {
        tableView->setColumnHidden(Columns::protocol, false);
    }

    showColumnToggleMessage(state);
}

void MainWindow::setCompletionColumn(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::completion_pct, true);
    } else {
        tableView->setColumnHidden(Columns::completion_pct, false);
    }

    showColumnToggleMessage(state);
}

void MainWindow::setScoreColumn(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::score, true);
    } else {
        tableView->setColumnHidden(Columns::score, false);
    }

    showColumnToggleMessage(state);
}

void MainWindow::setSpeedColumn(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::speed, true);
    } else {
        tableView->setColumnHidden(Columns::speed, false);
    }

    showColumnToggleMessage(state);
}

void MainWindow::setSyncColumn(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::last_sync, true);
    } else {
        tableView->setColumnHidden(Columns::last_sync, false);
    }

    showColumnToggleMessage(state);
}

void MainWindow::setDelayColumn(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::delay, true);
    } else {
        tableView->setColumnHidden(Columns::delay, false);
    }

    showColumnToggleMessage(state);
}

void MainWindow::setIPv4Column(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::ipv4, true);
    } else {
        tableView->setColumnHidden(Columns::ipv4, false);
    }

    showColumnToggleMessage(state);
}

void MainWindow::setIPv6Column(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::ipv6, true);
    } else {
        tableView->setColumnHidden(Columns::ipv6, false);
    }

    showColumnToggleMessage(state);
}

void MainWindow::setActiveColumn(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::active, true);
    } else {
        tableView->setColumnHidden(Columns::active, false);
    }

    showColumnToggleMessage(state);
}

void MainWindow::setIsosColumn(int state)
{
    if (state == Qt::Unchecked) {
        tableView->setColumnHidden(Columns::isos, true);
    } else {
        tableView->setColumnHidden(Columns::isos, false);
    }

    showColumnToggleMessage(state);
}

void MainWindow::showColumnToggleMessage(int state)
{
    if (state == Qt::Unchecked) {
        statusBar->showMessage("Column hidden", 10000);
    } else if (state == Qt::Checked) {
        statusBar->showMessage("Column shown", 10000);
    }
}

void MainWindow::setTableGroupTitle()
{
    int totalMirrors = mirrorModel->rowCount();
    int filterMirrors = mirrorProxyModel->rowCount();
    int selectedMirrors = selectionModelTableView->selectedRows(Columns::url).size();
    
    if (filterMirrors == totalMirrors) {
        if (selectedMirrors == 0) {
            mirrorTableGroupBox->setTitle(QString("Mirrors [Total %1]").arg(totalMirrors));
        } else {
            mirrorTableGroupBox->setTitle(QString("Mirrors [Selected %2 | Total %1]").arg(totalMirrors).arg(selectedMirrors));
        }
    } else {
        if (selectedMirrors == 0) {
            mirrorTableGroupBox->setTitle(QString("Mirrors [Filtered %1 | Total %2]").arg(filterMirrors).arg(totalMirrors));
        } else {
            mirrorTableGroupBox->setTitle(QString("Mirrors [Selected %3 | Filtered %1 | Total %2]").arg(filterMirrors).arg(totalMirrors).arg(selectedMirrors));
        }
    }
}

void MainWindow::filterByCountry(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList items = selected.indexes();

    for (const QModelIndex &index : qAsConst(items)) {
        mirrorProxyModel->appendCountryFilter(index.data().toString());
    }

    if (!items.isEmpty()) {
        statusBar->showMessage("Country selected", 10000);
    }

    items = deselected.indexes();

    for (const QModelIndex &index : qAsConst(items)) {
        mirrorProxyModel->removeCountryFilter(index.data().toString());
    }

    if (!items.isEmpty()) {
        statusBar->showMessage("Country deselected", 10000);
    }

    tableView->resizeColumnToContents(Columns::country);

    setTableGroupTitle();
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
        setTableGroupTitle();
    }

    showFilteringMessage(state);
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
        setTableGroupTitle();
    }

    showFilteringMessage(state);
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
        setTableGroupTitle();
    }

    showFilteringMessage(state);
}

void MainWindow::filterByActive(int state)
{
    mirrorProxyModel->setActiveFilter(state);
    setTableGroupTitle();
    showFilteringMessage(state);
}

void MainWindow::filterByIsos(int state)
{
    mirrorProxyModel->setIsosFilter(state);
    setTableGroupTitle();
    showFilteringMessage(state);
}

void MainWindow::filterByIPv4(int state)
{
    if (state == Qt::Unchecked && ipv6CheckBox->checkState() == Qt::Unchecked) {
        ipv4CheckBox->setCheckState(Qt::Checked);
    } else {
        mirrorProxyModel->setIPv4Filter(state);
        setTableGroupTitle();
    }
    showFilteringMessage(state);
}

void MainWindow::filterByIPv6(int state)
{
    if (state == Qt::Unchecked && ipv4CheckBox->checkState() == Qt::Unchecked) {
        ipv6CheckBox->setCheckState(Qt::Checked);
    } else {
        mirrorProxyModel->setIPv6Filter(state);
        setTableGroupTitle();
    }
    showFilteringMessage(state);
}

void MainWindow::showFilteringMessage(int state)
{
    if (state == Qt::PartiallyChecked) {
        statusBar->showMessage("Filter disabled", 10000);
    } else if (state == Qt::Checked || state == Qt::Unchecked) {
        statusBar->showMessage("Filter enabled", 10000);
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
        QMessageBox::warning(this, tr("Warning"), tr("No mirrors selected.\nPlease select at least one mirror."));
        statusBar->showMessage("Save warning: no mirrors selected", 10000);
    } else {
        statusBar->showMessage("Saving selected mirrors...");
        saveMirrorListDialog->open();
    }
}

void MainWindow::saveMirrorList(const QString file)
{
    // Get 1st column (URLs) indexes of all selected rows
    QModelIndexList urlIndexes = selectionModelTableView->selectedRows(Columns::url);
    mirrorModel->saveMirrorList(file, urlIndexes);
}

void MainWindow::saveDialogFinished(int result) {
    if(result == 0) {
        statusBar->showMessage("Save canceled", 10000);
    } else if (result == 1) {
        statusBar->showMessage("Selected mirrors saved", 10000);
    }
}

void MainWindow::rankMirrorList()
{
    if(!selectionModelTableView->hasSelection()) {
        QMessageBox::warning(this, tr("Warning"), tr("No mirrors selected.\nPlease select at least one mirror."));
        statusBar->showMessage("Ranking warning: no mirrors selected", 10000);
    } else {
        // Get 1st column (URLs) indexes of all selected rows
        QModelIndexList urlIndexes = selectionModelTableView->selectedRows(Columns::url);
        mirrorModel->rankMirrorList(urlIndexes);
    }
}

void MainWindow::updateMirrorList()
{
    if(!selectionModelTableView->hasSelection()) {
        QMessageBox::warning(this, tr("Warning"), tr("No mirrors selected.\nPlease select at least one http/https mirror."));
        statusBar->showMessage("Update warning: no mirrors selected", 10000);
    } else {
        QModelIndexList urlIndexes = selectionModelTableView->selectedRows(Columns::url);
        mirrorModel->updateMirrorList(urlIndexes);
    }
}

void MainWindow::updateFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)

    if (exitCode == 0) {
        QMessageBox::information(this, tr("Action"), tr("'/etc/pacman.d/mirrorlist' successfully updated.\nBackup of previous mirrorlist in /tmp/mirrorlist.backup"));
        statusBar->showMessage("Update successful", 10000);
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Update failure."));
        statusBar->showMessage("Update failure", 10000);
    }
}

void MainWindow::updateMirrorListError(QProcess::ProcessError error)
{
    if (error == QProcess::FailedToStart) {
        QMessageBox::critical(this, tr("Error"), tr("Binary 'pkexec' nor found!"));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Error updating '/etc/pacman.d/mirrorlist'"));
    }
    statusBar->showMessage("Update error", 10000);
}

void MainWindow::showHttpDialog()
{
    QMessageBox::warning(this, tr("Warning"), tr("No mirror with http or https protocol selected.\nPlease select at least one for Pacman use."));
    statusBar->showMessage("Update warning: no http/https mirror selected", 10000);
}

void MainWindow::about()
{
    QMessageBox *aboutBox = new QMessageBox(this);
    
    aboutBox->setWindowTitle("About");
    
    aboutBox->setIconPixmap(QPixmap(":/images/logos/logo-big.png"));
    
    QStringList lines;
    lines.append("Arch Linux - qGetMirrorList 0.2\n");
    lines.append("Get and manipulate latest Arch Linux Pacman mirror list.\n");
    lines.append("References:");
    lines.append(" https://www.archlinux.org/mirrors/status/");
    lines.append(" https://xyne.archlinux.ca/projects/reflector/\n");
    lines.append("Required dependency: Qt >= 5.13.1");
    lines.append("Optional dependencies: rsync >= 3.1.3, polkit >= 0.116");

    QString text = lines.join("\n");

    aboutBox->setText(text);

    aboutBox->setInformativeText("Copyright 2019 José María Castelo Ares\nLicense: GPLv3");
    
    aboutBox->exec();
}

void MainWindow::showJsonErrorDialog(QString errorMessage)
{
    QMessageBox::critical(this, tr("Error"), errorMessage);
    statusBar->showMessage("Error fetching mirror list", 10000);
}

void MainWindow::showRankingErrorsDialog(QString errorMessage)
{
    QMessageBox::critical(this, tr("Error ranking following mirrors"), errorMessage);
    statusBar->showMessage("Ranking finished with errors", 10000);
}

void MainWindow::showRankingStartedStatusMessage()
{
    statusBar->showMessage("Ranking selected mirrors...");
}

void MainWindow::showRankingFinishedStatusMessage()
{
    statusBar->showMessage("Ranking finished", 10000);
}
