
#pragma once

#include "datastructs.hpp"

static const double H0ScalingFactor = 87.0;

namespace BBTCalculator
{
    namespace Core
    {
        class Calculation
        {
        public:
            Calculation(Loc* l, const RouteList& rList, const BlockList& bList,
                        bool shallOverwriteExistingValues);

            void calculateNewBBTEntries(double correctionFactor);

        private:
            BBT& doIntervalComputation(double correctionFactor,
                                       const Route& route, BBT& bbt) const;

            void createNewBBTEntry(double correctionFactor,
                                   const Route& route) const;

            Loc* loc;

            const RouteList& routes;

            const BlockList& blocks;

            const bool overwriteExistingValues;
        };
    } // namespace Core
} // namespace BBTCalculator
