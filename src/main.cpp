
#include <QtCore/QDir>
#include <QtCore/QEvent>
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

namespace
{
    void retranslateOpenWorkspace(QAction* action)
    {
        action->setText(
            QApplication::translate("MainWindow", "&Open workspace"));
        action->setStatusTip(QApplication::translate(
            "MainWindow", "Opens an existing Rocrail workspace"));
    }

    /// Updates the action texts when the application language changes.
    class ActionRetranslator : public QObject
    {
    public:
        ActionRetranslator(QAction* a, QObject* parent)
            : QObject(parent)
            , action{a}
        {}

        bool eventFilter(QObject* watched, QEvent* event) override
        {
            if (event->type() == QEvent::LanguageChange) {
                retranslateOpenWorkspace(action);
            }
            return QObject::eventFilter(watched, event);
        }

    private:
        QAction* action;
    };

    void registerOpenWorkspaceAction(const aide::Application& application,
                                     QWidget* mainWindow,
                                     MainWindowController& controller)
    {
        const auto actionRegistry = application.actionRegistry();
        const auto fileMenu       = actionRegistry->getMenuContainer(
            aide::constants::CONSTANTS().MENU_FILE);
        if (!fileMenu.has_value()) {
            qWarning(
                "File menu not found, Open Workspace action not "
                "registered");
            return;
        }

        auto* menu                = fileMenu.value()->menu();
        auto* actionOpenWorkspace = new QAction(menu);
        retranslateOpenWorkspace(actionOpenWorkspace);
        QObject::connect(actionOpenWorkspace, &QAction::triggered, &controller,
                         &MainWindowController::onOpenWorkspaceClicked);

        mainWindow->installEventFilter(
            new ActionRetranslator(actionOpenWorkspace, actionOpenWorkspace));

        menu->insertAction(
            actionRegistry->action(aide::constants::CONSTANTS().FILE_SETTINGS)
                .value_or(nullptr),
            actionOpenWorkspace);
        actionRegistry->registerAction(
            actionOpenWorkspace,
            aide::HierarchicalId("Main Menu")("File")("Open Workspace"),
            {QKeySequence(QKeySequence::Open)});
    }
} // namespace

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
        if (!migration.run()) {
            qWarning(
                "Migration of legacy settings failed; it will be "
                "retried on the next start");
        }
    }

    Core core{aide::AideSettingsProvider::versionableSettings(),
              aide::AideSettingsProvider::unversionableSettings()};

    MainWindowController controller{};
    controller.setCore(&core);

    const auto mainWindow = application.mainWindow();
    mainWindow->setCentralWidget(core.centralWidget());

    registerOpenWorkspaceAction(application, core.centralWidget(), controller);

    mainWindow->show();

    core.initializeApplication(controller);

    return aide::Application::exec();
}