#pragma once

#include <memory>

#include <QtCore/QSettings>

#include <aide/hierarchicalid.hpp>
#include <aide/settingsinterface.hpp>

namespace BBTCalculator::Core
{
    /// One-time copy of legacy QSettings values into the aIDE stores.
    ///
    /// Correction factors go to the versionable store, everything else to the
    /// unversionable store. A marker is written only after all copies
    /// succeeded; legacy keys are never removed.
    class SettingsMigration
    {
    public:
        SettingsMigration(
            QSettings& legacySettings,
            std::shared_ptr<aide::SettingsInterface> versionableSettings,
            std::shared_ptr<aide::SettingsInterface> unversionableSettings);

        /// @return true if the migration is complete (now or earlier),
        ///         false if it failed and will be retried on the next run.
        bool run();

        static aide::HierarchicalId markerKey();

    private:
        QSettings& legacy;
        std::shared_ptr<aide::SettingsInterface> versionable;
        std::shared_ptr<aide::SettingsInterface> unversionable;
    };
} // namespace BBTCalculator::Core
