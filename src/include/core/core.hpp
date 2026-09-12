
#pragma once

#include <memory>

#include <QtCore/QSortFilterProxyModel>
#include <QtCore/QTranslator>

#include "gui/bbtmodel.hpp"
#include "gui/blockmodel.hpp"
#include "gui/locmodel.hpp"
#include "gui/routemodel.hpp"
#include "workspace.hpp"

namespace BBTCalculator
{
    namespace Gui
    {
        class MainWindowController;

        class MainWindow;
    } // namespace Gui
} // namespace BBTCalculator

namespace BBTCalculator
{
    namespace Core
    {
        class Core
        {
        public:
            Core();

            void initializeApplication(Gui::MainWindowController& contr);

            void letUserSelectWorkspace();

            void displayImageForLocName(const QString& locName);

            void createBBTModel(const QString& locName);

            void filterBlockByMainline(int column);

            void filterRouteByMainline(int column);

            void removeBlockAndRouteMainlineFilter();

            void calculateBBT(const QString& locName);

        private:
            void setupTranslator();

            void filterBlockAndRouteByMainline(QSortFilterProxyModel* model,
                                               int column);

            void loadLastOpenWorkspace();

            void loadWorkspace(const QString& userSelectedDirectory);

            std::shared_ptr<Gui::MainWindow> mainWindow;

            QTranslator translator;
            Workspace workspace;
            std::unique_ptr<Gui::LocModel> locModel;
            std::unique_ptr<Gui::BlockModel> blockModel;

            std::unique_ptr<Gui::RouteModel> routeModel;
            std::unique_ptr<Gui::BBTModel> bbtModel;
            std::unique_ptr<QSortFilterProxyModel> locSortFilterModel;
            std::unique_ptr<QSortFilterProxyModel> blockSortFilterModel;

            std::unique_ptr<QSortFilterProxyModel> routeSortFilterModel;

            std::unique_ptr<QSortFilterProxyModel> bbtSortFilterModel;
        };
    } // namespace Core
} // namespace BBTCalculator
