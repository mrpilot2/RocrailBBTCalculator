

#include "bbtmodel.hpp"

#include <QModelIndex>

#include <QtGui/QIcon>

using BBTCalculator::Core::BBTList;
using BBTCalculator::Gui::BBTModel;

BBTModel::BBTModel(BBTList lList, QObject* parent)
    : QAbstractTableModel(parent)
    , bbtList{lList}
{
}

int BBTModel::rowCount(const QModelIndex& parent) const
{
    return static_cast<int>(bbtList.size());
}

int BBTModel::columnCount(const QModelIndex& parent) const
{
    return static_cast<int>(BBTViewColumns::NUMBER_OF_COLUMNS);
}

QVariant BBTModel::data(const QModelIndex& index, int role) const
{
    if (not index.isValid()) // TODO: check index in range
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        switch (static_cast<BBTViewColumns>(index.column()))
        {
        case BBTViewColumns::BLOCK:
            return bbtList.at(static_cast<unsigned long>(index.row())).block;
        case BBTViewColumns::FROM_BLOCK:
            return bbtList.at(static_cast<unsigned long>(index.row()))
                .fromBlock;
        case BBTViewColumns::ROUTE:
            return bbtList.at(static_cast<unsigned long>(index.row())).route;
        case BBTViewColumns::INTERVAL:
            return bbtList.at(static_cast<unsigned long>(index.row())).interval;
        case BBTViewColumns::STEPS:
            return bbtList.at(static_cast<unsigned long>(index.row())).steps;
        case BBTViewColumns::SPEED:
            return bbtList.at(static_cast<unsigned long>(index.row())).speed;
        case BBTViewColumns::COUNT:
            return bbtList.at(static_cast<unsigned long>(index.row())).count;
        default:
            return QVariant();
        }
    }
    if (role == Qt::DecorationRole &&
        index.column() == static_cast<int>(BBTViewColumns::FIXED))
    {
        const bool isFixed{
            bbtList.at(static_cast<unsigned long>(index.row())).isFixed};

        return isFixed ? QIcon::fromTheme("pass", QIcon(":/icons/pass"))
                       : QIcon::fromTheme("fail", QIcon(":/icons/fail"));
    }
    return QVariant();
}

QVariant BBTModel::headerData(int section, Qt::Orientation orientation,
                              int role) const
{
    if (role != Qt::DisplayRole) // TODO: check index in rage
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (static_cast<BBTViewColumns>(section))
        {
        case BBTViewColumns::BLOCK:
            return tr("Block");
        case BBTViewColumns::FROM_BLOCK:
            return tr("From Block");
        case BBTViewColumns::ROUTE:
            return tr("Route");
        case BBTViewColumns::INTERVAL:
            return tr("Interval");
        case BBTViewColumns::STEPS:
            return tr("Steps");
        case BBTViewColumns::SPEED:
            return tr("Speed");
        case BBTViewColumns::COUNT:
            return tr("Counter");
        case BBTViewColumns::FIXED:
            return tr("Fixed");
        default:
            return QVariant();
        }
    }
    return QVariant();
}