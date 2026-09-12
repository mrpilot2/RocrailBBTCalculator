

#pragma once

#include <QIcon>
#include <QtCore/QAbstractTableModel>

#include "core/datastructs.hpp"

namespace BBTCalculator
{
    namespace Gui
    {
        enum class BBTViewColumns
        {
            FROM_BLOCK = 0,
            BLOCK,
            ROUTE,
            INTERVAL,
            STEPS,
            SPEED,
            COUNT,
            FIXED,
            NUMBER_OF_COLUMNS
        };

        class BBTModel : public QAbstractTableModel
        {
        public:
            explicit BBTModel(Core::BBTList lList, QObject* parent = nullptr);

            int rowCount(const QModelIndex& parent) const override;

            int columnCount(const QModelIndex& parent) const override;

            QVariant data(const QModelIndex& index, int role) const override;

            QVariant headerData(int section, Qt::Orientation orientation,
                                int role) const override;

        private:
            Core::BBTList bbtList;

            QIcon failIcon;
            QIcon passIcon;
        };
    } // namespace Gui
} // namespace BBTCalculator
