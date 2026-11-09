#include <catch2/catch_test_macros.hpp>

#include <memory>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtCore/QTemporaryDir>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>

#include "core/core.hpp"
#include "gui/mainwindowcontroller.hpp"
#include "inmemorysettings.hpp"

using BBTCalculator::Core::Core;
using BBTCalculator::Gui::MainWindowController;
using BBTCalculator::Test::InMemorySettings;

namespace
{
    struct Fixture
    {
        Fixture()
        {
            static int argc   = 1;
            static char arg[] = "ut";
            static char* argv[] = {arg, nullptr};
            // Intentionally leaked: destroying QApplication during static teardown
            // crashes.
            static auto* app = new QApplication(argc, argv);
            (void)app;

            QCoreApplication::setOrganizationName("rbbt-test");
            QCoreApplication::setApplicationName("rbbt-test");
            QSettings::setDefaultFormat(QSettings::IniFormat);
            QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                               legacyDir.path());

            REQUIRE(workspaceDir.isValid());
            REQUIRE(QFile::copy(PLAN_XML, workspaceDir.filePath("plan.xml")));
            QFile::setPermissions(workspaceDir.filePath("plan.xml"),
                                  QFileDevice::ReadOwner |
                                      QFileDevice::WriteOwner);
        }

        static QDoubleSpinBox* spinBox(const Core& core)
        {
            return core.centralWidget()->findChild<QDoubleSpinBox*>(
                "correctionFactor");
        }

        QTemporaryDir legacyDir;
        QTemporaryDir workspaceDir;
        std::shared_ptr<InMemorySettings> versionable =
            std::make_shared<InMemorySettings>();
        std::shared_ptr<InMemorySettings> unversionable =
            std::make_shared<InMemorySettings>();
        MainWindowController controller;
    };
} // namespace

TEST_CASE("Core restores workspace and correction factor from the stores")
{
    Fixture f;
    f.unversionable->values["Workspace/Directory"] = f.workspaceDir.path();
    f.versionable->values["Locomotive/Loco1"]      = 1.25;

    Core core{f.versionable, f.unversionable};
    f.controller.setCore(&core);
    core.initializeApplication(f.controller);

    core.createBBTModel("Loco1");

    REQUIRE(Fixture::spinBox(core)->value() == 1.25);
}

TEST_CASE("Core defaults the correction factor to 1.0 for unknown locos")
{
    Fixture f;
    f.unversionable->values["Workspace/Directory"] = f.workspaceDir.path();

    Core core{f.versionable, f.unversionable};
    f.controller.setCore(&core);
    core.initializeApplication(f.controller);

    core.createBBTModel("Loco1");

    REQUIRE(Fixture::spinBox(core)->value() == 1.0);
}

TEST_CASE("Opening a workspace stores its directory in the unversionable store")
{
    Fixture f;
    Core core{f.versionable, f.unversionable};
    f.controller.setCore(&core);
    core.initializeApplication(f.controller);

    core.openWorkspace(f.workspaceDir.path());

    REQUIRE(f.unversionable->values["Workspace/Directory"].toString() ==
            f.workspaceDir.path());
    REQUIRE(f.versionable->empty());
}

TEST_CASE("Calculating a BBT writes the correction factor to the versionable "
          "store only")
{
    Fixture f;
    f.unversionable->values["Workspace/Directory"] = f.workspaceDir.path();

    Core core{f.versionable, f.unversionable};
    f.controller.setCore(&core);
    core.initializeApplication(f.controller);
    core.createBBTModel("Loco1");
    Fixture::spinBox(core)->setValue(0.75);

    core.calculateBBT("Loco1");

    REQUIRE(f.versionable->values["Locomotive/Loco1"].toDouble() == 0.75);
    REQUIRE_FALSE(f.unversionable->contains("Locomotive/Loco1"));
}
