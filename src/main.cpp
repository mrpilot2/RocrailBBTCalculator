
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include <aide/application.hpp>

#include "core/core.hpp"
#include "gui/mainwindowcontroller.hpp"

using BBTCalculator::Core::Core;
using BBTCalculator::Gui::MainWindowController;

int main(int argc, char* argv[])
{
    aide::Application::setOrganizationName("mrpilot");
    aide::Application::setOrganizationDomain("https://github.com/mrpilot2");
    aide::Application::setApplicationName("rocrailBBTCalculator");
    aide::Application::setApplicationDisplayName("Rocrail BBT Calculator");

    const aide::Application application(argc, argv);

    Q_INIT_RESOURCE(application);

    QApplication::setWindowIcon(QIcon(":/icons/appicon"));
    Core core{};

    MainWindowController controller{};
    controller.setCore(&core);

    core.initializeApplication(controller);

    return aide::Application::exec();
}