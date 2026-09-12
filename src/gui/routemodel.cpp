

#include "routemodel.hpp"

#include <QApplication>
#include <QModelIndex>
#include <QtGui/QIcon>

using BBTCalculator::Core::RouteList;
using BBTCalculator::Gui::RouteModel;

RouteModel::RouteModel(RouteList& lList, QObject* parent)
    : QAbstractTableModel(parent)
    , routeList{lList}
    , failIcon{QIcon::fromTheme("fail", QIcon(":/icons/fail"))}
    , passIcon{QIcon::fromTheme("pass", QIcon(":/icons/pass"))}
{}

int RouteModel::rowCount(const QModelIndex& parent) const
{
    return static_cast<int>(routeList.size());
}

int RouteModel::columnCount(const QModelIndex& parent) const
{
    return static_cast<int>(RouteViewColumns::NUMBER_OF_COLUMNS);
}

QVariant RouteModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) // TODO: check index in range
        return QVariant();

    if (role == Qt::TextAlignmentRole) { return Qt::AlignCenter; }
    if (role == Qt::DisplayRole) {
        switch (static_cast<RouteViewColumns>(index.column())) {
        case RouteViewColumns::NAME:
            return routeList.at(static_cast<unsigned long>(index.row())).id;
        case RouteViewColumns::FROM_BLOCK:
            return routeList.at(static_cast<unsigned long>(index.row()))
                .fromBlock;
        case RouteViewColumns::TO_BLOCK:
            return routeList.at(static_cast<unsigned long>(index.row()))
                .toBlock;
        default:
            return QVariant();
        }
    }
    if (role == Qt::ToolTipRole) {
        if (index.column() == static_cast<int>(RouteViewColumns::STRAIGHT)) {
            return routeList.at(static_cast<unsigned long>(index.row()))
                .isCompletelyStraight;
        }
        if (index.column() == static_cast<int>(RouteViewColumns::MAINLINE)) {
            return routeList.at(static_cast<unsigned long>(index.row()))
                .isOnlyMainline;
        }
        if (index.column() == static_cast<int>(RouteViewColumns::REDUCEV)) {
            return routeList.at(static_cast<unsigned long>(index.row()))
                .shallReduceVelocity;
        }
    }
    if (role == Qt::DecorationRole) {
        if (index.column() == static_cast<int>(RouteViewColumns::STRAIGHT)) {
            const bool isStraight{
                routeList.at(static_cast<unsigned long>(index.row()))
                    .isCompletelyStraight};

            return isStraight ? passIcon : failIcon;
        }
        if (index.column() == static_cast<int>(RouteViewColumns::MAINLINE)) {
            const bool isMainLine{
                routeList.at(static_cast<unsigned long>(index.row()))
                    .isOnlyMainline};

            return isMainLine ? passIcon : failIcon;
        }
        if (index.column() == static_cast<int>(RouteViewColumns::REDUCEV)) {
            const bool reduceV{
                routeList.at(static_cast<unsigned long>(index.row()))
                    .shallReduceVelocity};

            return reduceV ? passIcon : failIcon;
        }
    }
    return QVariant();
}

QVariant RouteModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    if (role != Qt::DisplayRole) // TODO: check index in rage
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (static_cast<RouteViewColumns>(section)) {
        case RouteViewColumns::MAINLINE:
            return QApplication::translate("RouteModel", "Mainline");
        case RouteViewColumns::NAME:
            return QApplication::translate("RouteModel", "Name");
        case RouteViewColumns::FROM_BLOCK:
            return QApplication::translate("RouteModel", "From Block");
        case RouteViewColumns::TO_BLOCK:
            return QApplication::translate("RouteModel", "To Block");
        case RouteViewColumns::STRAIGHT:
            return QApplication::translate("RouteModel", "Straight");
        case RouteViewColumns::REDUCEV:
            return QApplication::translate("RouteModel", "Reduce velocity");
        default:
            return QVariant();
        }
    }
    return QVariant();
}
