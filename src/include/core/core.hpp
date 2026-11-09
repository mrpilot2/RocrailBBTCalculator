
#pragma once

#include <memory>

#include <QtCore/QPointer>
#include <QtCore/QSortFilterProxyModel>

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

            ~Core();

            Core(const Core&)            = delete;
            Core& operator=(const Core&) = delete;

            auto centralWidget() const -> QWidget*;

            void initializeApplication(Gui::MainWindowController& contr);

            void letUserSelectWorkspace();

            void displayImageForLocName(const QString& locName);

            void createBBTModel(const QString& locName);

            void filterBlockByMainline(int column);

            void filterRouteByMainline(int column);

            void removeBlockAndRouteMainlineFilter();

            void calculateBBT(const QString& locName);

        private:
            void filterBlockAndRouteByMainline(QSortFilterProxyModel* model,
                                               int column);

            void loadLastOpenWorkspace();

            void loadWorkspace(const QString& userSelectedDirectory);

            // Owned by the aIDE main window once embedded as central widget;
            // deleted in ~Core only while still unparented.
            QPointer<Gui::MainWindow> mainWindow;

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
