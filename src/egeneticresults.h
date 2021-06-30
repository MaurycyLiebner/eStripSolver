#ifndef EGENETICRESULTS_H
#define EGENETICRESULTS_H

#include "estripsolve.h"

class EGeneticResults : public QList<EStripResults> {
public:
    EGeneticResults() {}
    EGeneticResults(const int width) : mWidth(width) {}

    void randomMove();
    void randomSplit();
    void randomJoin();

    int totalHeight(const int maxHeight) const;
private:
    int mWidth;
};

#endif // EGENETICRESULTS_H
