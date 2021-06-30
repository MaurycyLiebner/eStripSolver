#ifndef EGENETICCALCULATOR_H
#define EGENETICCALCULATOR_H

#include "egenetic.h"
#include "egeneticresults.h"
#include "esize.h"

class EGeneticStripSolver {
public:
    EGeneticStripSolver(const EReceiver<EGeneticResults>& rec,
                        const EStopped<EGeneticResults>& stop);

    bool start(const int width,
               const int heightLimit,
               const QList<ESize>& data);
    bool stop();
private:
    const EReceiver<EGeneticResults> mRec;
    const EStopped<EGeneticResults> mStop;

    EGenetic<EGeneticResults>* mG = nullptr;
};

#endif // EGENETICCALCULATOR_H
