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

#include "FBBezierContour.hpp"

#include "FBBezierCurve.hpp"
#include "FBBezierIntersectRange.hpp"
#include "FBBezierIntersection.hpp"
#include "FBContourOverlap.hpp"
#include "FBCurveLocation.hpp"
#include "FBEdgeCrossing.hpp"

#include <algorithm>
#include <format>
#include <iostream>
#include <ranges>

namespace fb {

void FBBezierContour::addCurve(std::shared_ptr<FBBezierCurve> curve) {
  // Add the curve by wrapping it in an edge
  if (curve == nullptr) {
    return;
  }
  curve->setContour(shared_from_this());
  curve->setIndex(_edges.size());
  _edges.push_back(curve);
  _bounds = FBZeroRect; // force the bounds to be recalculated
  _boundingRect = FBZeroRect;
}

void FBBezierContour::addCurve(std::shared_ptr<FBEdgeCrossing> startCrossing,
                               std::shared_ptr<FBEdgeCrossing> endCrossing) {
  // First construct the curve that we're going to add, by seeing which crossing
  //  is nil. If the crossing isn't given go to the end of the edge on that
  //  side.
  std::shared_ptr<FBBezierCurve> curve = nullptr;
  if (startCrossing == nullptr && endCrossing != nullptr) {
    // From start to endCrossing
    curve = endCrossing->leftCurve();
  } else if (startCrossing != nullptr && endCrossing == nullptr) {
    // From startCrossing to end
    curve = startCrossing->rightCurve();
  } else if (startCrossing != nullptr && endCrossing != nullptr) {
    // From startCrossing to endCrossing
    curve
        = startCrossing->curve()->subcurveWithRange(FBRangeMake(startCrossing->parameter(), endCrossing->parameter()));
  }
  addCurve(curve);
}

void FBBezierContour::addReverseCurve(std::shared_ptr<FBBezierCurve> curve) {
  // Just reverse the points on the curve. Need to do this to ensure the end
  // point from one edge, matches the start
  //  on the next edge.
  if (curve == nullptr) {
    return;
  }
  addCurve(curve->reversedCurve());
}

void FBBezierContour::addReverseCurve(std::shared_ptr<FBEdgeCrossing> startCrossing,
                                      std::shared_ptr<FBEdgeCrossing> endCrossing) {
  // First construct the curve that we're going to add, by seeing which crossing
  //  is nil. If the crossing isn't given go to the end of the edge on that
  //  side.
  std::shared_ptr<FBBezierCurve> curve = nullptr;
  if (startCrossing == nullptr && endCrossing != nullptr) {
    // From start to endCrossing
    curve = endCrossing->leftCurve();
  } else if (startCrossing != nullptr && endCrossing == nullptr) {
    // From startCrossing to end
    curve = startCrossing->rightCurve();
  } else if (startCrossing != nullptr && endCrossing != nullptr) {
    // From startCrossing to endCrossing
    curve
        = startCrossing->curve()->subcurveWithRange(FBRangeMake(startCrossing->parameter(), endCrossing->parameter()));
  }
  addReverseCurve(curve);
}

FBRect FBBezierContour::bounds() const {
  // Cache the bounds to save time
  if (!FBEqualRects(_bounds, FBZeroRect)) {
    return _bounds;
  }

  // If no edges, no bounds
  if (_edges.size() == 0) {
    return FBZeroRect;
  }

  FBRect totalBounds = FBZeroRect;
  for (const auto &edge : _edges) {
    auto bounds = edge->bounds();
    if (FBEqualRects(totalBounds, FBZeroRect)) {
      totalBounds = bounds;
    } else {
      totalBounds = FBUnionRect(totalBounds, bounds);
    }
  }

  _bounds = totalBounds;

  return _bounds;
}

FBRect FBBezierContour::boundingRect() const {
  // Cache the bounds to save time
  if (!FBEqualRects(_boundingRect, FBZeroRect)) {
    return _boundingRect;
  }

  // If no edges, no bounds
  if (_edges.size() == 0) {
    return FBZeroRect;
  }

  FBRect totalBounds = FBZeroRect;
  for (const auto &edge : _edges) {
    FBRect bounds = edge->boundingRect();
    if (FBEqualRects(totalBounds, FBZeroRect)) {
      totalBounds = bounds;
    } else {
      totalBounds = FBUnionRect(totalBounds, bounds);
    }
  }

  _boundingRect = totalBounds;

  return _boundingRect;
}

FBPoint FBBezierContour::firstPoint() const {
  if (_edges.size() == 0) {
    return FBZeroPoint;
  }

  const auto &edge = _edges[0];
  return edge->endPoint1();
}

bool FBBezierContour::containsPoint(FBPoint testPoint) const {
  if (!FBPointInRect(testPoint, boundingRect()) || !FBPointInRect(testPoint, bounds())) {
    return false;
  }

  // Create a test line from our point to somewhere outside our graph. We'll see
  // how many times the test
  //  line intersects edges of the graph. Based on the even/odd rule, if it's an
  //  odd number, we're inside the graph, if even, outside.
  FBPoint lineEndPoint = FBMakePoint(testPoint.x > FBMinX(bounds()) ? FBMinX(bounds()) - 10 : FBMaxX(bounds()) + 10,
                                     testPoint.y); /* just move us outside the bounds of the graph */
  auto testCurve = std::make_shared<FBBezierCurve>(testPoint, lineEndPoint);

  size_t intersectCount = numberOfIntersectionsWithRay(testCurve);
  return (intersectCount & 1) == 1;
}

size_t FBBezierContour::numberOfIntersectionsWithRay(std::shared_ptr<FBBezierCurve> testEdge) const {
  std::size_t count = 0;
  intersectionsWithRay(testEdge, [&](std::shared_ptr<FBBezierIntersection> inersection) { ++count; });
  return count;
}

void FBBezierContour::intersectionsWithRay(
    std::shared_ptr<FBBezierCurve> testEdge,
    std::function<void(std::shared_ptr<FBBezierIntersection> intersection)> block) const {
  std::shared_ptr<FBBezierIntersection> firstIntersection = nullptr;
  std::shared_ptr<FBBezierIntersection> previousIntersection = nullptr;

  // Count how many times we intersect with this particular contour
  for (const auto &edge : _edges) {
    // Check for intersections between our test ray and the rest of the bezier
    // graph
    std::shared_ptr<FBBezierIntersectRange> intersectRange = nullptr;
    testEdge->intersectionsWithBezierCurve(
        edge, &intersectRange, [&](std::shared_ptr<FBBezierIntersection> intersection, bool *stop) {
          // Make sure this is a proper crossing
          if (!testEdge->crossesEdge(edge, intersection) || edge->isPoint()) { // don't count tangents
            return;
          }

          // Make sure we don't count the same intersection twice. This happens
          // when the ray crosses at
          //  start or end of an edge.
          if (intersection->isAtStartOfCurve2() && previousIntersection != nullptr) {
            auto previousEdge = edge->previous();
            if (previousIntersection->isAtEndPointOfCurve2() && previousEdge == previousIntersection->curve2()) {
              return;
            }
          } else if (intersection->isAtEndPointOfCurve2() && firstIntersection != nullptr) {
            auto nextEdge = edge->next();
            if (firstIntersection->isAtStartOfCurve2() && nextEdge == firstIntersection->curve2()) {
              return;
            }
          }

          block(intersection);
          if (firstIntersection == nullptr) {
            firstIntersection = intersection;
          }
          previousIntersection = intersection;
        });
    if (intersectRange != nullptr && testEdge->crossesEdge(edge, intersectRange)) {
      block(intersectRange->middleIntersection());
    }
  }
}

std::shared_ptr<FBBezierCurve> FBBezierContour::startEdge() const {
  // When marking we need to start at a point that is clearly either inside or outside
  //  the other graph, otherwise we could mark the crossings exactly opposite of what
  //  they're supposed to be.
  if (edges().size() == 0) {
    return nullptr;
  }

  auto startEdge = edges()[0];
  auto stopValue = startEdge;
  while (startEdge->isStartShared()) {
    startEdge = startEdge->next();
    if (startEdge == stopValue) {
      break; // for safety. But if we're here, we could be hosed
    }
  }
  return startEdge;
}

FBPoint FBBezierContour::testPointForContainment() const {
  // Start with the startEdge, and if it's not shared (overlapping) then use its first point
  auto testEdge = startEdge();
  if (!testEdge->isStartShared()) {
    return testEdge->endPoint1();
  }

  // At this point we know that all the end points defining this contour are shared. We'll
  //  need to somewhat arbitrarily pick a point on an edge that's not overlapping
  auto stopValue = testEdge;
  FBFloat parameter = 0.5;
  while (doesOverlapContainParameter(parameter, testEdge)) {
    testEdge = testEdge->next();
    if (testEdge == stopValue) {
      break; // for safety. But if we're here, we could be hosed
    }
  }

  return std::get<0>(testEdge->pointAtParameter(parameter));
}

std::tuple<std::shared_ptr<FBBezierCurve>, FBPoint, FBFloat> FBBezierContour::startingEdge() const {
  // Start with the startEdge, and if it's not shared (overlapping) then use its first point
  auto testEdge = startEdge();
  if (!testEdge->isStartShared()) {
    return {testEdge, testEdge->endPoint1(), 0.0};
  }

  // At this point we know that all the end points defining this contour are shared. We'll
  //  need to somewhat arbitrarily pick a point on an edge that's not overlapping
  auto stopValue = testEdge;
  FBFloat parameter = 0.5;
  while (doesOverlapContainParameter(parameter, testEdge)) {
    testEdge = testEdge->next();
    if (testEdge == stopValue) {
      break; // for safety. But if we're here, we could be hosed
    }
  }

  auto point = std::get<0>(testEdge->pointAtParameter(parameter));
  return {testEdge, point, parameter};
}

void FBBezierContour::markCrossingsAsEntryOrExitWithContour(std::shared_ptr<FBBezierContour> otherContour,
                                                            bool markInside) {
  // Go through and mark all the crossings with the given contour as "entry" or "exit". This
  //  determines what part of ths contour is outputted.

  // When marking we need to start at a point that is clearly either inside or outside
  //  the other graph, otherwise we could mark the crossings exactly opposite of what
  //  they're supposed to be.
  auto [startEdge, startPoint, startParameter] = startingEdge();

  // Calculate the first entry value. We need to determine if the edge we're starting
  //  on is inside or outside the otherContour.
  bool contains = otherContour->contourAndSelfIntersectingContoursContainPoint(startPoint);
  bool isEntry = markInside ? !contains : contains;
  auto otherContours = otherContour->selfIntersectingContours();
  otherContours.push_back(otherContour);

  static const FBFloat FBStopParameterNoLimit = 2.0; // needs to be > 1.0
  static const FBFloat FBStartParameterNoLimit = 0.0;

  // Walk all the edges in this contour and mark the crossings
  isEntry = markCrossingsOnEdge(startEdge, startParameter, FBStopParameterNoLimit, otherContours, isEntry);
  auto edge = startEdge->next();
  while (edge != startEdge) {
    isEntry = markCrossingsOnEdge(edge, FBStartParameterNoLimit, FBStopParameterNoLimit, otherContours, isEntry);
    edge = edge->next();
  }
  markCrossingsOnEdge(startEdge, FBStartParameterNoLimit, startParameter, otherContours, isEntry);
}

bool FBBezierContour::markCrossingsOnEdge(std::shared_ptr<FBBezierCurve> edge, FBFloat startParameter,
                                          FBFloat stopParameter,
                                          const std::vector<std::shared_ptr<FBBezierContour>> &otherContours,
                                          bool startIsEntry) {
  bool isEntry = startIsEntry;
  // Mark all the crossings on this edge
  edge->crossingsWithBlock([&](std::shared_ptr<FBEdgeCrossing> crossing, bool *stop) {
    // skip over other contours
    if (crossing->isSelfCrossing()
        || !std::ranges::contains(otherContours, crossing->counterpart()->edge()->contour())) {
      return;
    }
    if (crossing->parameter() < startParameter || crossing->parameter() >= stopParameter) {
      return;
    }
    crossing->setEntry(isEntry);
    isEntry = !isEntry; // toggle.
  });
  return isEntry;
}

bool FBBezierContour::contourAndSelfIntersectingContoursContainPoint(FBPoint point) {
  std::size_t containerCount = 0;
  if (containsPoint(point)) {
    ++containerCount;
  }
  auto intersectingContours = selfIntersectingContours();
  for (const auto &contour : intersectingContours) {
    if (contour->containsPoint(point)) {
      ++containerCount;
    }
  }
  return (containerCount & 1) != 0;
}

void FBBezierContour::close() {
  // adds an element to connect first and last points on the contour
  if (_edges.size() == 0) {
    return;
  }

  auto first = _edges[0];
  auto last = _edges[_edges.size() - 1];

  if (!FBArePointsClose(first->endPoint1(), last->endPoint2())) {
    addCurve(std::make_shared<FBBezierCurve>(last->endPoint2(), first->endPoint1()));
  }
}

std::shared_ptr<FBBezierContour> FBBezierContour::reversedContour() const {
  auto revContour = std::make_shared<FBBezierContour>();

  for (const auto &edge : _edges) {
    revContour->addReverseCurve(edge);
  }

  return revContour;
}

FBContourDirection FBBezierContour::direction() const {
  FBPoint lastPoint = FBZeroPoint, currentPoint = FBZeroPoint;
  bool firstPoint = true;
  FBFloat a = 0.0;

  for (const auto &edge : _edges) {
    if (firstPoint) {
      lastPoint = edge->endPoint1();
      firstPoint = false;
    } else {
      currentPoint = edge->endPoint2();
      a += ((lastPoint.x * currentPoint.y) - (currentPoint.x * lastPoint.y));
      lastPoint = currentPoint;
    }
  }

  return (a >= 0) ? FBContourClockwise : FBContourAntiClockwise;
}

std::shared_ptr<FBBezierContour> FBBezierContour::contourMadeClockwiseIfNecessary() {
  auto dir = direction();

  if (dir == FBContourClockwise) {
    return shared_from_this();
  }

  return reversedContour();
}

bool FBBezierContour::crossesOwnContour(std::shared_ptr<FBBezierContour> contour) {
  for (const auto &edge : _edges) {
    bool intersects = false;
    edge->crossingsWithBlock([&](std::shared_ptr<FBEdgeCrossing> crossing, bool *stop) {
      if (!crossing->isSelfCrossing()) {
        return; // Only want the self intersecting crossings
      }
      auto intersectingEdge = crossing->counterpart()->edge();
      if (intersectingEdge->contour() == contour) {
        intersects = true;
        *stop = true;
      }
    });
    if (intersects) {
      return true;
    }
  }
  return false;
}

std::vector<std::shared_ptr<FBBezierContour>> FBBezierContour::intersectingContours() const {
  // Go and find all the unique contours that intersect this specific contour
  std::vector<std::shared_ptr<FBBezierContour>> contours;
  for (const auto &edge : _edges) {
    edge->intersectingEdgesWithBlock([&](std::shared_ptr<FBBezierCurve> intersectingEdge) {
      if (!std::ranges::contains(contours, intersectingEdge->contour())) {
        contours.push_back(intersectingEdge->contour());
      }
    });
  }
  return contours;
}

std::vector<std::shared_ptr<FBBezierContour>> FBBezierContour::selfIntersectingContours() {
  // Go and find all the unique contours that intersect this specific contour from our own graph
  std::vector<std::shared_ptr<FBBezierContour>> contours;
  addSelfIntersectingContoursToArray(contours, shared_from_this());
  return contours;
}

void FBBezierContour::addSelfIntersectingContoursToArray(std::vector<std::shared_ptr<FBBezierContour>> &contours,
                                                         std::shared_ptr<FBBezierContour> originalContour) const {
  for (const auto &edge : _edges) {
    edge->selfIntersectingEdgesWithBlock([&](std::shared_ptr<FBBezierCurve> intersectingEdge) {
      if (intersectingEdge->contour() != originalContour
          && !std::ranges::contains(contours, intersectingEdge->contour())) {
        contours.push_back(intersectingEdge->contour());
        intersectingEdge->contour()->addSelfIntersectingContoursToArray(contours, originalContour);
      }
    });
  }
}

void FBBezierContour::addOverlap(std::shared_ptr<FBContourOverlap> overlap) {
  if (overlap->isEmpty()) {
    return;
  }

  _overlaps.push_back(overlap);
}

void FBBezierContour::removeAllOverlaps() { _overlaps.clear(); }

bool FBBezierContour::isEquivalent(std::shared_ptr<FBBezierContour> other) const {
  for (const auto &overlap : _overlaps) {
    if (overlap->isBetweenContour(shared_from_this(), other) && overlap->isComplete()) {
      return true;
    }
  }
  return false;
}

void FBBezierContour::forEachEdgeOverlapDo(std::function<void(std::shared_ptr<FBEdgeOverlap> overlap)> block) {
  for (const auto &overlap : _overlaps) {
    overlap->runsWithBlock([&](std::shared_ptr<FBEdgeOverlapRun> run, bool *stop) {
      for (const auto &edgeOverlap : run->overlaps()) {
        block(edgeOverlap);
      }
    });
  }
}

bool FBBezierContour::doesOverlapContainCrossing(std::shared_ptr<FBEdgeCrossing> crossing) const {
  for (const auto &overlap : _overlaps) {
    if (overlap->doesContainCrossing(crossing)) {
      return true;
    }
  }
  return false;
}

bool FBBezierContour::doesOverlapContainParameter(FBFloat parameter, std::shared_ptr<FBBezierCurve> edge) const {
  for (const auto &overlap : _overlaps) {
    if (overlap->doesContainParameter(parameter, edge)) {
      return true;
    }
  }
  return false;
}

std::shared_ptr<FBBezierContour> FBBezierContour::copy() const {
  auto copy = std::make_shared<FBBezierContour>();
  for (const auto &edge : _edges) {
    copy->addCurve(edge);
  }
  return copy;
}

std::shared_ptr<FBCurveLocation> FBBezierContour::closestLocationToPoint(FBPoint point) {
  std::shared_ptr<FBBezierCurve> closestEdge = nullptr;
  FBBezierCurveLocation location = {};

  for (const auto &edge : _edges) {
    auto edgeLocation = edge->closestLocationToPoint(point);
    if (closestEdge == nullptr || edgeLocation.distance < location.distance) {
      closestEdge = edge;
      location = edgeLocation;
    }
  }

  if (closestEdge == nullptr) {
    return nullptr;
  }

  auto curveLocation = std::make_shared<FBCurveLocation>(closestEdge, location.parameter, location.distance);
  curveLocation->setContour(shared_from_this());
  return curveLocation;
}

std::string FBBezierContour::str(int indent) const {
  std::ostringstream ss;
  ss << std::format("{}<FBBezierContour: bounds={}, edges=[", fb::indent(indent), bounds());
  if (indent >= 0) {
    ss << '\n';
  }
  for (std::size_t i = 0; i < edges().size(); ++i) {
    ss << edges()[i]->str(indent >= 0 ? indent + 2 : indent);
    if (i != edges().size() - 1) {
      ss << ((indent >= 0) ? ",\n" : ", ");
    }
  }
  if (indent >= 0) {
    ss << '\n';
  }
  ss << std::format("{}]>", fb::indent(indent));
  return ss.str();
}

} // namespace fb