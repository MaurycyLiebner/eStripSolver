#include "estripsolve.h"

#include <functional>

#include "algorithms/StripPacking.h"
#include "algorithms/GuillotineBinPack.h"
#include "algorithms/MaxRectsBinPack.h"

using namespace rbp;

struct ECalculation {
    QRect fRect;
    int fAlgorithm;
    GuillotineBinPack::GuillotineSplitHeuristic fGuillSplit;
    GuillotineBinPack::FreeRectChoiceHeuristic fGuillChoice;
    MaxRectsBinPack::FreeRectChoiceHeuristic fMaxChoice;

    int bottom() const { return fRect.bottom(); }
};

using CalcFunc = std::function<Rect(int, int)>;
bool calc(const CalcFunc& func,
          ECalculation& perm,
          const QList<ESize>& data,
          QList<QRect>& sol) {
    sol.clear();
    QRect rUnion(0, 0, 0, 0);
    for(auto& rect : data) {
        const ESize solSize(rect);
        const int w = solSize.d0();
        const int h = solSize.d1();
        const auto result = func(w, h);
        const QRect qrect(result.x, result.y, result.width, result.height);
        { // check
            if(qrect.width() <= 0 || qrect.height() <= 0) return false;
            for(const auto& r : qAsConst(sol)) {
                if(qrect.intersects(r)) {
                    return false;
                }
            }
        } // check
        sol << qrect;
        rUnion = rUnion.united(qrect);
    }
    perm.fRect = rUnion;
    return true;
}

bool calc(const int width,
          const int height,
          ECalculation& perm,
          const QList<ESize>& data,
          QList<QRect>& sol) {
    if(perm.fAlgorithm == 0) {
        GuillotineBinPack g(width, height);
        const auto choice = perm.fGuillChoice;
        const auto split = perm.fGuillSplit;

        const auto alg = [&g, choice, split](const int wm, const int hm) {
            return g.Insert(wm, hm, true, choice, split);
        };
        return calc(alg, perm, data, sol);
    } else {
        MaxRectsBinPack g(width, height);
        const auto choice = perm.fMaxChoice;
        const auto alg = [&g, choice](const int wm, const int hm) {
            return g.Insert(wm, hm, choice);
        };
        return calc(alg, perm, data, sol);
    }
}


bool calcSols(const int width,
              const int height,
              EStripResults& results) {
    const QList<ESize>& data = results.fData;
    QList<ECalculation> calcs;
    for(const int a : {0, 1}) {
        using GSH = GuillotineBinPack::GuillotineSplitHeuristic;
        using GFRCH = GuillotineBinPack::FreeRectChoiceHeuristic;
        using MFRCH = MaxRectsBinPack::FreeRectChoiceHeuristic;
        if(a == 0) { // Guill
            const auto mfrch = static_cast<MFRCH>(0);
            const int cMax = GuillotineBinPack::RectWorstLongSideFit;
            for(int c = 0; c <= cMax; c++) {
                const auto choice = static_cast<GFRCH>(c);
                const int sMax = GuillotineBinPack::SplitLongerAxis;
                for(int s = 0; s <= sMax; s++) {
                    const auto split = static_cast<GSH>(s);
                    calcs << ECalculation{QRect(), 1, split, choice, mfrch};
                }
            }
        } else { // Max
            const auto gsh = static_cast<GSH>(0);
            const auto gfrch = static_cast<GFRCH>(0);
            const int cMax = MaxRectsBinPack::RectContactPointRule;
            for(int c = 0; c <= cMax; c++) {
                const auto choice = static_cast<MFRCH>(c);
                calcs << ECalculation{QRect(), 1, gsh, gfrch, choice};
            }
        }
    }

    QList<ECalculation> validcalcs;
    for(auto& cal : calcs) {
        QList<QRect> sols;
        const bool valid = calc(width, height, cal, data, sols);
        if(valid) validcalcs << cal;
    }

    if(validcalcs.isEmpty()) return false;

    const auto sorter = [](const ECalculation& a, const ECalculation& b) {
        return a.bottom() < b.bottom();
    };
    std::sort(validcalcs.begin(), validcalcs.end(), sorter);

    auto bestCalc = validcalcs.first();
    calc(width, height, bestCalc, data, results.fRects);
    results.fRect = bestCalc.fRect;
    return true;
}

void setSolution(const StripPacking::Solution& sol, EStripResults& results) {
    auto& bRect = results.fRect;
    auto rects = results.fRects;
    bRect = QRect();
    for(const auto& rect : sol.solvedRects_) {
        const int x = rect.x_;
        const int y = rect.y_;
        const int w = rect.width_;
        const int h = rect.height_;
        const QRect qrect(x, y, w, h);
        rects.append(qrect);
        bRect = bRect.united(qrect);
    }
}

bool gStripSolve(const int width, EStripResults& output) {
    EStripResults results = output;
    const auto& data = results.fData;

    if(!calcSols(width, 100000, results)) return false;

    {
        int h = results.fRect.bottom();
        for(;;) {
            h -= 50;
            if(h < 0) break;
            EStripResults res = results;
            const bool r = calcSols(width, h, res);
            if(r) results = res;
            else break;
        }

        StripPacking::StripProblem solverNFDH(width);
        StripPacking::StripProblem solverFFDH(width);
        StripPacking::StripProblem solverBFDH(width);

        for(const auto& rect : qAsConst(data)) {
            const ESize rectSize(rect);
            const int dim0 = rectSize.d0();
            const int dim1 = rectSize.d1();
            solverNFDH.AddRect(StripPacking::Rect(dim0, dim1));
            solverFFDH.AddRect(StripPacking::Rect(dim0, dim1));
            solverBFDH.AddRect(StripPacking::Rect(dim0, dim1));
        }

        solverNFDH.SetAlgorithm<StripPacking::Algorithms::NFDH>();
        const auto solutionNFDH = solverNFDH.Solve();
        solverFFDH.SetAlgorithm<StripPacking::Algorithms::FFDH>();
        const auto solutionFFDH = solverFFDH.Solve();
        solverBFDH.SetAlgorithm<StripPacking::Algorithms::BFDH>();
        const auto solutionBFDH = solverBFDH.Solve();

        int currentHeight = h;
        if(solutionNFDH.height_ < currentHeight) {
            currentHeight = solutionNFDH.height_;
            setSolution(solutionNFDH, results);
        }
        if(solutionFFDH.height_ < currentHeight) {
            currentHeight = solutionFFDH.height_;
            setSolution(solutionFFDH, results);
        }
        if(solutionBFDH.height_ < currentHeight) {
            setSolution(solutionBFDH, results);
        }
    }

    output = results;
    return true;
}
