
#pragma once

#include <memory>

#include <QtCore/QAbstractTableModel>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

#include <aide/settingsinterface.hpp>

#include "mainwindowcontroller.hpp"

namespace Ui
{
    class BBTMainWindow;
} // namespace Ui

namespace BBTCalculator
{
    namespace Gui
    {
        class MainWindow : public QWidget
        {
        public:
            explicit MainWindow(
                std::shared_ptr<aide::SettingsInterface> unversionableSettings,
                QWidget* parent = nullptr);

            ~MainWindow() override;

            void setController(MainWindowController* contr);

            bool eventFilter(QObject* watched, QEvent* event) override;

            void retranslateUi();

            void saveState();

            auto letUserSelectWorkspaceDirectory() -> QString;

            void notifyUserSelectedDirectoryDoesNotExist();

            void showRootPath(const QString& path);

            void setLocTableModel(QAbstractItemModel* model);

            void setBlockTableModel(QAbstractItemModel* model);

            void setRouteTableModel(QAbstractItemModel* model);

            void displayLocImage(QPixmap locImage);

            void setBBTTableModel(QAbstractItemModel* model);

            void setLocNameInBBTBox(const QString& locName);

            void setCorrectionFactor(double correctionFactor);

            auto getUserSelectedCorrectionFactor() const -> double;

            auto askUserIfExistingBBTEntriesShallBeDeleted() -> int;

        protected:
            void changeEvent(QEvent* event) override;

        private:
            void setupTable(QTableView* table) const;

            void saveHeader(QTableView* table, const char* group);
            void restoreHeader(QTableView* table, const char* group);
            void restoreSplitter(QSplitter* splitter, const char* group);

            void connectSignals();

            std::shared_ptr<aide::SettingsInterface> settings;

            Ui::BBTMainWindow* ui;

            MainWindowController* controller;

            QPixmap originalLocImage;
        };
    } // namespace Gui
} // namespace BBTCalculator