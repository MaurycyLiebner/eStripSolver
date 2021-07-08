#ifndef EGENETICSTRIPSOLVER_H
#define EGENETICSTRIPSOLVER_H

#include "eStripSolver_global.h"
#include "egeneticresults.h"
#include "egenetic.h"

class ESTRIPSOLVER_EXPORT EGeneticStripSolver {
public:
    EGeneticStripSolver();

    bool start(const EReceiver<EGeneticResults>& rec,
               const EStopped<EGeneticResults>& stop,
               const EGeneticSettings& settings,
               const int width,
               const int heightLimit,
               const QList<ESize>& data);
    bool stop();
private:
    EGenetic<EGeneticResults>* mG = nullptr;
};

#endif // EGENETICSTRIPSOLVER_H
