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
#include "FBGeometry.hpp"

#include <functional>
#include <sstream>

namespace fb {

class FBEdgeCrossing;
class FBBezierContour;
class FBBezierIntersection;
class FBBezierIntersectRange;
struct FBBezierCurveLocation;

using FBCurveIntersectionBlock = std::function<void(std::shared_ptr<FBBezierIntersection> intersection, bool *stop)>;

typedef struct FBBezierCurveLocation {
  FBFloat parameter;
  FBFloat distance;
} FBBezierCurveLocation;

struct FBBezierCurveData {
  FBPoint endPoint1;
  FBPoint controlPoint1;
  FBPoint controlPoint2;
  FBPoint endPoint2;

  bool isStraightLine = false;                    // GPC: flag when curve came from a straight line segment
  FBFloat length = 0.0;                           // cached value
  FBRect bounds = {{0.0, 0.0}, {0.0, 0.0}};       // cached value
  bool isPoint = false;                           // cached value
  FBRect boundingRect = {{0.0, 0.0}, {0.0, 0.0}}; // cached value
};

// FBBezierCurve is one cubic 2D bezier curve. It represents one segment of a
// bezier path, and is where the intersection calculation happens
class FBBezierCurve : public std::enable_shared_from_this<FBBezierCurve> {
  mutable FBBezierCurveData _data;
  std::vector<std::shared_ptr<FBEdgeCrossing>> _crossings; // sorted by parameter of the intersection
  std::weak_ptr<FBBezierContour> _contour;
  size_t _index;
  bool _startShared;

protected:
  FBFloat refineParameter(FBFloat parameter, FBPoint point);
  void sortCrossings();

public:
  FBBezierCurve(FBPoint startPoint, FBPoint endPoint, std::shared_ptr<FBBezierContour> contour = nullptr);
  FBBezierCurve(FBPoint endPoint1, FBPoint controlPoint1, FBPoint controlPoint2, FBPoint endPoint2,
                std::shared_ptr<FBBezierContour> contour = nullptr);
  FBBezierCurve(const FBBezierCurveData &data);
  bool operator==(const FBBezierCurve &other) const;
  bool operator!=(const FBBezierCurve &other) const { return !(*this == other); }

  FBPoint endPoint1() const { return _data.endPoint1; }
  FBPoint controlPoint1() const { return _data.controlPoint1; }
  FBPoint controlPoint2() const { return _data.controlPoint2; }
  FBPoint endPoint2() const { return _data.endPoint2; }
  bool isStraightLine() const { return _data.isStraightLine; }
  FBRect bounds() const;
  FBRect boundingRect() const;
  bool isPoint() const;

  bool doesHaveIntersections(std::shared_ptr<FBBezierCurve> curve);
  void intersectionsWithBezierCurve(std::shared_ptr<FBBezierCurve> curve,
                                    std::shared_ptr<FBBezierIntersectRange> *intersectRange,
                                    FBCurveIntersectionBlock block) const;
  std::tuple<FBPoint, std::shared_ptr<FBBezierCurve>, std::shared_ptr<FBBezierCurve>>
  pointAtParameter(FBFloat parameter) const;
  std::shared_ptr<FBBezierCurve> subcurveWithRange(FBRange range);
  std::tuple<std::shared_ptr<FBBezierCurve>, std::shared_ptr<FBBezierCurve>, std::shared_ptr<FBBezierCurve>>
  splitSubcurvesWithRange(FBRange range) const;
  FBFloat length(FBFloat parameter) const;
  FBFloat length() const;

  FBPoint pointFromRightOffset(FBFloat offset) const;
  FBPoint pointFromLeftOffset(FBFloat offset) const;

  FBPoint tangentFromRightOffset(FBFloat offset) const;
  FBPoint tangentFromLeftOffset(FBFloat offset) const;

  FBBezierCurveLocation closestLocationToPoint(FBPoint point) const;
  std::shared_ptr<FBBezierCurve> reversedCurve() const;
  std::shared_ptr<FBBezierCurve> clone() const;
  std::vector<std::shared_ptr<FBEdgeCrossing>> crossings() const { return _crossings; }

  const FBBezierCurveData &data() const { return _data; }
  FBBezierCurveData &data() { return _data; }

  // MARK: ********** FBBezierCurve+Edge **********
  std::shared_ptr<FBBezierContour> contour() const { return _contour.lock(); }
  void setContour(std::shared_ptr<FBBezierContour> contour) { _contour = contour; }
  std::size_t index() const { return _index; }
  void setIndex(std::size_t index) { _index = index; }

  // An easy way to iterate all the edges. Wraps around.
  std::shared_ptr<FBBezierCurve> next();
  std::shared_ptr<FBBezierCurve> previous();
  std::shared_ptr<FBBezierCurve> nextNonpoint();
  std::shared_ptr<FBBezierCurve> previousNonpoint();

  std::shared_ptr<FBEdgeCrossing> firstCrossing() const;
  std::shared_ptr<FBEdgeCrossing> lastCrossing() const;

  bool hasCrossings() const;

  std::shared_ptr<FBEdgeCrossing> firstNonselfCrossing() const;
  std::shared_ptr<FBEdgeCrossing> lastNonselfCrossing() const;

  bool hasNonselfCrossings() const;

  void crossingsWithBlock(std::function<void(std::shared_ptr<FBEdgeCrossing> crossing, bool *stop)> block);
  void crossingsCopyWithBlock(std::function<void(std::shared_ptr<FBEdgeCrossing> crossing, bool *stop)> block);

  std::shared_ptr<FBEdgeCrossing> nextCrossing(std::shared_ptr<FBEdgeCrossing> crossing);
  std::shared_ptr<FBEdgeCrossing> previousCrossing(std::shared_ptr<FBEdgeCrossing> crossing);

  void intersectingEdgesWithBlock(std::function<void(std::shared_ptr<FBBezierCurve> intersectingEdge)> block);
  void selfIntersectingEdgesWithBlock(std::function<void(std::shared_ptr<FBBezierCurve> intersectingEdge)> block);

  bool isStartShared() const { return _startShared; }
  void setStartShared(bool startShared) { _startShared = startShared; }

  void addCrossing(std::shared_ptr<FBEdgeCrossing> crossing);
  void removeCrossing(std::shared_ptr<FBEdgeCrossing> crossing);
  void removeAllCrossings();

  bool crossesEdge(std::shared_ptr<FBBezierCurve> edge2, std::shared_ptr<FBBezierIntersection> intersection);
  bool crossesEdge(std::shared_ptr<FBBezierCurve> edge2, std::shared_ptr<FBBezierIntersectRange> intersectRange);

  std::string str(int indent = -1) const;
};

} // namespace fb

template <> struct std::formatter<fb::FBBezierCurve> : std::formatter<std::string> {
  auto format(const fb::FBBezierCurve &curve, std::format_context &ctx) const {
    return std::formatter<std::string>::format(curve.str(), ctx);
  }
};

template <> struct std::formatter<std::shared_ptr<fb::FBBezierCurve>> : std::formatter<std::string> {
  auto format(const std::shared_ptr<fb::FBBezierCurve> &curve, std::format_context &ctx) const {
    return std::formatter<std::string>::format(curve->str(), ctx);
  }
};

template <> struct std::formatter<std::vector<std::shared_ptr<fb::FBBezierCurve>>> : std::formatter<std::string> {
  auto format(const std::vector<std::shared_ptr<fb::FBBezierCurve>> &curves, std::format_context &ctx) const {
    std::ostringstream ss;
    ss << '[';
    for (std::size_t i = 0; i < curves.size(); ++i) {
      ss << std::format("{}", curves[i]);
      if (i != curves.size() - 1) {
        ss << ", ";
      }
    }
    ss << ']';
    return std::formatter<std::string>::format(ss.str(), ctx);
  }
};
