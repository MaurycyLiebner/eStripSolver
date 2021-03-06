#ifndef EGENETICRESULTS_H
#define EGENETICRESULTS_H

#include "esize.h"
#include <QRect>
#include <QList>
#include "eStripSolver_global.h"

struct ESTRIPSOLVER_EXPORT EStripResults {
    QRect fRect;
    QList<ESize> fData;
    QList<QRect> fRects;
};

bool ESTRIPSOLVER_EXPORT operator==(const EStripResults& s0, const EStripResults& s1);

class ESTRIPSOLVER_EXPORT EGeneticResults : public QList<EStripResults> {
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
