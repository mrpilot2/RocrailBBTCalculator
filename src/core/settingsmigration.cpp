#include "settingsmigration.hpp"

#include <exception>
#include <utility>

#include <QtCore/QByteArray>
#include <QtCore/QString>

#include "settingskeys.hpp"

using BBTCalculator::Core::SettingsMigration;
namespace SettingsKeys = BBTCalculator::Core::SettingsKeys;

namespace
{
    struct Mapping
    {
        const char* legacyKey;
        const char* group; // nullptr for a key directly below "View"
        const char* name;
    };

    // All targets live below "View"; literals have static storage duration,
    // which HierarchicalId (raw pointers) requires.
    const Mapping unversionableMappings[] = {
        {"showMainLineOnly", nullptr, "ShowMainlineOnly"},
        {"locView/columnSequence", "LocTable", "HeaderState"},
        {"locView/geometry", "LocTable", "HeaderGeometry"},
        {"blockView/columnSequence", "BlockTable", "HeaderState"},
        {"blockView/geometry", "BlockTable", "HeaderGeometry"},
        {"routesView/columnSequence", "RoutesTable", "HeaderState"},
        {"routesView/geometry", "RoutesTable", "HeaderGeometry"},
        {"bbtView/columnSequence", "BBTTable", "HeaderState"},
        {"bbtView/geometry", "BBTTable", "HeaderGeometry"},
        {"splitter/state", "Splitter1", "State"},
        {"splitter/geometry", "Splitter1", "Geometry"},
        {"splitter2/state", "Splitter2", "State"},
        {"splitter2/geometry", "Splitter2", "Geometry"},
    };

    aide::HierarchicalId idFor(const Mapping& m)
    {
        if (m.group == nullptr) { return aide::HierarchicalId("View")(m.name); }
        return aide::HierarchicalId("View")(m.group)(m.name);
    }
} // namespace

SettingsMigration::SettingsMigration(
    QSettings& legacySettings,
    std::shared_ptr<aide::SettingsInterface> versionableSettings,
    std::shared_ptr<aide::SettingsInterface> unversionableSettings)
    : legacy{legacySettings}
    , versionable{std::move(versionableSettings)}
    , unversionable{std::move(unversionableSettings)}
{}

aide::HierarchicalId SettingsMigration::markerKey()
{ return aide::HierarchicalId("Migration")("LegacySettingsMigrated"); }

bool SettingsMigration::run()
{
    try {
        if (unversionable->value(markerKey(), false).toBool()) { return true; }

        if (legacy.contains("workspaceDirectory")) {
            unversionable->setValue(
                SettingsKeys::workspaceDirectory(),
                legacy.value("workspaceDirectory"));
        }

        for (const auto& mapping : unversionableMappings) {
            if (legacy.contains(mapping.legacyKey)) {
                unversionable->setValue(idFor(mapping),
                                        legacy.value(mapping.legacyKey));
            }
        }

        const QString prefix{"locomotive/"};
        for (const QString& key : legacy.allKeys()) {
            if (!key.startsWith(prefix)) { continue; }
            SettingsKeys::withLocomotiveKey(
                key.mid(prefix.size()),
                [&](const aide::HierarchicalId& id) {
                    versionable->setValue(id, legacy.value(key).toDouble());
                    return 0;
                });
        }

        // Persist the marker only after everything else was saved.
        versionable->save();
        unversionable->setValue(markerKey(), true);
        unversionable->save();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}
