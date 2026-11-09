#pragma once

#include <map>
#include <string>

#include <aide/hierarchicalid.hpp>
#include <aide/settingsinterface.hpp>

namespace BBTCalculator::Test
{
    /// In-memory aide::SettingsInterface used as the test seam.
    class InMemorySettings : public aide::SettingsInterface
    {
    public:
        void setValue(const aide::HierarchicalId& key,
                      const QVariant& value) override
        {
            values[key.name()] = value;
        }

        QVariant value(const aide::HierarchicalId& key) override
        {
            return value(key, QVariant{});
        }

        QVariant value(const aide::HierarchicalId& key,
                       const QVariant& defaultValue) override
        {
            const auto it = values.find(key.name());
            return it == values.end() ? defaultValue : it->second;
        }

        void removeKey(const aide::HierarchicalId& key) override
        {
            values.erase(key.name());
        }

        void save() override {}
        void load() override {}

        [[nodiscard]] bool contains(const std::string& name) const
        {
            return values.count(name) != 0;
        }

        [[nodiscard]] bool empty() const { return values.empty(); }

        std::map<std::string, QVariant> values;
    };
} // namespace BBTCalculator::Test
