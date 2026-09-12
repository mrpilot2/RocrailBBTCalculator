

#include "locmodel.hpp"

#include <QApplication>
#include <QModelIndex>
#include <QtGui/QIcon>

using BBTCalculator::Core::LocList;
using BBTCalculator::Gui::LocModel;

LocModel::LocModel(LocList& lList, QObject* parent)
    : QAbstractTableModel(parent)
    , locList{lList}
    , failIcon{QIcon::fromTheme("fail", QIcon(":/icons/fail"))}
    , passIcon{QIcon::fromTheme("pass", QIcon(":/icons/pass"))}
{}

int LocModel::rowCount(const QModelIndex& parent) const
{
    return static_cast<int>(locList.size());
}

int LocModel::columnCount(const QModelIndex& parent) const
{
    return static_cast<int>(LocViewColumns::NUMBER_OF_COLUMNS);
}

QVariant LocModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) // TODO: check index in range
        return QVariant();

    if (role == Qt::TextAlignmentRole) { return Qt::AlignCenter; }
    if (role == Qt::DisplayRole) {
        switch (static_cast<LocViewColumns>(index.column())) {
        case LocViewColumns::NAME:
            return locList.at(static_cast<unsigned long>(index.row())).name;
        case LocViewColumns::BBT_STEPS:
            return locList.at(static_cast<unsigned long>(index.row())).bbtSteps;
        case LocViewColumns::V_MIN:
            return locList.at(static_cast<unsigned long>(index.row())).v_min;
        case LocViewColumns::V_MID:
            return locList.at(static_cast<unsigned long>(index.row())).v_mid;
        case LocViewColumns::V_CRU:
            return locList.at(static_cast<unsigned long>(index.row())).v_cru;
        default:
            return QVariant();
        }
    }
    if (role == Qt::ToolTipRole &&
        index.column() == static_cast<int>(LocViewColumns::BBT)) {
        return locList.at(static_cast<unsigned long>(index.row())).useBBT;
    }
    if (role == Qt::DecorationRole &&
        index.column() == static_cast<int>(LocViewColumns::BBT)) {
        const bool useBBT{
            locList.at(static_cast<unsigned long>(index.row())).useBBT};

        return useBBT ? passIcon : failIcon;
    }
    return QVariant();
}

QVariant LocModel::headerData(int section, Qt::Orientation orientation,
                              int role) const
{
    if (role != Qt::DisplayRole) // TODO: check index in rage
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (static_cast<LocViewColumns>(section)) {
        case LocViewColumns::BBT:
            return QApplication::translate("LocModel", "BBT");
        case LocViewColumns::NAME:
            return QApplication::translate("LocModel", "Name");
        case LocViewColumns::BBT_STEPS:
            return QApplication::translate("LocModel", "BBT Steps");
        case LocViewColumns::V_MIN:
            return QApplication::translate("LocModel", "V min");
        case LocViewColumns::V_MID:
            return QApplication::translate("LocModel", "V mid");
        case LocViewColumns::V_CRU:
            return QApplication::translate("LocModel", "V cru");
        default:
            return QVariant();
        }
    }
    return QVariant();
}
