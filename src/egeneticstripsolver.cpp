#include "../include/egeneticstripsolver.h"

#include "../src/estripsolve.h"

EGeneticStripSolver::EGeneticStripSolver() {

}

bool EGeneticStripSolver::start(const EReceiver<EGeneticResults>& rec,
                                const EStopped<EGeneticResults>& stop,
                                const EGeneticSettings& settings,
                                const int width,
                                const int heightLimit,
                                const QList<ESize>& data) {
    if(mG) return false;

    EGeneticFunctions<EGeneticResults> funcs;

    funcs.fGenerator = [data, width]() {
        EGeneticResults results(width);
        EStripResults r;
        r.fData = data;
        gStripSolve(width, r);
        results.append(r);
        return results;
    };

    funcs.fBreeder = [width](const EGeneticResults& r1,
                             const EGeneticResults& r2) {
        EGeneticResults results(width);
        if(rand() % 2) {
            results = r1;
        } else {
            results = r2;
        }
        const int jMax = rand() % 4 + 1;
        for(int  j = 0; j < jMax; j++) {
            const int i = rand() % 3;
            if(i == 0) {
                results.randomJoin();
            } else if(i == 1) {
                results.randomSplit();
            } else results.randomMove();
        }
        return results;
    };

    funcs.fStopped = [this, stop]() {
        const auto g = mG;
        mG = nullptr;
        stop();
        delete g;
    };

    funcs.fReceiver = rec;

    funcs.fSelector = [heightLimit](const EGeneticResults& r) {
        return r.totalHeight(heightLimit);
    };

    mG = new EGenetic<EGeneticResults>();
    mG->start(funcs, settings);
    return true;
}

bool EGeneticStripSolver::stop() {
    if(!mG) return false;
    mG->stop();
    return true;
}
