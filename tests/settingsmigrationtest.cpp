#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>

#include <QtCore/QByteArray>
#include <QtCore/QSettings>
#include <QtCore/QTemporaryDir>

#include "core/settingsmigration.hpp"
#include "inmemorysettings.hpp"

using BBTCalculator::Core::SettingsMigration;
using BBTCalculator::Test::InMemorySettings;

namespace
{
    /// Fails every write once `allowedWrites` writes have happened.
    class FailingSettings : public InMemorySettings
    {
    public:
        explicit FailingSettings(int allowedWrites)
            : remaining{allowedWrites}
        {}

        void setValue(const aide::HierarchicalId& key,
                      const QVariant& value) override
        {
            if (remaining-- <= 0) { throw std::runtime_error("disk full"); }
            InMemorySettings::setValue(key, value);
        }

        int remaining;
    };

    class SaveFailingSettings : public InMemorySettings
    {
    public:
        void save() override { throw std::runtime_error("save failed"); }
    };

    const QByteArray blob{"\x01\x02\x03", 3};

    struct Fixture
    {
        Fixture()
            : dir{}
            , legacy{dir.filePath("legacy.ini"), QSettings::IniFormat}
            , versionable{std::make_shared<InMemorySettings>()}
            , unversionable{std::make_shared<InMemorySettings>()}
        {}

        void populateLegacy()
        {
            legacy.setValue("workspaceDirectory", "/home/me/rocrail");
            legacy.setValue("locomotive/BR 80", 1.25);
            legacy.setValue("locomotive/V 200", 0.8);
            legacy.setValue("showMainLineOnly", true);
            legacy.setValue("locView/columnSequence", blob);
            legacy.setValue("locView/geometry", blob);
            legacy.setValue("blockView/columnSequence", blob);
            legacy.setValue("blockView/geometry", blob);
            legacy.setValue("routesView/columnSequence", blob);
            legacy.setValue("routesView/geometry", blob);
            legacy.setValue("bbtView/columnSequence", blob);
            legacy.setValue("bbtView/geometry", blob);
            legacy.setValue("splitter/state", blob);
            legacy.setValue("splitter/geometry", blob);
            legacy.setValue("splitter2/state", blob);
            legacy.setValue("splitter2/geometry", blob);
            legacy.setValue("mainwindow/geometry", blob);
            legacy.sync();
        }

        bool migrate(std::shared_ptr<aide::SettingsInterface> unver = nullptr)
        {
            SettingsMigration migration{legacy, versionable,
                                        unver ? unver : unversionable};
            return migration.run();
        }

        QTemporaryDir dir;
        QSettings legacy;
        std::shared_ptr<InMemorySettings> versionable;
        std::shared_ptr<InMemorySettings> unversionable;
    };

    std::string name(const char* a, const char* b)
    { return aide::HierarchicalId(a)(b).name(); }

    std::string name(const char* a, const char* b, const char* c)
    { return aide::HierarchicalId(a)(b)(c).name(); }
} // namespace

TEST_CASE("Populated legacy settings are migrated to the right stores")
{
    Fixture f;
    f.populateLegacy();

    REQUIRE(f.migrate());

    SECTION("correction factors go to the versionable store only")
    {
        REQUIRE(f.versionable->values.size() == 2);
        CHECK(f.versionable->values.at(name("Locomotive", "BR 80")).toDouble()
              == 1.25);
        CHECK(f.versionable->values.at(name("Locomotive", "V 200")).toDouble()
              == 0.8);
        CHECK_FALSE(f.unversionable->contains(name("Locomotive", "BR 80")));
    }

    SECTION("everything else goes to the unversionable store")
    {
        auto& v = f.unversionable->values;
        CHECK(v.at(name("Workspace", "Directory")).toString()
              == "/home/me/rocrail");
        CHECK(v.at(name("View", "ShowMainlineOnly")).toBool());

        const std::pair<const char*, const char*> tables[] = {
            {"LocTable", "locView"},
            {"BlockTable", "blockView"},
            {"RoutesTable", "routesView"},
            {"BBTTable", "bbtView"}};
        for (const auto& [table, legacyName] : tables) {
            INFO(table);
            CHECK(v.at(name("View", table, "HeaderState")).toByteArray()
                  == blob);
            CHECK(v.at(name("View", table, "HeaderGeometry")).toByteArray()
                  == blob);
        }
        for (const char* splitter : {"Splitter1", "Splitter2"}) {
            INFO(splitter);
            CHECK(v.at(name("View", splitter, "State")).toByteArray() == blob);
            CHECK(v.at(name("View", splitter, "Geometry")).toByteArray()
                  == blob);
        }
    }

    SECTION("main window geometry is not copied")
    {
        for (const auto& [key, value] : f.unversionable->values) {
            CHECK(key.find("mainwindow") == std::string::npos);
            CHECK(key.find("Mainwindow") == std::string::npos);
            CHECK(key.find("MainWindow") == std::string::npos);
        }
        // 1 dir + 1 mainline + 8 header + 4 splitter + marker
        CHECK(f.unversionable->values.size() == 15);
    }

    SECTION("legacy keys remain untouched")
    {
        CHECK(f.legacy.allKeys().size() == 17);
        CHECK(f.legacy.value("workspaceDirectory").toString()
              == "/home/me/rocrail");
        CHECK(f.legacy.contains("mainwindow/geometry"));
        CHECK(f.legacy.contains("locomotive/BR 80"));
    }
}

TEST_CASE("A successful run sets the marker")
{
    Fixture f;
    f.populateLegacy();
    REQUIRE(f.migrate());
    CHECK(f.unversionable->contains(SettingsMigration::markerKey().name()));
    CHECK_FALSE(f.versionable->contains(SettingsMigration::markerKey().name()));
}

TEST_CASE("An empty legacy store migrates cleanly and sets the marker")
{
    Fixture f;

    REQUIRE(f.migrate());

    CHECK(f.versionable->empty());
    CHECK(f.unversionable->values.size() == 1);
    CHECK(f.unversionable->contains(SettingsMigration::markerKey().name()));
}

TEST_CASE("A second run changes nothing")
{
    Fixture f;
    f.populateLegacy();
    REQUIRE(f.migrate());

    f.unversionable->setValue(aide::HierarchicalId("Workspace")("Directory"),
                              "/edited");
    f.versionable->setValue(aide::HierarchicalId("Locomotive")("BR 80"), 9.0);
    f.legacy.setValue("locomotive/New", 2.0);
    const auto unverBefore = f.unversionable->values;
    const auto verBefore   = f.versionable->values;

    REQUIRE(f.migrate());

    CHECK(f.unversionable->values == unverBefore);
    CHECK(f.versionable->values == verBefore);
}

TEST_CASE("A failure mid-run leaves the marker unset so the next run retries")
{
    Fixture f;
    f.populateLegacy();

    auto failing = std::make_shared<FailingSettings>(3);
    CHECK_FALSE(f.migrate(failing));
    CHECK_FALSE(failing->contains(SettingsMigration::markerKey().name()));

    // next start: healthy stores, retry completes the migration
    failing->remaining = 1000; // partial writes from the first run persist
    REQUIRE(f.migrate(failing));
    CHECK(failing->contains(SettingsMigration::markerKey().name()));
    CHECK(failing->contains(name("Workspace", "Directory")));
    CHECK(failing->contains(name("View", "Splitter2", "Geometry")));
}

TEST_CASE("A failing save of the versionable store leaves the marker unset")
{
    Fixture f;
    f.populateLegacy();

    auto failingVersionable = std::make_shared<SaveFailingSettings>();
    SettingsMigration migration{f.legacy, failingVersionable, f.unversionable};

    CHECK_FALSE(migration.run());
    CHECK_FALSE(f.unversionable->contains(SettingsMigration::markerKey().name()));
}
