#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QString>

#include <aide/hierarchicalid.hpp>

namespace BBTCalculator::Core::SettingsKeys
{
    inline aide::HierarchicalId workspaceDirectory()
    {
        return aide::HierarchicalId("Workspace")("Directory");
    }

    /// Calls @p f with the correction-factor key of the given locomotive.
    /// HierarchicalId keeps raw pointers, so the UTF-8 name buffer has to
    /// outlive the id; this helper guarantees that for the duration of @p f.
    template <typename Func>
    auto withLocomotiveKey(const QString& locomotiveName, Func&& f)
    {
        const QByteArray name{locomotiveName.toUtf8()};
        return f(aide::HierarchicalId("Locomotive")(name.constData()));
    }
} // namespace BBTCalculator::Core::SettingsKeys
