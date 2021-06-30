#include "../include/egeneticstripsolver.h"

#include "../src/estripsolve.h"

EGeneticStripSolver::EGeneticStripSolver(const EReceiver<EGeneticResults>& rec,
                                         const EStopped<EGeneticResults>& stop) :
    mRec(rec), mStop(stop) {

}

bool EGeneticStripSolver::start(const int width,
                                const int heightLimit,
                                const QList<ESize>& data) {
    if(mG) return false;
    EGeneticSettings genSettings;
    genSettings.fReceiveInc = 200;

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
        const int i = rand() % 3;
        if(i == 0) {
            results.randomJoin();
        } else if(i == 1) {
            results.randomSplit();
        }
        results.randomMove();
        return results;
    };

    funcs.fStopped = [this]() {
        const auto g = mG;
        mG = nullptr;
        mStop();
        delete g;
    };

    funcs.fReceiver = mRec;;

    funcs.fSelector = [heightLimit](const EGeneticResults& r) {
        return r.totalHeight(heightLimit);
    };

    mG = new EGenetic<EGeneticResults>();
    mG->start(funcs, genSettings);
    return true;
}

bool EGeneticStripSolver::stop() {
    if(!mG) return false;
    mG->stop();
    return true;
}
