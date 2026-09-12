#include "core.hpp"

#include <memory>

#include <QtWidgets/QtWidgets>

#include "gui/imagesortfilterproxymodel.hpp"
#include "gui/mainwindow.hpp"
#include "gui/mainwindowcontroller.hpp"

#include "calculation.hpp"
#include "planparser.hpp"

using BBTCalculator::Core::Core;
using BBTCalculator::Gui::MainWindow;
using BBTCalculator::Gui::MainWindowController;

Core::Core::Core()
    : mainWindow{new MainWindow()}
{
}

void Core::initializeApplication(MainWindowController& contr)
{
    mainWindow->setController(&contr);
    setupTranslator();
    mainWindow->show();

    loadLastOpenWorkspace();
}

void Core::setupTranslator()
{
    if (translator.load(QLocale(), QLatin1String("rocrailBBT"),
                        QLatin1String("_"), QLatin1String(":/translations")))
    {
        QApplication::installTranslator(&translator);
        mainWindow->retranslateUi();
    }
}

void BBTCalculator::Core::Core::loadLastOpenWorkspace()
{
    QSettings settings;

    QString workspaceDirectory =
        settings.value("workspaceDirectory", "").toString();

    loadWorkspace(workspaceDirectory);
}

void BBTCalculator::Core::Core::letUserSelectWorkspace()
{
    QString userSelectedDirectory =
        mainWindow->letUserSelectWorkspaceDirectory();

    loadWorkspace(userSelectedDirectory);

    QSettings settings;
    settings.setValue("workspaceDirectory", userSelectedDirectory);
}

void Core::loadWorkspace(const QString& userSelectedDirectory)
{
    if (!userSelectedDirectory.isEmpty())
    {
        const QDir dir{userSelectedDirectory};
        if (dir.exists())
        {
            workspace.setRootPath(dir);
            mainWindow->showRootPath(dir.absolutePath());
            PlanParser parser(workspace.getPlanFilePath());
            parser.parse();

            workspace.setLocList(parser.getLocList());
            workspace.setBlockList(parser.getBlockList());
            workspace.setRouteList(parser.getRouteList());

            locModel = std::make_unique<Gui::LocModel>(workspace.getLocList());
            blockModel =
                std::make_unique<Gui::BlockModel>(workspace.getBlockList());
            routeModel =
                std::make_unique<Gui::RouteModel>(workspace.getRouteList());

            locSortFilterModel =
                std::make_unique<Gui::ImageSortFilterProxyModel>();
            blockSortFilterModel =
                std::make_unique<Gui::ImageSortFilterProxyModel>();
            routeSortFilterModel =
                std::make_unique<Gui::ImageSortFilterProxyModel>();

            locSortFilterModel->setSourceModel(locModel.get());
            blockSortFilterModel->setSourceModel(blockModel.get());
            routeSortFilterModel->setSourceModel(routeModel.get());

            mainWindow->setLocTableModel(locSortFilterModel.get());
            mainWindow->setBlockTableModel(blockSortFilterModel.get());
            mainWindow->setRouteTableModel(routeSortFilterModel.get());
        }
        else
        {
            mainWindow->notifyUserSelectedDirectoryDoesNotExist();
        }
    }
}

void Core::displayImageForLocName(const QString& locName)
{
    const LocList& locList = workspace.getLocList();

    const auto search = [locName](const Loc& item) {
        return item.name == locName;
    };

    auto it = std::find_if(locList.begin(), locList.end(), search);

    if (it != locList.end())
    {
       mainWindow->displayLocImage(it->locImage);
    }
}

void Core::createBBTModel(const QString& locName)
{
    const LocList& locList = workspace.getLocList();

    const auto search = [locName](Loc item) { return item.name == locName; };

    auto it = std::find_if(locList.begin(), locList.end(), search);

    if (it != locList.end())
    {
        bbtModel = std::make_unique<Gui::BBTModel>(it->bbt);

        bbtSortFilterModel = std::make_unique<Gui::ImageSortFilterProxyModel>();
        bbtSortFilterModel->setSourceModel(bbtModel.get());

        mainWindow->setBBTTableModel(bbtSortFilterModel.get());

        mainWindow->setLocNameInBBTBox(it->name);

        QSettings settings;
        mainWindow->setCorrectionFactor(
            settings.value(QString("locomotive/") + it->name, 1.0).toDouble());
    }
}

void Core::filterBlockByMainline(int column)
{
    filterBlockAndRouteByMainline(blockSortFilterModel.get(), column);
}

void Core::filterRouteByMainline(int column)
{
    filterBlockAndRouteByMainline(routeSortFilterModel.get(), column);
}

void Core::removeBlockAndRouteMainlineFilter()
{
    blockSortFilterModel->setFilterRegularExpression("");
    routeSortFilterModel->setFilterRegularExpression("");
}

void Core::filterBlockAndRouteByMainline(QSortFilterProxyModel* model,
                                         int column)
{
    if (model != nullptr)
    {
        model->setFilterKeyColumn(column);
        model->setFilterRegularExpression("true");
    }
}

void Core::calculateBBT(const QString& locName)
{
    LocList& locList = workspace.getLocList();

    const auto search = [locName](Loc item) { return item.name == locName; };

    auto it = std::find_if(locList.begin(), locList.end(), search);

    if (it != locList.end())
    {
        bool shallOverwriteExistingValues{false};
        if (!it->bbt.empty())
        {
            int returnCode =
                mainWindow->askUserIfExistingBBTEntriesShallBeDeleted();
            if (returnCode == QMessageBox::Cancel)
            {
                return;
            }
            if (returnCode == QMessageBox::Yes)
            {
                shallOverwriteExistingValues = true;
            }
        }

        BlockList& blockList{workspace.getBlockList()};
        RouteList& routeList{workspace.getRouteList()};

        Calculation calc{&(*it), routeList, blockList,
                         shallOverwriteExistingValues};
        calc.calculateNewBBTEntries(
            mainWindow->getUserSelectedCorrectionFactor());

        QSettings settings;
        settings.setValue(QString("locomotive/") + it->name,
                          mainWindow->getUserSelectedCorrectionFactor());

        createBBTModel(locName);

        PlanParser parser(workspace.getPlanFilePath());
        parser.saveBBTForLocomotive(*it);
    }
}
