
#pragma once

#include <memory>

#include <QtCore/QAbstractTableModel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTableView>

#include "mainwindowcontroller.hpp"

namespace Ui
{
    class BBTMainWindow;
} // namespace Ui

namespace BBTCalculator
{
    namespace Gui
    {
        class MainWindow : public QMainWindow
        {
        public:
            explicit MainWindow(QWidget* parent = nullptr);

            ~MainWindow() override;

            void setController(MainWindowController* contr);

            bool eventFilter(QObject* watched, QEvent* event) override;

            void retranslateUi();

            void closeEvent(QCloseEvent* event) override;

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

        private:
            void setupTable(QTableView* table) const;

            void setupMenuActions();

            void connectSignals();

            Ui::BBTMainWindow* ui;

            MainWindowController* controller;

            QPixmap originalLocImage;
        };
    } // namespace Gui
} // namespace BBTCalculator