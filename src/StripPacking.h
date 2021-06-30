// Copyright (c) 2015 Matthew J. Runyan

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#include <vector>
#include <ostream>
#include <iomanip>

using std::vector;

namespace StripPacking {

struct Rect
{
    int x_; // this is for returning solution don't set it will be overriden
    int y_; // this is for returning solution don't set it will be overriden
    int width_;
    int height_;

    Rect(int width, int height)
        : x_(0), y_(0), width_(width), height_(height)
    {
    }

    Rect(int x, int y, int width, int height)
        : x_(x), y_(y), width_(width), height_(height)
    {
    }

    // this function only checks dimensions...
    bool operator == (const Rect& rect) const
    {
        return (rect.height_ == height_ &&
                rect.width_ == width_);
    }
};


std::ostream& operator<< (std::ostream& stream, const Rect& rect)
{
    return stream << '{' << rect.x_ << ',' << rect.y_
                  << ',' << rect.width_ << ',' << rect.height_ << '}';
}


struct Level
{
    int height_;
    int widthUsed_;

    Level(int height)
        : height_(height), widthUsed_(0)
    {
    }
};


class Solver
{
public:
    vector<Rect> unsolvedRects_;

    vector<Rect> solvedRects_;

    vector<Rect> unsolvableRects_;

    vector<Level> levels_;

    int currentLevel_;

    int maxWidthUsed_;

    int stripWidth_;

    bool isFinalized_;

    int selectedRectIndex_;

    enum SortMethod {
        SM_GTL, // greatest to least
        SM_LTG, // least to greatest
    };

    enum SortBy {
        SB_WIDTH,
        SB_HEIGHT,
        SB_X,
        SB_Y,
    };

    Solver(vector<Rect> &rects, int stripWidth)
        : unsolvedRects_(rects), stripWidth_(stripWidth), isFinalized_(false),
          maxWidthUsed_(0), selectedRectIndex_(-1), currentLevel_(0)
    {
        levels_.push_back(Level(0));
    }

    /// places the rect to the right of the next rect on the current level
    /// returns true if successful
    bool PlaceRect(int index, int level)
    {
        if (level < 0 || level >= levels_.size())
            return false;

        if (index < 0 || index >= unsolvedRects_.size())
            return false;

        Rect &rect = unsolvedRects_[index];

        // set pos
        rect.y_ = levels_[level].height_;
        rect.x_ = levels_[level].widthUsed_;

        int widthUsed = ( levels_[level].widthUsed_ += rect.width_ );

        if (index <= selectedRectIndex_)
            --selectedRectIndex_;

        maxWidthUsed_ = (maxWidthUsed_ > widthUsed) ? maxWidthUsed_ : widthUsed;

        solvedRects_.push_back( rect );
        unsolvedRects_.erase(unsolvedRects_.begin() + index);
        return true;
    }

    /// find index from the value of the pointer (this rect MUST be contained in
    /// "unsolvedRects_ or this will do nothing)
    /// returns true if successful
    bool PlaceRect(const Rect &rect, int level)
    {
        // get index from pointer if a direct reference to location in unsolvedRects_
        if ( PlaceRect((&rect) - &unsolvedRects_[0], level) )
            return true;

        // now we must scan for it in unsolvedRects_ vector because this is not
        // a direct reference
        for ( int i = 0; i < unsolvedRects_.size(); ++i )
            if ( rect == unsolvedRects_[i] )
                return PlaceRect(i, level);
        return false;
    }

    bool PlaceRect(const Rect &rect)
    {
        return PlaceRect(rect, currentLevel_);
    }

    // finds highest level and sets this as the new current level
    void MoveToNextLevel()
    {
        if (currentLevel_ < 0 || currentLevel_ >= levels_.size())
            return;

        // find highest rect and that is the new height
        int maxHeight = 0;
        for (int i = 0; i < solvedRects_.size(); ++i)
        {
            Rect &rect = solvedRects_[i];
            maxHeight = (rect.height_ + rect.y_ > maxHeight)
                      ? (rect.height_ + rect.y_) : maxHeight;
        }

        levels_.push_back( Level(maxHeight) );
        ++currentLevel_;
    }

    int GapLeft(const Rect &rect, int level)
    {
        if (level < 0 || level >= levels_.size())
            return false;
        return ( stripWidth_ - levels_[level].widthUsed_ - rect.width_ );
    }

    /// does this rect fit in the space remaining on this level?
    bool Fits(const Rect &rect, int level)
    {
        return (GapLeft(rect,level) >= 0);
    }

    bool Fits(const Rect &rect)
    {
        return Fits(rect, currentLevel_);
    }

    Rect &NextRect()
    {
        if (++selectedRectIndex_ < 0 &&
                selectedRectIndex_ >= unsolvedRects_.size())
            throw std::string("Don't call \"NextRect\" more than number of rects.");
        return unsolvedRects_[selectedRectIndex_];
    }

    void ResetCurrentRect()
    {
        selectedRectIndex_ = -1;
    }

    // declare that we are now finished
    void Finalize()
    {
        isFinalized_ = true;

        // make sure the highest level is updated correctly
        MoveToNextLevel();

        // TODO: check for any rects inside one another
    }

    void FindUnsolvableRects()
    {
        for ( int i = 0; i < unsolvedRects_.size(); ++i )
        {
            Rect &rect = unsolvedRects_[i];
            if ( rect.width_ > stripWidth_ )
            {
                unsolvableRects_.push_back(rect);
                unsolvedRects_.erase( unsolvedRects_.begin() + i );
                --i;
            }
        }
    }

    void SortRects(vector<Rect> &rects, SortBy sortBy, SortMethod sortMethod)
    {
        for (int i = 0; i < rects.size(); ++i)
        for (int j = i; j < rects.size(); ++j)
        {
            int first;
            int second;
            switch (sortBy)
            {
            default: return;
            case SB_HEIGHT:
                first = rects[i].height_;
                second = rects[j].height_;
                break;
            case SB_WIDTH:
                first = rects[i].width_;
                second = rects[j].width_;
            }
            if ( ( sortMethod == SM_GTL && first < second ) ||
                 ( sortMethod == SM_LTG && first > second )    )
            {
                Rect tmp = rects[i];
                rects[i] = rects[j];
                rects[j] = tmp;
            }
        }
    }
};


/// represents a solution to a StripProblem
struct Solution
{
    int width_;
    int height_;
    vector<Rect> solvedRects_;
    vector<Rect> failedRects_;
    float efficiency_; // from 0.0f to 1.0f
    std::string algoName_;

    Solution(std::string algoName, int width, int height, vector<Rect> solvedRects,
             vector<Rect> failedRects, float efficiency)
        : algoName_(algoName), width_(width), height_(height), solvedRects_(solvedRects),
          failedRects_(failedRects), efficiency_(efficiency)
    {
    }

    Solution()
    {
    }
};


class StripProblem;


/// contains some functions that define the behavoir
/// that is used to dictate what to do before we
/// actually solve, when to move to next level, etc.
///
/// TODO: add more :)
namespace Algorithms {

/// base class that algorithms must inherrit from
class AlgoBase
{
public:
    AlgoBase()
    {
    }

    virtual ~AlgoBase()
    {
    }

    /// called once just before the algo really starts this is
    /// where one might sort the rects based on their height etc.
    virtual void Start(Solver&)
    {
    }

    /// called for every pass of the solution
    virtual void Update(Solver&) = 0;

    /// I am not sure how this would be useful and as such it is
    /// not a pure virtual but it is here for those who want it...
    virtual void End(Solver&)
    {
    }

    virtual std::string GetName() = 0;
};

} // namespace Algorithms


/// represents a strip problem that needs to be solved and solves it
class StripProblem
{
public:
    int maxWidth_;

    vector<Rect> rects_;

    Solution solution_;

    Algorithms::AlgoBase *algorithm_;


    StripProblem(int width) : algorithm_(nullptr), maxWidth_(width)
    {
    }

    ~StripProblem()
    {
        if (algorithm_)
            delete algorithm_;
    }

    /// sets the algorithm that is used to find the solution
    template<class Algorithm>
    void SetAlgorithm()
    {
        if (algorithm_)
            delete algorithm_;
        algorithm_ = new Algorithm();
    }

    /// adds a rect to the list of rects to solve
    void AddRect(const Rect &rect)
    {
        rects_.push_back(rect);
    }

    /// copies each rect from the vector and adds it
    void AddRects(const vector<Rect> &rects)
    {
        for (int i = 0; i < rects.size(); ++i)
            rects_.push_back(rects[i]);
    }

    /// returns the solution after solved
    Solution &Solve()
    {
        // start
        Solver solver(rects_, maxWidth_);
        solver.FindUnsolvableRects();
        algorithm_->Start(solver);

        // update
        while (!solver.isFinalized_)
        {
            algorithm_->Update(solver);

            // if there are no more rects and all the rects seem to be
            // in the solved vector list then the algo must be done
            if (solver.solvedRects_.size() + solver.unsolvableRects_.size() == rects_.size() &&
                    solver.unsolvedRects_.size() == 0)
                solver.Finalize();
        }

        // finish
        algorithm_->End(solver);

        // find area of used rectangles
        long unsigned int usedArea = 0;
        for ( int i = 0; i < solver.solvedRects_.size(); ++i )
            usedArea += solver.solvedRects_[i].width_ * solver.solvedRects_[i].height_;

        int maxHeightReached = solver.levels_[solver.levels_.size() - 1].height_;

        long unsigned int totalArea = solver.maxWidthUsed_ * maxHeightReached;

        solution_ = Solution(algorithm_->GetName(),
                             solver.maxWidthUsed_,
                             maxHeightReached,
                             solver.solvedRects_,
                             solver.unsolvableRects_,
                             (float)usedArea / (float)totalArea);
        return solution_;
    }
};


} // namespace StripPacking


namespace std {
template <class T>
std::ostream& operator<< (std::ostream& stream, const vector<T>& t)
{
    for ( int i = 0; i < t.size(); ++i )
        stream << t[i] << ",\n";
    return stream;
}
} // namespace std


#ifdef USE_LOVE2D
/// outputs lua friendly part of a table
std::ostream& operator<< (std::ostream& stream, const StripPacking::Solution& solution)
{
    return stream << '{' << "\\\"" << solution.algoName_ << "\\\","
                         << '\'' << std::setprecision(2) << std::setiosflags(std::ios::fixed)
                            << solution.efficiency_*100 << "',"
                         << '{'
                            << solution.width_ << ','
                            << solution.height_ << ','
                         << "},"
                         << '{'
                            << solution.solvedRects_
                         << "},"
                         << '{'
                            << solution.failedRects_
                         << "},"
                  << "},";
}
#else
std::ostream& operator<< (std::ostream& stream, const StripPacking::Solution& solution)
{
    return stream << "----- Solution -----\n\n"
                  << "Algorithm: " << solution.algoName_ << '\n'
                  << "Efficiency: " << std::setprecision(2) << std::setiosflags(std::ios::fixed)
                       << solution.efficiency_*100 << "%\t\n"
                  << "Total Size: " << '{' << solution.width_ << ',' << solution.height_ << '}'
                  << "\n\n"
                  << "----- Successful Rects -----\n\n" << solution.solvedRects_ << '\n'
                  << "----- Failed Rects -----\n\n" << solution.failedRects_ << '\n';
}
#endif

namespace StripPacking {
namespace Algorithms {

class FFDH : public AlgoBase
{
    void Start(Solver &solver)
    {
        solver.SortRects(solver.unsolvedRects_, Solver::SB_HEIGHT, Solver::SM_GTL);
    }

    void Update(Solver &solver)
    {
        Rect &rect = solver.NextRect();
        if ( solver.Fits(rect) )
        {
            solver.PlaceRect(rect);
            solver.ResetCurrentRect();
        }

        if ( solver.selectedRectIndex_ + 1 == solver.unsolvedRects_.size() )
        {
            solver.MoveToNextLevel();
            solver.ResetCurrentRect();
        }
    }

    std::string GetName()
    {
        return "First-Fit Decreasing Height";
    }
};


class NFDH : public AlgoBase
{
    void Start(Solver &solver)
    {
        solver.SortRects(solver.unsolvedRects_, Solver::SB_HEIGHT, Solver::SM_GTL);
    }

    void Update(Solver &solver)
    {
        Rect &rect = solver.NextRect();
        if ( !solver.Fits(rect) )
            solver.MoveToNextLevel();
        solver.PlaceRect(rect);
    }

    std::string GetName()
    {
        return "Next-Fit Decreasing Height";
    }
};


class BFDH : public AlgoBase
{
    void Start(Solver &solver)
    {
        solver.SortRects(solver.unsolvedRects_, Solver::SB_HEIGHT, Solver::SM_GTL);
    }

    void Update(Solver &solver)
    {
        Rect &rect = solver.NextRect();

        unsigned int smallestGapLevelIndex = -1;
        unsigned int smallestGap = -1;
        for ( int i = 0; i < solver.levels_.size(); ++i )
        {
            int gap = solver.GapLeft(rect, i);
            if ( gap >=0 && gap < smallestGap )
            {
                smallestGap = gap;
                smallestGapLevelIndex = i;
            }
        }

        if ( smallestGapLevelIndex != -1 )
            solver.PlaceRect(rect, smallestGapLevelIndex);
        else
        {
            solver.MoveToNextLevel();
            solver.PlaceRect(rect);
        }
    }

    std::string GetName()
    {
        return "Best-Fit Decreasing Height";
    }
};

} // namespace Algorithms
} // namespace StripPacking
