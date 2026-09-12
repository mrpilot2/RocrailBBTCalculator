

#include "workspace.hpp"

#include <workspace.hpp>

using BBTCalculator::Core::BlockList;
using BBTCalculator::Core::LocList;
using BBTCalculator::Core::RouteList;
using BBTCalculator::Core::Workspace;

void Workspace::setRootPath(const QDir& root)
{
    rootPath = root;

    planFilePath = rootPath.absoluteFilePath("plan.xml").toStdString();
}

auto Workspace::getPlanFilePath() const -> const std::string&
{
    return planFilePath;
}

void Workspace::setLocList(const LocList& lList)
{
    locList = lList;

    for (auto& loc : locList) {
        QFileInfo fileInfo{getImagePath() + loc.imageName};

        if (fileInfo.exists() && fileInfo.isFile()) {
            loc.locImage = QPixmap{fileInfo.absoluteFilePath()};
        }
    }
}

auto Workspace::getLocList() -> LocList&
{
    return locList;
}

void Workspace::setBlockList(const BlockList& lList)
{
    blockList = lList;
}

auto Workspace::getBlockList() -> BlockList&
{
    return blockList;
}

void Workspace::setRouteList(const RouteList& rList)
{
    routeList = rList;
}

auto Workspace::getRouteList() -> RouteList&
{
    return routeList;
}

auto Workspace::getImagePath() const -> QString
{
    return QString(rootPath.absoluteFilePath("images/"));
}
