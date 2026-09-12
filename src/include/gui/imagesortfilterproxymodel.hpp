
#pragma once

#include <QSortFilterProxyModel>

namespace BBTCalculator
{
    namespace Gui
    {
        class ImageSortFilterProxyModel : public QSortFilterProxyModel
        {
        public:
            explicit ImageSortFilterProxyModel(QObject* parent = nullptr);

        protected:
            bool filterAcceptsRow(
                int source_row,
                const QModelIndex& source_parent) const override;

            bool lessThan(const QModelIndex& source_left,
                          const QModelIndex& source_right) const override;
        };

    } // end namespace Gui

} // end namespace BBTCalculator
