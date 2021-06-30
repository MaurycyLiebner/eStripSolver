#ifndef ESIZE_H
#define ESIZE_H

#include "eStripSolver_global.h"

#include <QString>
#include <QSize>

struct ESTRIPSOLVER_EXPORT ESize {
    ESize() : ESize(-1, -1) {}
    ESize(const QSize& size) : ESize(size.width(), size.height()) {}

    ESize(const int d0, const int d1) {
        setSize(d0, d1);
    }

    void setSize(const int d0, const int d1) {
        mDim0 = qMin(d0, d1);
        mDim1 = qMax(d0, d1);
    }

    int d0() const { return mDim0; }
    int d1() const { return mDim1; }

    bool isValid() const {
        return mDim0 > 0 && mDim1 > 0;
    }
private:
    int mDim0;
    int mDim1;
};

bool ESTRIPSOLVER_EXPORT operator==(const ESize& s0, const ESize& s1);
bool ESTRIPSOLVER_EXPORT operator!=(const ESize& s0, const ESize& s1);

#endif // ESIZE_H
