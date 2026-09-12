
#pragma once

#include <vector>

#include <QPixmap>
#include <QString>

namespace BBTCalculator
{
    namespace Core
    {
        struct BBT
        {
            QString block     = "";
            QString fromBlock = "";
            QString route     = "";

            int interval{0};
            int steps{0};
            int speed{0};
            int blockenterside{1};
            int count{0};

            bool isFixed{false};
        };

        using BBTList = std::vector<BBT>;

        struct Loc
        {
            QString name      = "";
            QString imageName = "";

            bool useBBT{false};
            int bbtSteps{0};

            int v_min{0};
            int v_mid{0};
            int v_cru{0};

            BBTList bbt;
            QPixmap locImage;
        };

        using LocList = std::vector<Loc>;

        struct Block
        {
            QString name = "";

            int length{0};

            // Some blocks have set mainline false but usebbt to true
            bool isMainLine{false};
        };

        using BlockList = std::vector<Block>;

        struct Route
        {
            QString id = "";

            QString fromBlock = "";
            QString toBlock   = "";

            QString fromBlockEnterSide = "";
            QString toBlockEnterSide   = "";

            bool shallReduceVelocity{true};

            bool isCompletelyStraight{false};

            bool isOnlyMainline{false};
        };

        using RouteList = std::vector<Route>;
    } // namespace Core
} // namespace BBTCalculator