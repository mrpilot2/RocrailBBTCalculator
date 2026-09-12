

#include "planparser.hpp"

#include <planparser.hpp>

#include <QtCore/QTextStream>
#include <QtXml/QDomDocument>

#include "datastructs.hpp"

static const int indentation = 2;

using BBTCalculator::Core::BBTList;
using BBTCalculator::Core::Loc;
using BBTCalculator::Core::PlanParser;

PlanParser::PlanParser(std::string f)
    : file{QString::fromStdString(f)}
{
    const bool opened = file.open(QIODevice::ReadWrite);
    if (!opened || !file.isReadable()) {
        // TODO: handle error case
    }
}

void PlanParser::parse()
{
    QDomDocument document("plan");
    if (!document.setContent(&file)) {
        // TODO: report error
        file.close();
    }

    QDomElement root{document.documentElement()};

    QDomNodeList locs = root.elementsByTagName("lc");

    parseLocs(locs);

    QDomNodeList blocks = root.elementsByTagName("bk");

    parseBlocks(blocks);

    QDomNodeList routes = root.elementsByTagName("st");

    parseRoutes(routes);
}

void PlanParser::parseLocs(const QDomNodeList& locs)
{
    locList.reserve(static_cast<unsigned long>(locs.length()));

    for (int i = 0; i < locs.length(); ++i) {
        const auto currentLoc = locs.at(i);
        const auto currentLocAttributes{currentLoc.attributes()};

        const QDomAttr locNameAttr{
            currentLocAttributes.namedItem("id").toAttr()};
        const QDomAttr locImagePathAttr{
            currentLocAttributes.namedItem("image").toAttr()};
        const QDomAttr useBBTAttribute{
            currentLocAttributes.namedItem("usebbt").toAttr()};
        const QDomAttr bbtStepsAttribute{
            currentLocAttributes.namedItem("bbtsteps").toAttr()};
        const QDomAttr vMinAttribute{
            currentLocAttributes.namedItem("V_min").toAttr()};
        const QDomAttr vMidAttribute{
            currentLocAttributes.namedItem("V_mid").toAttr()};
        const QDomAttr vCruAttribute{
            currentLocAttributes.namedItem("V_cru").toAttr()};

        Loc loc{locNameAttr.value(),
                locImagePathAttr.value(),
                convertStringToBool(useBBTAttribute.value()),
                bbtStepsAttribute.value().toInt(),
                vMinAttribute.value().toInt(),
                vMidAttribute.value().toInt(),
                vCruAttribute.value().toInt()};

        loc.bbt = parseBBT(currentLoc);

        locList.emplace_back(loc);
    }
}

BBTList PlanParser::parseBBT(const QDomNode& node)
{
    QDomNodeList bbts = node.toElement().elementsByTagName("bbt");

    BBTList bbtList;
    bbtList.reserve(static_cast<unsigned long>(bbts.count()));

    for (int i = 0; i < bbts.count(); ++i) {
        const auto currentBBT = bbts.at(i);
        const auto currentBBTAttributes{currentBBT.attributes()};

        const QDomAttr blockAttribute{
            currentBBTAttributes.namedItem("bk").toAttr()};
        const QDomAttr fromBlockAttribute{
            currentBBTAttributes.namedItem("frombk").toAttr()};
        const QDomAttr routeAttribute{
            currentBBTAttributes.namedItem("route").toAttr()};
        const QDomAttr intervalAttribute{
            currentBBTAttributes.namedItem("interval").toAttr()};
        const QDomAttr stepsAttribute{
            currentBBTAttributes.namedItem("steps").toAttr()};
        const QDomAttr speedAttribute{
            currentBBTAttributes.namedItem("speed").toAttr()};
        const QDomAttr blockEnterSideAttribute{
            currentBBTAttributes.namedItem("blockenterside").toAttr()};
        const QDomAttr countAttribute{
            currentBBTAttributes.namedItem("count").toAttr()};
        const QDomAttr fixedAttribute{
            currentBBTAttributes.namedItem("fixed").toAttr()};

        BBT bbt{blockAttribute.value(),
                fromBlockAttribute.value(),
                routeAttribute.value(),
                intervalAttribute.value().toInt(),
                stepsAttribute.value().toInt(),
                speedAttribute.value().toInt(),
                blockAttribute.value().toInt(),
                countAttribute.value().toInt(),
                convertStringToBool(fixedAttribute.value())};
        bbtList.emplace_back(bbt);
    }
    return bbtList;
}

void PlanParser::parseBlocks(const QDomNodeList& blocks)
{
    blockList.reserve(static_cast<unsigned long>(blocks.length()));

    for (int i = 0; i < blocks.length(); ++i) {
        const auto currentBlock = blocks.at(i);
        const auto currentBlockAttributes{currentBlock.attributes()};

        const QDomAttr blockNameAttr{
            currentBlockAttributes.namedItem("id").toAttr()};
        const QDomAttr isMainLineAttribute{
            currentBlockAttributes.namedItem("mainline").toAttr()};
        const QDomAttr blockLenAttribute{
            currentBlockAttributes.namedItem("len").toAttr()};

        Block block{blockNameAttr.value(), blockLenAttribute.value().toInt(),
                    convertStringToBool(isMainLineAttribute.value())};

        blockList.emplace_back(block);
    }
}

void PlanParser::parseRoutes(const QDomNodeList& routes)
{
    routeList.reserve(static_cast<unsigned long>(routes.length()));

    for (int i = 0; i < routes.length(); ++i) {
        const auto currentRoute = routes.at(i);
        const auto currentRouteAttributes{currentRoute.attributes()};

        const QDomAttr routeIdAttr{
            currentRouteAttributes.namedItem("id").toAttr()};
        const QDomAttr routeFromBlockAttr{
            currentRouteAttributes.namedItem("bka").toAttr()};
        const QDomAttr routeToBlockAttr{
            currentRouteAttributes.namedItem("bkb").toAttr()};
        const QDomAttr routeFromBlockEnterSideAttr{
            currentRouteAttributes.namedItem("bkaside").toAttr()};
        const QDomAttr routeToBlockEnterSideAttr{
            currentRouteAttributes.namedItem("bkbside").toAttr()};
        const QDomAttr reduceVelocityAttr{
            currentRouteAttributes.namedItem("reduceV").toAttr()};

        const QDomNodeList switches{
            currentRoute.toElement().elementsByTagName("swcmd")};

        const bool isCompletelyStraight{!isTurnInRoute(switches)};

        QString blockSearchName{routeFromBlockAttr.value()};

        const auto search = [&blockSearchName](const Block& item) {
            return item.name == blockSearchName && item.isMainLine;
        };

        auto fromBlockIt =
            std::find_if(blockList.begin(), blockList.end(), search);

        blockSearchName = routeToBlockAttr.value();

        auto toBlockIt =
            std::find_if(blockList.begin(), blockList.end(), search);

        const bool isOnlyMainline{fromBlockIt != blockList.end() &&
                                  toBlockIt != blockList.end()};
        Route route{routeIdAttr.value(),
                    routeFromBlockAttr.value(),
                    routeToBlockAttr.value(),
                    routeFromBlockEnterSideAttr.value(),
                    routeToBlockEnterSideAttr.value(),
                    convertStringToBool(reduceVelocityAttr.value().isEmpty()
                                            ? "true"
                                            : reduceVelocityAttr.value()),
                    isCompletelyStraight,
                    isOnlyMainline};

        routeList.emplace_back(route);
    }
}

bool PlanParser::isTurnInRoute(const QDomNodeList& switches)
{
    if (switches.size() == 0) return false;

    for (int i = 0; i < switches.size(); ++i) {
        const auto currentSwitch = switches.at(i);
        const auto currentSwitchAttributes{currentSwitch.attributes()};

        const QDomAttr switchCommand{
            currentSwitchAttributes.namedItem("cmd").toAttr()};

        if (switchCommand.value() == "turnout") { return true; }
    }

    return false;
}

auto PlanParser::getLocList() const -> BBTCalculator::Core::LocList
{
    return locList;
}

auto PlanParser::getBlockList() const -> BBTCalculator::Core::BlockList
{
    return blockList;
}

auto PlanParser::getRouteList() const -> BBTCalculator::Core::RouteList
{
    return routeList;
}

auto PlanParser::convertStringToBool(const QString& string) -> bool
{
    return string.toUpper() == "TRUE";
}

void PlanParser::saveBBTForLocomotive(const Loc& loc)
{
    QDomDocument document("plan");
    if (!document.setContent(&file)) {
        // TODO: report error
        file.close();
    }

    QDomElement root{document.documentElement()};

    QDomNodeList locs = root.elementsByTagName("lc");

    for (int i = 0; i < locs.length(); ++i) {
        auto currentLoc = locs.at(i);
        const auto currentLocAttributes{currentLoc.attributes()};

        const QDomAttr locNameAttr{
            currentLocAttributes.namedItem("id").toAttr()};

        if (locNameAttr.value() == loc.name) {
            QDomNodeList bbts = currentLoc.toElement().elementsByTagName("bbt");
            while (!bbts.isEmpty()) {
                QDomNode node = bbts.at(0);
                currentLoc.removeChild(node);
            }

            for (auto currentBBT : loc.bbt) {
                QDomElement bbtElement = document.createElement("bbt");
                bbtElement.setAttribute("bk", currentBBT.block);
                bbtElement.setAttribute("frombk", currentBBT.fromBlock);
                bbtElement.setAttribute("route", currentBBT.route);
                bbtElement.setAttribute("interval", currentBBT.interval);
                bbtElement.setAttribute("steps", currentBBT.steps);
                bbtElement.setAttribute("speed", currentBBT.speed);
                bbtElement.setAttribute("count", currentBBT.count);
                bbtElement.setAttribute("fixed",
                                        currentBBT.isFixed ? "true" : "false");
                bbtElement.setAttribute(
                    "bbtenterside",
                    currentBBT.route.endsWith("+]") ? "1" : "2");
                currentLoc.appendChild(bbtElement);
            }
            break;
        }
    }

    file.resize(0);
    QTextStream stream(&file);

    document.save(stream, indentation);
    file.close();
}
