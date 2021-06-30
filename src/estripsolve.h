#ifndef ESTRIPSOLVE_H
#define ESTRIPSOLVE_H

#include <QList>
#include <QRect>

#include "esize.h"

struct EStripResults {
    QRect fRect;
    QList<ESize> fData;
    QList<QRect> fRects;
};

extern bool gStripSolve(const int width, EStripResults& output);

#endif // ESTRIPSOLVE_H
