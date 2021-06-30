#ifndef EGENETICSTRIPSOLVER_H
#define EGENETICSTRIPSOLVER_H

#include "eStripSolver_global.h"
#include "egeneticresults.h"
#include "egenetic.h"

class ESTRIPSOLVER_EXPORT EGeneticStripSolver {
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

#endif // EGENETICSTRIPSOLVER_H
