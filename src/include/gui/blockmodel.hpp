

#pragma once

#include <QIcon>
#include <QtCore/QAbstractTableModel>

#include "core/datastructs.hpp"

namespace BBTCalculator
{
    namespace Gui
    {
        enum class BlockViewColumns
        {
            MAINLINE = 0,
            NAME,
            LEN,
            NUMBER_OF_COLUMNS
        };

        class BlockModel : public QAbstractTableModel
        {
        public:
            explicit BlockModel(Core::BlockList& lList,
                                QObject* parent = nullptr);

            int rowCount(const QModelIndex& parent) const override;

            int columnCount(const QModelIndex& parent) const override;

            QVariant data(const QModelIndex& index, int role) const override;

            QVariant headerData(int section, Qt::Orientation orientation,
                                int role) const override;

        private:
            Core::BlockList& blockList;

            QIcon failIcon;
            QIcon passIcon;
        };
    } // namespace Gui
} // namespace BBTCalculator
