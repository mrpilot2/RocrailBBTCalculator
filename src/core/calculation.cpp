
#include "calculation.hpp"

static constexpr short MINIMUM_INTERVAL_TIME_IN_MS{1000};

using BBTCalculator::Core::BlockList;
using BBTCalculator::Core::Calculation;
using BBTCalculator::Core::Loc;
using BBTCalculator::Core::RouteList;

Calculation::Calculation(Loc* l, const RouteList& rList, const BlockList& bList,
                         bool shallOverwriteExistingValues)
    : loc{l}
    , routes{rList}
    , blocks{bList}
    , overwriteExistingValues{shallOverwriteExistingValues}
{}

void Calculation::calculateNewBBTEntries(double correctionFactor)
{
    for (const auto& route : routes) {
        if (overwriteExistingValues) {
            BBTList& presentBBTs = loc->bbt;

            const QString routeId{route.id};
            auto bbtIt = std::find_if(
                presentBBTs.begin(), presentBBTs.end(),
                [routeId](BBT& bbt) { return bbt.route == routeId; });
            if (bbtIt != presentBBTs.end()) {
                const QString blockName{bbtIt->block};
                const auto search = [blockName](const Block& item) {
                    return item.name == blockName;
                };

                auto blockIt = find_if(blocks.begin(), blocks.end(), search);

                if (blockIt != blocks.end() && blockIt->length < 1) {
                    presentBBTs.erase(bbtIt);
                } else {
                    if (!bbtIt->isFixed) {
                        bbtIt->count = 0;
                        *bbtIt = doIntervalComputation(correctionFactor, route,
                                                       *bbtIt);
                    }
                }
            } else {
                createNewBBTEntry(correctionFactor, route);
            }
        } else {
            createNewBBTEntry(correctionFactor, route);
        }
    }
}

void Calculation::createNewBBTEntry(
    double correctionFactor, const BBTCalculator::Core::Route& route) const
{
    const QString blockName{route.toBlock};
    const auto search = [blockName](const Block& item) {
        return item.name == blockName;
    };

    auto blockIt = find_if(blocks.begin(), blocks.end(), search);

    if (blockIt != blocks.end() && blockIt->length < 1) { return; }

    BBT bbt;

    bbt.route     = route.id;
    bbt.fromBlock = route.fromBlock;
    bbt.block     = route.toBlock;
    bbt.count     = 0;

    bbt = doIntervalComputation(correctionFactor, route, bbt);

    loc->bbt.emplace_back(bbt);
}

BBTCalculator::Core::BBT& Calculation::doIntervalComputation(
    double correctionFactor, const BBTCalculator::Core::Route& route,
    BBTCalculator::Core::BBT& bbt) const
{
    if (route.isOnlyMainline) {
        bbt.speed = !route.shallReduceVelocity   ? loc->v_cru
                    : route.isCompletelyStraight ? loc->v_cru
                                                 : loc->v_mid;
    } else {
        bbt.speed = loc->v_mid;
    }
    bbt.steps = loc->bbtSteps;

    const double speedMmPerSecond{(bbt.speed - loc->v_min) / 3.6 * 1000};
    const double speedMMPerSecondScaled{speedMmPerSecond / H0ScalingFactor};

    const QString blockName{bbt.block};
    const auto search = [blockName](const Block& item) {
        return item.name == blockName;
    };

    auto blockIt = find_if(blocks.begin(), blocks.end(), search);

    if (blockIt != blocks.end()) {
        const double totalBrakeTimeInMillisecons{2.0 * blockIt->length /
                                                 speedMMPerSecondScaled *
                                                 1000.0 * correctionFactor};
        bbt.interval =
            static_cast<int>(totalBrakeTimeInMillisecons / bbt.steps);

        if (bbt.interval < MINIMUM_INTERVAL_TIME_IN_MS) {
            bbt.steps = static_cast<int>(totalBrakeTimeInMillisecons /
                                         MINIMUM_INTERVAL_TIME_IN_MS);

            bbt.interval =
                static_cast<int>(totalBrakeTimeInMillisecons / bbt.steps);
        }
    }
    return bbt;
}
