/*
Copyright (c) 2011 Andrew Finnell

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

class FBBezierCurve;
class FBContourOverlap;
class FBEdgeCrossing;
class FBBezierIntersection;
class FBCurveLocation;
class FBEdgeOverlap;

typedef enum FBContourInside { FBContourInsideFilled, FBContourInsideHole } FBContourInside;

typedef enum FBContourDirection { FBContourClockwise, FBContourAntiClockwise } FBContourDirection;

class FBBezierContour : public std::enable_shared_from_this<FBBezierContour> {
private:
  std::vector<std::shared_ptr<FBBezierCurve>> _edges;
  mutable FBRect _bounds;       // cache
  mutable FBRect _boundingRect; // cache
  FBContourInside _inside;
  std::vector<std::shared_ptr<FBContourOverlap>> _overlaps;

protected:
  bool contourAndSelfIntersectingContoursContainPoint(FBPoint point);
  void addSelfIntersectingContoursToArray(std::vector<std::shared_ptr<FBBezierContour>> &contours,
                                          std::shared_ptr<FBBezierContour> originalContour) const;
  std::tuple<std::shared_ptr<FBBezierCurve>, FBPoint, FBFloat> startingEdge() const;
  bool markCrossingsOnEdge(std::shared_ptr<FBBezierCurve> edge, FBFloat startParameter, FBFloat stopParameter,
                           const std::vector<std::shared_ptr<FBBezierContour>> &otherContours, bool isEntry);
  std::vector<std::shared_ptr<FBBezierContour>> selfIntersectingContours();

public:
  // Methods for building up the contour. The reverse forms flip points in the bezier curve before
  // adding them
  //  to the contour. The crossing to crossing methods assuming the crossings are on the same edge.
  //  One of
  //  crossings can be nil, but not both.
  void addCurve(std::shared_ptr<FBBezierCurve> curve);
  void addCurve(std::shared_ptr<FBEdgeCrossing> startCrossing, std::shared_ptr<FBEdgeCrossing> endCrossing);
  void addReverseCurve(std::shared_ptr<FBBezierCurve> curve);
  void addReverseCurve(std::shared_ptr<FBEdgeCrossing> startCrossing, std::shared_ptr<FBEdgeCrossing> endCrossing);

  void intersectionsWithRay(std::shared_ptr<FBBezierCurve> testEdge,
                            std::function<void(std::shared_ptr<FBBezierIntersection> intersection)> block) const;
  size_t numberOfIntersectionsWithRay(std::shared_ptr<FBBezierCurve> testEdge) const;
  bool containsPoint(FBPoint testPoint) const;
  void markCrossingsAsEntryOrExitWithContour(std::shared_ptr<FBBezierContour> otherContour, bool markInside);

  void close();

  std::shared_ptr<FBBezierContour> reversedContour() const;
  FBContourDirection direction() const;
  std::shared_ptr<FBBezierContour> contourMadeClockwiseIfNecessary();

  void addOverlap(std::shared_ptr<FBContourOverlap> overlap);
  void removeAllOverlaps();
  bool isEquivalent(std::shared_ptr<FBBezierContour> other) const;

  std::shared_ptr<FBBezierCurve> startEdge() const;
  FBPoint testPointForContainment() const;

  std::shared_ptr<FBCurveLocation> closestLocationToPoint(FBPoint point);

  const std::vector<std::shared_ptr<FBBezierCurve>> edges() const { return _edges; }
  FBRect bounds() const;
  FBRect boundingRect() const;
  FBPoint firstPoint() const;
  FBContourInside inside() const { return _inside; }
  void setInside(FBContourInside inside) { _inside = inside; }
  std::vector<std::shared_ptr<FBBezierContour>> intersectingContours() const;

  bool crossesOwnContour(std::shared_ptr<FBBezierContour> contour);

  void forEachEdgeOverlapDo(std::function<void(std::shared_ptr<FBEdgeOverlap> overlap)> block);
  bool doesOverlapContainCrossing(std::shared_ptr<FBEdgeCrossing> crossing) const;
  bool doesOverlapContainParameter(FBFloat parameter, std::shared_ptr<FBBezierCurve> edge) const;

  std::shared_ptr<FBBezierContour> copy() const;

  std::string str(int indent = -1) const;
};

} // namespace fb

template <> struct std::formatter<fb::FBBezierContour> : std::formatter<std::string> {
  auto format(const fb::FBBezierContour &contour, std::format_context &ctx) const {
    return std::formatter<std::string>::format(contour.str(), ctx);
  }
};

template <> struct std::formatter<std::shared_ptr<fb::FBBezierContour>> : std::formatter<std::string> {
  auto format(const std::shared_ptr<fb::FBBezierContour> &curve, std::format_context &ctx) const {
    return std::formatter<std::string>::format(curve->str(), ctx);
  }
};
