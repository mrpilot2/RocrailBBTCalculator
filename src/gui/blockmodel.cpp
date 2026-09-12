

#include "blockmodel.hpp"

#include <QApplication>
#include <QModelIndex>
#include <QtGui/QIcon>

using BBTCalculator::Core::BlockList;
using BBTCalculator::Gui::BlockModel;

BlockModel::BlockModel(BlockList& lList, QObject* parent)
    : QAbstractTableModel(parent)
    , blockList{lList}
    , failIcon{QIcon::fromTheme("fail", QIcon(":/icons/fail"))}
    , passIcon{QIcon::fromTheme("pass", QIcon(":/icons/pass"))}
{}

int BlockModel::rowCount(const QModelIndex& parent) const
{
    return static_cast<int>(blockList.size());
}

int BlockModel::columnCount(const QModelIndex& parent) const
{
    return static_cast<int>(BlockViewColumns::NUMBER_OF_COLUMNS);
}

QVariant BlockModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) // TODO: check index in range
        return QVariant();

    if (role == Qt::TextAlignmentRole) { return Qt::AlignCenter; }
    if (role == Qt::DisplayRole) {
        switch (static_cast<BlockViewColumns>(index.column())) {
        case BlockViewColumns::NAME:
            return blockList.at(static_cast<unsigned long>(index.row())).name;
        case BlockViewColumns::LEN:
            return blockList.at(static_cast<unsigned long>(index.row())).length;
        default:
            return QVariant();
        }
    }
    if (role == Qt::ToolTipRole &&
        index.column() == static_cast<int>(BlockViewColumns::MAINLINE)) {
        return blockList.at(static_cast<unsigned long>(index.row())).isMainLine;
    }
    if (role == Qt::DecorationRole &&
        index.column() == static_cast<int>(BlockViewColumns::MAINLINE)) {
        const bool isMainLine{
            blockList.at(static_cast<unsigned long>(index.row())).isMainLine};

        return isMainLine ? passIcon : failIcon;
    }
    return QVariant();
}

QVariant BlockModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    if (role != Qt::DisplayRole) // TODO: check index in rage
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (static_cast<BlockViewColumns>(section)) {
        case BlockViewColumns::MAINLINE:
            return QApplication::translate("BlockModel", "Mainline");
        case BlockViewColumns::NAME:
            return QApplication::translate("BlockModel", "Name");
        case BlockViewColumns::LEN:
            return QApplication::translate("BlockModel", "Length");
        default:
            return QVariant();
        }
    }
    return QVariant();
}
