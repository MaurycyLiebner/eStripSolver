#include "../include/esize.h"

bool operator==(const ESize& s0, const ESize& s1) {
    return s0.d0() == s1.d0() && s0.d1() == s1.d1();
}

bool operator!=(const ESize& s0, const ESize& s1) {
    return s0.d0() != s1.d0() || s0.d1() != s1.d1();
}
