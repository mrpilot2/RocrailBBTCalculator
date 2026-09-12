
#include "mainwindowcontroller.hpp"

#include <mainwindowcontroller.hpp>

#include "core/core.hpp"

using BBTCalculator::Core::Core;
using BBTCalculator::Gui::MainWindowController;

MainWindowController::MainWindowController()
    : core{nullptr}
    , currentlySelectedLocName{""}
{}

void MainWindowController::setCore(BBTCalculator::Core::Core* c)
{
    core = c;
}

void MainWindowController::onOpenWorkspaceClicked(bool)
{
    core->letUserSelectWorkspace();
}

void MainWindowController::onLocSelectionChanged(
    const QItemSelection& selected, const QItemSelection& deselected)
{
    if (!selected.isEmpty()) {
        const QItemSelectionRange& singleRow{selected.at(0)};

        const QString locName{singleRow.indexes()
                                  .at(static_cast<int>(LocViewColumns::NAME))
                                  .data()
                                  .toString()};
        currentlySelectedLocName = locName;
        core->displayImageForLocName(locName);
        core->createBBTModel(locName);
    }
}

void MainWindowController::onFilterMainlineStateChanged(int state)
{
    if (state == Qt::Checked) {
        core->filterBlockByMainline(
            static_cast<int>(BlockViewColumns::MAINLINE));
        core->filterRouteByMainline(
            static_cast<int>(RouteViewColumns::MAINLINE));
    } else {
        core->removeBlockAndRouteMainlineFilter();
    }
}

void MainWindowController::onUserRequestsBBTCalculation()
{
    core->calculateBBT(currentlySelectedLocName);
}
