
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMenu>

#include <aide/aideconstants.hpp>
#include <aide/aidesettingsprovider.hpp>
#include <aide/application.hpp>
#include <aide/gui/translatorinterface.hpp>
#include <aide/hierarchicalid.hpp>
#include <aide/menucontainerinterface.hpp>

#include "core/core.hpp"
#include "core/settingsmigration.hpp"
#include "gui/mainwindowcontroller.hpp"

using BBTCalculator::Core::Core;
using BBTCalculator::Gui::MainWindowController;

int main(int argc, char* argv[])
{
    aide::Application::setOrganizationName("mrpilot");
    aide::Application::setOrganizationDomain("http://github.com/mrpilot2");
    aide::Application::setApplicationName("rocrailBBTCalculator");
    aide::Application::setApplicationDisplayName("Rocrail BBT Calculator");

    const aide::Application application(argc, argv);

    Q_INIT_RESOURCE(application);

    application.translator()->addAdditionalTranslationFilePath(
        QDir(":/translations"), QLatin1String("rocrailBBT"));

    QApplication::setWindowIcon(QIcon(":/icons/appicon"));

    {
        QSettings legacySettings;
        BBTCalculator::Core::SettingsMigration migration{
            legacySettings, aide::AideSettingsProvider::versionableSettings(),
            aide::AideSettingsProvider::unversionableSettings()};
        migration.run();
    }

    Core core{aide::AideSettingsProvider::versionableSettings(),
              aide::AideSettingsProvider::unversionableSettings()};

    MainWindowController controller{};
    controller.setCore(&core);

    const auto mainWindow = application.mainWindow();
    mainWindow->setCentralWidget(core.centralWidget());

    const auto actionRegistry = application.actionRegistry();
    if (const auto fileMenu = actionRegistry->getMenuContainer(
            aide::constants::CONSTANTS().MENU_FILE);
        fileMenu.has_value()) {
        auto* menu                = fileMenu.value()->menu();
        auto* actionOpenWorkspace = new QAction(
            QApplication::translate("MainWindow", "&Open workspace"), menu);
        actionOpenWorkspace->setStatusTip(QApplication::translate(
            "MainWindow", "Opens an existing Rocrail workspace"));
        QObject::connect(actionOpenWorkspace, &QAction::triggered, &controller,
                         &MainWindowController::onOpenWorkspaceClicked);

        menu->insertAction(
            actionRegistry->action(aide::constants::CONSTANTS().FILE_SETTINGS)
                .value_or(nullptr),
            actionOpenWorkspace);
        actionRegistry->registerAction(
            actionOpenWorkspace,
            aide::HierarchicalId("Main Menu")("File")("Open Workspace"),
            {QKeySequence(QKeySequence::Open)});
    }

    mainWindow->show();

    core.initializeApplication(controller);

    return aide::Application::exec();
}