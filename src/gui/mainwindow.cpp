
#include "mainwindow.hpp"

#include <utility>

#include <aide/hierarchicalid.hpp>

#include <QResizeEvent>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "bbtmodel.hpp"
#include "blockmodel.hpp"
#include "locmodel.hpp"
#include "routemodel.hpp"
#include "ui_mainwindow.h"

using BBTCalculator::Gui::MainWindow;
using BBTCalculator::Gui::MainWindowController;

namespace
{
    // Key layout (mapped from legacy QSettings keys by the migration):
    //   View/ShowMainlineOnly             <- showMainLineOnly
    //   View/<Table>/HeaderState          <- <x>View/columnSequence
    //   View/<Table>/HeaderGeometry       <- <x>View/geometry
    //   View/<Splitter>/State|Geometry    <- splitter[2]/state|geometry
    aide::HierarchicalId viewKey(const char* group, const char* name)
    { return aide::HierarchicalId("View")(group)(name); }

    aide::HierarchicalId showMainlineOnlyKey()
    { return aide::HierarchicalId("View")("ShowMainlineOnly"); }
} // namespace

MainWindow::MainWindow(
    std::shared_ptr<aide::SettingsInterface> unversionableSettings,
    QWidget* parent)
    : QWidget(parent)
    , settings{std::move(unversionableSettings)}
    , ui(new Ui::BBTMainWindow)
{
    ui->setupUi(this);

    setupTable(ui->locList);

    setupTable(ui->blockTable);

    setupTable(ui->routesTable);

    setupTable(ui->bbtTable);

    ui->locImage->installEventFilter(this);

    ui->calculateBBT->setEnabled(false);
    ui->correctionFactorDescription->setEnabled(false);
    ui->correctionFactor->setEnabled(false);
    ui->showMainlineOnly->setEnabled(false);

    restoreSplitter(ui->splitter, "Splitter1");
    restoreSplitter(ui->splitter_2, "Splitter2");
}

MainWindow::~MainWindow()
{ delete ui; }

void MainWindow::setController(MainWindowController* contr)
{
    controller = contr;
    connectSignals();

    ui->showMainlineOnly->setChecked(
        settings->value(showMainlineOnlyKey(), false).toBool());
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == ui->locImage && event->type() == QEvent::Resize) {
        auto resizeEvent = dynamic_cast<QResizeEvent*>(event);

        if (!ui->locImage->pixmap().isNull()) {
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

void MainWindow::retranslateUi()
{
    ui->retranslateUi(this);

    // Header captions come from the models' headerData(); force a repaint.
    for (auto* table : {ui->blockTable, ui->routesTable, ui->bbtTable}) {
        table->horizontalHeader()->viewport()->update();
    }
    ui->locList->horizontalHeader()->viewport()->update();
}

void MainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) { retranslateUi(); }
    QWidget::changeEvent(event);
}

void MainWindow::saveHeader(QTableView* table, const char* group)
{
    settings->setValue(viewKey(group, "HeaderState"),
                       table->horizontalHeader()->saveState());
    settings->setValue(viewKey(group, "HeaderGeometry"),
                       table->horizontalHeader()->saveGeometry());
}

void MainWindow::restoreHeader(QTableView* table, const char* group)
{
    table->horizontalHeader()->restoreState(
        settings->value(viewKey(group, "HeaderState")).toByteArray());
    table->horizontalHeader()->restoreGeometry(
        settings->value(viewKey(group, "HeaderGeometry")).toByteArray());
    setupTable(table);
}

void MainWindow::restoreSplitter(QSplitter* splitter, const char* group)
{
    splitter->restoreState(
        settings->value(viewKey(group, "State")).toByteArray());
    splitter->restoreGeometry(
        settings->value(viewKey(group, "Geometry")).toByteArray());
}

void MainWindow::saveState()
{
    settings->setValue(showMainlineOnlyKey(),
                       ui->showMainlineOnly->isChecked());

    saveHeader(ui->locList, "LocTable");
    saveHeader(ui->blockTable, "BlockTable");
    saveHeader(ui->routesTable, "RoutesTable");
    saveHeader(ui->bbtTable, "BBTTable");

    for (auto [splitter, group] :
         {std::pair{ui->splitter, "Splitter1"},
          std::pair{ui->splitter_2, "Splitter2"}}) {
        settings->setValue(viewKey(group, "State"), splitter->saveState());
        settings->setValue(viewKey(group, "Geometry"),
                           splitter->saveGeometry());
    }
}

void MainWindow::connectSignals()
{
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
    window()->setWindowTitle(QApplication::applicationDisplayName() + " - " +
                             path);
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
    restoreHeader(ui->locList, "LocTable");
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

    restoreHeader(ui->blockTable, "BlockTable");
}

void MainWindow::setRouteTableModel(QAbstractItemModel* model)
{
    ui->routesTable->setModel(model);

    ui->routesTable->resizeColumnsToContents();
    ui->routesTable->sortByColumn(static_cast<int>(RouteViewColumns::NAME),
                                  Qt::AscendingOrder);
    restoreHeader(ui->routesTable, "RoutesTable");
}

void MainWindow::displayLocImage(QPixmap locImage)
{
    if (locImage.isNull()) {
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

    restoreHeader(ui->bbtTable, "BBTTable");
}

void MainWindow::setLocNameInBBTBox(const QString& locName)
{
    ui->bbtBox->setTitle(
        "BBT - " + locName + " (" +
        QString::number(ui->bbtTable->model()->rowCount()) + " " +
        QApplication::translate("MainWindow", "entries") + ")");
}

void MainWindow::setCorrectionFactor(double correctionFactor)
{ ui->correctionFactor->setValue(correctionFactor); }

auto MainWindow::getUserSelectedCorrectionFactor() const -> double
{ return ui->correctionFactor->value(); }

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
