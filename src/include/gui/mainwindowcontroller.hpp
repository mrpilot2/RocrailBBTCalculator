
#pragma once

#include <QtCore/QItemSelection>
#include <QtCore/QObject>

namespace BBTCalculator
{
    namespace Core
    {
        class Core;
    } // namespace Core
} // namespace BBTCalculator

namespace BBTCalculator
{
    namespace Gui
    {
        class MainWindowController : public QObject
        {
            Q_OBJECT
        public:
            MainWindowController();

            void setCore(BBTCalculator::Core::Core* core);

        public slots:

            void onOpenWorkspaceClicked(bool checked = false);

            void onLocSelectionChanged(const QItemSelection& selected,
                                       const QItemSelection& deselected);

            void onFilterMainlineStateChanged(int state);

            void onUserRequestsBBTCalculation();

        private:
            BBTCalculator::Core::Core* core;

            QString currentlySelectedLocName;
        };
    } // namespace Gui
} // namespace BBTCalculator