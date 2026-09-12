

#include "imagesortfilterproxymodel.hpp"

#include <QIcon>

#include "gui/bbtmodel.hpp"

using BBTCalculator::Gui::BBTModel;
using BBTCalculator::Gui::ImageSortFilterProxyModel;

ImageSortFilterProxyModel::ImageSortFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

bool ImageSortFilterProxyModel::filterAcceptsRow(
    int source_row, const QModelIndex& source_parent) const
{
    for (int i = 0; i < sourceModel()->columnCount(source_parent); ++i) {
        QModelIndex index = sourceModel()->index(source_row, i, source_parent);
        if (index.data(Qt::DecorationRole).typeId() == QMetaType::QIcon) {
            return index.data(Qt::ToolTipRole).toString().contains(
                filterRegularExpression());
        }
    }

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

bool ImageSortFilterProxyModel::lessThan(const QModelIndex& source_left,
                                         const QModelIndex& source_right) const
{
    if (source_left.data(Qt::DecorationRole).typeId() == QMetaType::QIcon)
    {
        return source_left.data(Qt::ToolTipRole).toString() >
               source_right.data(Qt::ToolTipRole).toString();
    }
    else
    {
        return QSortFilterProxyModel::lessThan(source_left, source_right);
    }
}
