/*
Copyright (c) 2011 Andrew Finnell
Copyright (c) 2025 - Yohei Yoshihara

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the “Software”), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "FBCommon.hpp"

namespace fb {

class FBBezierContour;
class FBBezierCurve;
class FBBezierIntersectRange;
class FBEdgeCrossing;

class FBEdgeOverlap {
  std::shared_ptr<FBBezierCurve> _edge1;
  std::shared_ptr<FBBezierCurve> _edge2;
  std::shared_ptr<FBBezierIntersectRange> _range;

public:
  FBEdgeOverlap(std::shared_ptr<FBBezierIntersectRange> range, std::shared_ptr<FBBezierCurve> edge1,
                std::shared_ptr<FBBezierCurve> edge2)
      : _edge1(edge1)
      , _edge2(edge2)
      , _range(range) {}
  std::shared_ptr<FBBezierCurve> edge1() { return _edge1; }
  std::shared_ptr<FBBezierCurve> edge2() { return _edge2; }
  std::shared_ptr<FBBezierIntersectRange> range() { return _range; }

  bool fitsBefore(std::shared_ptr<FBEdgeOverlap> nextOverlap);
  bool fitsAfter(std::shared_ptr<FBEdgeOverlap> previousOverlap);
  void addMiddleCrossing();
  bool doesContainParameter(FBFloat parameter, std::shared_ptr<FBBezierCurve> edge, bool extendsBeforeStart,
                            bool extendsAfterEnd);
};

class FBEdgeOverlapRun {
  std::vector<std::shared_ptr<FBEdgeOverlap>> _overlaps;

public:
  const std::vector<std::shared_ptr<FBEdgeOverlap>> &overlaps() const { return _overlaps; }
  bool isCrossing();
  void addCrossings();

  bool insertOverlap(std::shared_ptr<FBEdgeOverlap> overlap);
  bool isComplete();
  std::shared_ptr<FBBezierContour> contour1();
  std::shared_ptr<FBBezierContour> contour2();

  bool doesContainCrossing(std::shared_ptr<FBEdgeCrossing> crossing);
  bool doesContainParameter(FBFloat parameter, std::shared_ptr<FBBezierCurve> edge);
};

class FBContourOverlap {
  std::vector<std::shared_ptr<FBEdgeOverlapRun>> _runs;

public:
  std::shared_ptr<FBBezierContour> contour1();
  std::shared_ptr<FBBezierContour> contour2();

  void addOverlap(std::shared_ptr<FBBezierIntersectRange> range, std::shared_ptr<FBBezierCurve> edge1,
                  std::shared_ptr<FBBezierCurve> edge2);
  void runsWithBlock(std::function<void(std::shared_ptr<FBEdgeOverlapRun> run, bool *stop)> block);

  void reset();

  bool isComplete();
  bool isEmpty();

  bool isBetweenContour(std::shared_ptr<const FBBezierContour> contour1,
                        std::shared_ptr<const FBBezierContour> contour2);
  bool doesContainCrossing(std::shared_ptr<FBEdgeCrossing> crossing);
  bool doesContainParameter(FBFloat parameter, std::shared_ptr<FBBezierCurve> edge);
};

} // namespace fb