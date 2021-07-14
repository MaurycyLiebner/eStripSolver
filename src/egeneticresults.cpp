#include "../include/egeneticresults.h"

#include "estripsolve.h"

void EGeneticResults::randomMove() {
    const int r0id = rand() % count();
    const int r1id = rand() % count();

    auto& r0 = this->operator[](r0id);
    if(r0.fData.count() < 2) return;
    auto& r1 = this->operator[](r1id);

    const int nr0 = rand() % r0.fData.count();
    const auto d0 = r0.fData.takeAt(nr0);
    r1.fData.append(d0);

    gStripSolve(mWidth, r0);
    if(&r0 != &r1) gStripSolve(mWidth, r1);
}

void EGeneticResults::randomSplit() {
    const int r0id = rand() % count();
    auto& r0 = this->operator[](r0id);
    auto& r0d = r0.fData;
    if(r0d.count() < 2) return;
    const int nr1 = qMax(1, rand() % (r0d.count() - 1));
    EStripResults r1;
    for(int i = 0; i < nr1; i++) {
        const int takeId = rand() % r0d.count();
        const auto took = r0d.takeAt(takeId);
        r1.fData.append(took);
    }

    gStripSolve(mWidth, r0);
    gStripSolve(mWidth, r1);
    append(r1);
}

void EGeneticResults::randomJoin() {
    if(count() < 2) return;
    const int r0id = rand() % count();
    const auto r0 = takeAt(r0id);

    const int r1id = rand() % count();
    const auto r1 = takeAt(r1id);

    EStripResults r;
    for(const auto& d : r0.fData) {
        r.fData.append(d);
    }
    for(const auto& d : r1.fData) {
        r.fData.append(d);
    }

    gStripSolve(mWidth, r);
    append(r);
}

int EGeneticResults::totalHeight(const int maxHeight) const {
    int result = 0;
    for(const auto& r : *this) {
        const int h = r.fRect.bottom();
        result += h + 2*(h > maxHeight ? h - maxHeight : 0);
    }
    return (1 + 0.01*count())*(result + count());
}

bool operator==(const EStripResults &s0, const EStripResults &s1) {
    return s0.fRect == s1.fRect &&
           s0.fData == s1.fData &&
           s0.fRects == s1.fRects;
}
