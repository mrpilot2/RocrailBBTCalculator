
#include "mainwindow.hpp"

#include <QResizeEvent>

#include <QtCore/QSettings>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "bbtmodel.hpp"
#include "blockmodel.hpp"
#include "locmodel.hpp"
#include "routemodel.hpp"
#include "ui_mainwindow.h"

using BBTCalculator::Gui::MainWindow;
using BBTCalculator::Gui::MainWindowController;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupMenuActions();

    setupTable(ui->locList);

    setupTable(ui->blockTable);

    setupTable(ui->routesTable);

    setupTable(ui->bbtTable);

    ui->locImage->installEventFilter(this);

    ui->calculateBBT->setEnabled(false);
    ui->correctionFactorDescription->setEnabled(false);
    ui->correctionFactor->setEnabled(false);
    ui->showMainlineOnly->setEnabled(false);

    QSettings settings;
    this->restoreGeometry(settings.value("mainwindow/geometry").toByteArray());

    ui->splitter->restoreState(settings.value("splitter/state").toByteArray());
    ui->splitter_2->restoreState(
        settings.value("splitter2/state").toByteArray());

    ui->splitter->restoreGeometry(
        settings.value("splitter/geometry").toByteArray());
    ui->splitter_2->restoreGeometry(
        settings.value("splitter2/geometry").toByteArray());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setController(MainWindowController* contr)
{
    controller = contr;
    connectSignals();

    QSettings settings;
    ui->showMainlineOnly->setChecked(
        settings.value("showMainLineOnly", false).toBool());
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == ui->locImage && event->type() == QEvent::Resize)
    {
        auto resizeEvent = dynamic_cast<QResizeEvent*>(event);

        if (!ui->locImage->pixmap().isNull())
        {
            ui->locImage->setPixmap(originalLocImage.scaled(
                resizeEvent->size(), Qt::KeepAspectRatio));
            return true;
        }
        return false;
    }
    return QObject::eventFilter(watched, event);
}

void MainWindow::setupTable(QTableView* table) const
{
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSortingEnabled(true);
    table->horizontalHeader()->setStretchLastSection(false);
    table->horizontalHeader()->setSectionsMovable(true);
}

void MainWindow::setupMenuActions()
{
    ui->actionClose->setShortcuts(QKeySequence::Quit);
    ui->actionOpenWorkspace->setShortcuts(QKeySequence::Open);
}

void MainWindow::retranslateUi()
{
    ui->retranslateUi(this);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QSettings settings;
    settings.setValue("mainwindow/geometry", this->saveGeometry());

    settings.setValue("showMainLineOnly", ui->showMainlineOnly->isChecked());

    settings.setValue("locView/columnSequence",
                      ui->locList->horizontalHeader()->saveState());
    settings.setValue("blockView/columnSequence",
                      ui->blockTable->horizontalHeader()->saveState());
    settings.setValue("routesView/columnSequence",
                      ui->routesTable->horizontalHeader()->saveState());
    settings.setValue("bbtView/columnSequence",
                      ui->bbtTable->horizontalHeader()->saveState());

    settings.setValue("locView/geometry",
                      ui->locList->horizontalHeader()->saveGeometry());
    settings.setValue("blockView/geometry",
                      ui->blockTable->horizontalHeader()->saveGeometry());
    settings.setValue("routesView/geometry",
                      ui->routesTable->horizontalHeader()->saveGeometry());
    settings.setValue("bbtView/geometry",
                      ui->bbtTable->horizontalHeader()->saveGeometry());

    settings.setValue("splitter/state", ui->splitter->saveState());
    settings.setValue("splitter2/state", ui->splitter_2->saveState());

    settings.setValue("splitter/geometry", ui->splitter->saveGeometry());
    settings.setValue("splitter2/geometry", ui->splitter_2->saveGeometry());

    QWidget::closeEvent(event);
}

void MainWindow::connectSignals()
{
    connect(ui->actionOpenWorkspace, &QAction::triggered, controller,
            &MainWindowController::onOpenWorkspaceClicked);
    connect(ui->showMainlineOnly, &QCheckBox::checkStateChanged, controller,
            &MainWindowController::onFilterMainlineStateChanged);
    connect(ui->calculateBBT, &QPushButton::clicked, controller,
            &MainWindowController::onUserRequestsBBTCalculation);
}

auto MainWindow::letUserSelectWorkspaceDirectory() -> QString
{
    return QFileDialog::getExistingDirectory(
        this, QApplication::translate("MainWindow", "Select Rocrail workspace"),
        QDir::homePath(), QFileDialog::ShowDirsOnly);
}

void MainWindow::notifyUserSelectedDirectoryDoesNotExist()
{
    QMessageBox::critical(
        this,
        QApplication::translate("MainWindow", "Workspace directory not found"),
        QApplication::translate("MainWindow",
                                "The selected workspace directory does not"
                                "exist."));
}

void MainWindow::showRootPath(const QString& path)
{
    this->setWindowTitle(path);
}

void MainWindow::setLocTableModel(QAbstractItemModel* model)
{
    ui->locList->setModel(model);

    ui->locList->resizeColumnsToContents();
    ui->locList->sortByColumn(static_cast<int>(LocViewColumns::NAME),
                              Qt::AscendingOrder);
    connect(ui->locList->selectionModel(),
            &QItemSelectionModel::selectionChanged, controller,
            &MainWindowController::onLocSelectionChanged);
    QSettings settings;
    ui->locList->horizontalHeader()->restoreState(
        settings.value("locView/columnSequence").toByteArray());
    ui->locList->horizontalHeader()->restoreGeometry(
        settings.value("locView/geometry").toByteArray());
    setupTable(ui->locList);
}

void MainWindow::setBlockTableModel(QAbstractItemModel* model)
{
    ui->blockTable->setModel(model);

    ui->blockTable->resizeColumnsToContents();
    ui->blockTable->sortByColumn(static_cast<int>(BlockViewColumns::NAME),
                                 Qt::AscendingOrder);
    ui->showMainlineOnly->setEnabled(true);
    controller->onFilterMainlineStateChanged(
        ui->showMainlineOnly->checkState());

    QSettings settings;
    ui->blockTable->horizontalHeader()->restoreState(
        settings.value("blockView/columnSequence").toByteArray());
    ui->blockTable->horizontalHeader()->restoreGeometry(
        settings.value("blockView/geometry").toByteArray());
    setupTable(ui->blockTable);
}

void MainWindow::setRouteTableModel(QAbstractItemModel* model)
{
    ui->routesTable->setModel(model);

    ui->routesTable->resizeColumnsToContents();
    ui->routesTable->sortByColumn(static_cast<int>(RouteViewColumns::NAME),
                                  Qt::AscendingOrder);
    QSettings settings;
    ui->routesTable->horizontalHeader()->restoreState(
        settings.value("routesView/columnSequence").toByteArray());
    ui->routesTable->horizontalHeader()->restoreGeometry(
        settings.value("routesView/geometry").toByteArray());
    setupTable(ui->routesTable);
}

void MainWindow::displayLocImage(QPixmap locImage)
{
    if (locImage.isNull())
    {
        ui->locImage->clear();
        return;
    }
    originalLocImage = locImage;

    ui->locImage->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->locImage->setPixmap(
        originalLocImage.scaled(ui->locImage->size(), Qt::KeepAspectRatio));
}

void BBTCalculator::Gui::MainWindow::setBBTTableModel(QAbstractItemModel* model)
{
    ui->bbtTable->setModel(model);

    ui->bbtTable->resizeColumnsToContents();
    ui->bbtTable->sortByColumn(static_cast<int>(BBTViewColumns::BLOCK),
                               Qt::AscendingOrder);

    ui->calculateBBT->setEnabled(true);
    ui->correctionFactorDescription->setEnabled(true);
    ui->correctionFactor->setEnabled(true);

    QSettings settings;
    ui->bbtTable->horizontalHeader()->restoreState(
        settings.value("bbtView/columnSequence").toByteArray());
    ui->bbtTable->horizontalHeader()->restoreGeometry(
        settings.value("bbtView/geometry").toByteArray());
    setupTable(ui->bbtTable);
}

void MainWindow::setLocNameInBBTBox(const QString& locName)
{
    ui->bbtBox->setTitle(
        "BBT - " + locName + " (" +
        QString::number(ui->bbtTable->model()->rowCount()) + " " +
        QApplication::translate("MainWindow", "entries") + ")");
}

void MainWindow::setCorrectionFactor(double correctionFactor)
{
    ui->correctionFactor->setValue(correctionFactor);
}

auto MainWindow::getUserSelectedCorrectionFactor() const -> double
{
    return ui->correctionFactor->value();
}

auto MainWindow::askUserIfExistingBBTEntriesShallBeDeleted() -> int
{
    return QMessageBox::warning(
        this, QApplication::translate("MainWindow", "Existing BBT entries"),
        QApplication::translate("MainWindow",
                                "There are some BBT entries stored for this "
                                "locomotive.\nShall they "
                                "be overwritten?"),
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
}
