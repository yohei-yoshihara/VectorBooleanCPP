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

#include "FBContourOverlap.hpp"

#include "FBBezierContour.hpp"
#include "FBBezierCurve.hpp"
#include "FBBezierIntersectRange.hpp"
#include "FBEdgeCrossing.hpp"
#include "FBGeometry.hpp"

namespace fb {

static const FBFloat FBOverlapThreshold = 1e-2;

static FBFloat FBComputeEdge1Tangents(std::shared_ptr<FBEdgeOverlap> firstOverlap,
                                      std::shared_ptr<FBEdgeOverlap> lastOverlap, FBFloat offset,
                                      FBPoint edge1Tangents[2]) {
  // edge1Tangents are firstOverlap.range1.minimum going to previous and lastOverlap.range1.maximum
  // going to next
  FBFloat firstLength = 0.0;
  FBFloat lastLength = 0.0;
  if (firstOverlap->range()->isAtStartOfCurve1()) {
    auto otherEdge1 = firstOverlap->edge1()->previousNonpoint();
    edge1Tangents[0] = otherEdge1->tangentFromRightOffset(offset);
    firstLength = otherEdge1->length();
  } else {
    edge1Tangents[0] = firstOverlap->range()->curve1LeftBezier()->tangentFromRightOffset(offset);
    firstLength = firstOverlap->range()->curve1LeftBezier()->length();
  }
  if (lastOverlap->range()->isAtStopOfCurve1()) {
    auto otherEdge1 = lastOverlap->edge1()->nextNonpoint();
    edge1Tangents[1] = otherEdge1->tangentFromLeftOffset(offset);
    lastLength = otherEdge1->length();
  } else {
    edge1Tangents[1] = lastOverlap->range()->curve1RightBezier()->tangentFromLeftOffset(offset);
    lastLength = lastOverlap->range()->curve1RightBezier()->length();
  }
  return std::min(firstLength, lastLength);
}

static FBFloat FBComputeEdge2Tangents(std::shared_ptr<FBEdgeOverlap> firstOverlap,
                                      std::shared_ptr<FBEdgeOverlap> lastOverlap, FBFloat offset,
                                      FBPoint edge2Tangents[2]) {
  // edge2Tangents are firstOverlap.range2.minimum going to previous and lastOverlap.range2.maximum
  // going to next
  //  unless reversed, then
  // edge2Tangents are firstOverlap.range2.maximum going to next and lastOverlap.range2.minimum
  // going to previous
  FBFloat firstLength = 0.0;
  FBFloat lastLength = 0.0;
  if (!firstOverlap->range()->reversed()) {
    if (firstOverlap->range()->isAtStartOfCurve2()) {
      auto otherEdge2 = firstOverlap->edge2()->previousNonpoint();
      edge2Tangents[0] = otherEdge2->tangentFromRightOffset(offset);
      firstLength = otherEdge2->length();
    } else {
      edge2Tangents[0] = firstOverlap->range()->curve2LeftBezier()->tangentFromRightOffset(offset);
      firstLength = firstOverlap->range()->curve2LeftBezier()->length();
    }
    if (lastOverlap->range()->isAtStopOfCurve2()) {
      auto otherEdge2 = lastOverlap->edge2()->nextNonpoint();
      edge2Tangents[1] = otherEdge2->tangentFromLeftOffset(offset);
      lastLength = otherEdge2->length();
    } else {
      edge2Tangents[1] = lastOverlap->range()->curve2RightBezier()->tangentFromLeftOffset(offset);
      lastLength = lastOverlap->range()->curve2RightBezier()->length();
    }
  } else {
    if (firstOverlap->range()->isAtStopOfCurve2()) {
      auto otherEdge2 = firstOverlap->edge2()->nextNonpoint();
      edge2Tangents[0] = otherEdge2->tangentFromLeftOffset(offset);
      firstLength = otherEdge2->length();
    } else {
      edge2Tangents[0] = firstOverlap->range()->curve2RightBezier()->tangentFromLeftOffset(offset);
      firstLength = firstOverlap->range()->curve2RightBezier()->length();
    }
    if (lastOverlap->range()->isAtStartOfCurve2()) {
      auto otherEdge2 = lastOverlap->edge2()->previousNonpoint();
      edge2Tangents[1] = otherEdge2->tangentFromRightOffset(offset);
      lastLength = otherEdge2->length();
    } else {
      edge2Tangents[1] = lastOverlap->range()->curve2LeftBezier()->tangentFromRightOffset(offset);
      lastLength = lastOverlap->range()->curve2LeftBezier()->length();
    }
  }
  return std::min(firstLength, lastLength);
}

static void FBComputeEdge1TestPoints(std::shared_ptr<FBEdgeOverlap> firstOverlap,
                                     std::shared_ptr<FBEdgeOverlap> lastOverlap, FBFloat offset,
                                     FBPoint testPoints[2]) {
  // edge1Tangents are firstOverlap.range1.minimum going to previous and lastOverlap.range1.maximum
  // going to next
  if (firstOverlap->range()->isAtStartOfCurve1()) {
    auto otherEdge1 = firstOverlap->edge1()->previousNonpoint();
    testPoints[0] = otherEdge1->pointFromRightOffset(offset);
  } else {
    testPoints[0] = firstOverlap->range()->curve1LeftBezier()->pointFromRightOffset(offset);
  }
  if (lastOverlap->range()->isAtStopOfCurve1()) {
    auto otherEdge1 = lastOverlap->edge1()->nextNonpoint();
    testPoints[1] = otherEdge1->pointFromLeftOffset(offset);
  } else {
    testPoints[1] = lastOverlap->range()->curve1RightBezier()->pointFromLeftOffset(offset);
  }
}

// MARK: ********** FBContourOverlap **********

void FBContourOverlap::addOverlap(std::shared_ptr<FBBezierIntersectRange> range, std::shared_ptr<FBBezierCurve> edge1,
                                  std::shared_ptr<FBBezierCurve> edge2) {
  auto overlap = std::make_shared<FBEdgeOverlap>(range, edge1, edge2);
  bool createNewRun = false;
  if (_runs.size() == 0) {
    createNewRun = true;
  } else if (_runs.size() == 1) {
    bool inserted = _runs.back()->insertOverlap(overlap);
    createNewRun = !inserted;
  } else {
    bool inserted = _runs.back()->insertOverlap(overlap);
    if (!inserted) {
      inserted = _runs.front()->insertOverlap(overlap);
    }
    createNewRun = !inserted;
  }
  if (createNewRun) {
    auto run = std::make_shared<FBEdgeOverlapRun>();
    run->insertOverlap(overlap);
    _runs.push_back(run);
  }
}

bool FBContourOverlap::doesContainCrossing(std::shared_ptr<FBEdgeCrossing> crossing) {
  for (auto &run : _runs) {
    if (run->doesContainCrossing(crossing)) {
      return true;
    }
  }
  return false;
}

bool FBContourOverlap::doesContainParameter(FBFloat parameter, std::shared_ptr<FBBezierCurve> edge) {
  for (auto &run : _runs) {
    if (run->doesContainParameter(parameter, edge)) {
      return true;
    }
  }
  return false;
}

void FBContourOverlap::runsWithBlock(std::function<void(std::shared_ptr<FBEdgeOverlapRun> run, bool *stop)> block) {
  bool stop = false;
  for (auto &run : _runs) {
    block(run, &stop);
    if (stop) {
      break;
    }
  }
}

void FBContourOverlap::reset() { _runs.clear(); }

bool FBContourOverlap::isComplete() {
  // To be complete, we should have exactly one run that wraps around
  if (_runs.size() != 1) {
    return false;
  }

  return _runs[0]->isComplete();
}

bool FBContourOverlap::isEmpty() { return _runs.size() == 0; }

std::shared_ptr<FBBezierContour> FBContourOverlap::contour1() {
  if (_runs.size() == 0) {
    return nullptr;
  }

  auto run = _runs[0];
  return run->contour1();
}

std::shared_ptr<FBBezierContour> FBContourOverlap::contour2() {
  if (_runs.size() == 0) {
    return nullptr;
  }

  auto run = _runs[0];
  return run->contour2();
}

bool FBContourOverlap::isBetweenContour(std::shared_ptr<const FBBezierContour> contour1,
                                        std::shared_ptr<const FBBezierContour> contour2) {
  auto myContour1 = this->contour1();
  auto myContour2 = this->contour2();
  return (contour1 == myContour1 && contour2 == myContour2) || (contour1 == myContour2 && contour2 == myContour1);
}

// MARK: ********** FBEdgeOverlapRun **********

void FBEdgeOverlapRun::addCrossings() {
  // Add crossings to both graphs for this intersection/overlap. Pick the middle point and use that
  if (_overlaps.size() == 0)
    return;

  auto middleOverlap = _overlaps[_overlaps.size() / 2];
  middleOverlap->addMiddleCrossing();
}

bool FBEdgeOverlapRun::insertOverlap(std::shared_ptr<FBEdgeOverlap> overlap) {
  if (_overlaps.size() == 0) {
    // The first one always works
    _overlaps.push_back(overlap);
    return true;
  }

  // Check to see if overlap fits after our last overlap
  auto lastOverlap = _overlaps.back();
  if (lastOverlap->fitsBefore(overlap)) {
    _overlaps.push_back(overlap);
    return true;
  }
  // Check to see if overlap fits before our first overlap
  auto firstOverlap = _overlaps[0];
  if (firstOverlap->fitsAfter(overlap)) {
    _overlaps.insert(_overlaps.begin(), overlap);
    return true;
  }
  return false;
}

bool FBEdgeOverlapRun::isComplete() {
  // To be complete, we should wrap around
  if (_overlaps.size() == 0) {
    return false;
  }

  auto lastOverlap = _overlaps.back();
  auto firstOverlap = _overlaps[0];
  return lastOverlap->fitsBefore(firstOverlap);
}

bool FBEdgeOverlapRun::doesContainCrossing(std::shared_ptr<FBEdgeCrossing> crossing) {
  return doesContainParameter(crossing->parameter(), crossing->edge());
}

bool FBEdgeOverlapRun::doesContainParameter(FBFloat parameter, std::shared_ptr<FBBezierCurve> edge) {
  if (_overlaps.size() == 0) {
    return false;
  }

  // Find the FBEdgeOverlap that contains the crossing (if it exists)
  std::shared_ptr<FBEdgeOverlap> containingOverlap = nullptr;
  for (auto &overlap : _overlaps) {
    if (overlap->edge1() == edge || overlap->edge2() == edge) {
      containingOverlap = overlap;
      break;
    }
  }

  // The edge it's attached to isn't here
  if (containingOverlap == nullptr) {
    return false;
  }

  auto lastOverlap = _overlaps.back();
  auto firstOverlap = _overlaps[0];

  bool atTheStart = containingOverlap == firstOverlap;
  bool extendsBeforeStart = !atTheStart || (atTheStart && lastOverlap->fitsBefore(firstOverlap));

  bool atTheEnd = containingOverlap == lastOverlap;
  bool extendsAfterEnd = !atTheEnd || (atTheEnd && firstOverlap->fitsAfter(lastOverlap));

  return containingOverlap->doesContainParameter(parameter, edge, extendsBeforeStart, extendsAfterEnd);
}

bool FBEdgeOverlapRun::isCrossing() {
  // The intersection happens at the end of one of the edges, meaning we'll have to look at the next
  //  edge in sequence to see if it crosses or not. We'll do that by computing the four tangents at
  //  the exact
  //  point the intersection takes place. We'll compute the polar angle for each of the tangents. If
  //  the
  //  angles of self split the angles of edge2 (i.e. they alternate when sorted), then the edges
  //  cross. If
  //  any of the angles are equal or if the angles group up, then the edges don't cross.

  // Calculate the four tangents: The two tangents moving away from the intersection point on self,
  // the two tangents
  //  moving away from the intersection point on edge2.

  auto firstOverlap = _overlaps[0];
  auto lastOverlap = _overlaps.back();

  FBPoint edge1Tangents[] = {FBZeroPoint, FBZeroPoint};
  FBPoint edge2Tangents[] = {FBZeroPoint, FBZeroPoint};
  FBFloat offset = 0.0;
  FBFloat maxOffset = 0.0;

  do {
    FBFloat length1 = FBComputeEdge1Tangents(firstOverlap, lastOverlap, offset, edge1Tangents);
    FBFloat length2 = FBComputeEdge2Tangents(firstOverlap, lastOverlap, offset, edge2Tangents);
    maxOffset = std::min(length1, length2);

    offset += 1.0;
  } while (FBAreTangentsAmbigious(edge1Tangents, edge2Tangents) && offset < maxOffset);

  if (FBTangentsCross(edge1Tangents, edge2Tangents)) {
    return true;
  }

  // Tangents work, mostly, for overlaps. If we get a yes, it's solid. If we get a no, it might
  // still
  //  be a crossing. Only way to tell now is to an actual point test
  FBPoint testPoints[2] = {};
  FBComputeEdge1TestPoints(firstOverlap, lastOverlap, 1.0, testPoints);
  auto contour2 = firstOverlap->edge2()->contour();
  bool testPoint1Inside = contour2->containsPoint(testPoints[0]);
  bool testPoint2Inside = contour2->containsPoint(testPoints[1]);
  return testPoint1Inside != testPoint2Inside;
}

std::shared_ptr<FBBezierContour> FBEdgeOverlapRun::contour1() {
  if (_overlaps.size() == 0) {
    return nullptr;
  }

  auto overlap = _overlaps[0];
  return overlap->edge1()->contour();
}

std::shared_ptr<FBBezierContour> FBEdgeOverlapRun::contour2() {
  if (_overlaps.size() == 0) {
    return nullptr;
  }

  auto overlap = _overlaps[0];
  return overlap->edge2()->contour();
}

// MARK: ********** FBEdgeOverlap **********

bool FBEdgeOverlap::fitsBefore(std::shared_ptr<FBEdgeOverlap> nextOverlap) {
  if (FBAreValuesCloseWithOptions(_range->parameterRange1().maximum, 1.0, FBOverlapThreshold)) {
    // nextOverlap should start at 0 of the next edge
    auto nextEdge = _edge1->next();
    return nextOverlap->edge1() == nextEdge &&
           FBAreValuesCloseWithOptions(nextOverlap->range()->parameterRange1().minimum, 0.0, FBOverlapThreshold);
  }

  // nextOverlap should start at about maximum on the same edge
  return nextOverlap->edge1() == _edge1 &&
         FBAreValuesCloseWithOptions(nextOverlap->range()->parameterRange1().minimum, _range->parameterRange1().maximum,
                                     FBOverlapThreshold);
}

bool FBEdgeOverlap::fitsAfter(std::shared_ptr<FBEdgeOverlap> previousOverlap) {
  if (FBAreValuesCloseWithOptions(_range->parameterRange1().minimum, 0.0, FBOverlapThreshold)) {
    // previousOverlap should end at 1 of the previous edge
    auto previousEdge = _edge1->previous();
    return previousOverlap->edge1() == previousEdge &&
           FBAreValuesCloseWithOptions(previousOverlap->range()->parameterRange1().maximum, 1.0, FBOverlapThreshold);
  }

  // previousOverlap should end at about the minimum on the same edge
  return previousOverlap->edge1() == _edge1 &&
         FBAreValuesCloseWithOptions(previousOverlap->range()->parameterRange1().maximum,
                                     _range->parameterRange1().minimum, FBOverlapThreshold);
}

void FBEdgeOverlap::addMiddleCrossing() {
  auto intersection = _range->middleIntersection();
  auto ourCrossing = std::make_shared<FBEdgeCrossing>(intersection);
  auto theirCrossing = std::make_shared<FBEdgeCrossing>(intersection);
  ourCrossing->setCounterpart(theirCrossing);
  theirCrossing->setCounterpart(ourCrossing);
  ourCrossing->setFromCrossingOverlap(true);
  theirCrossing->setFromCrossingOverlap(true);
  _edge1->addCrossing(ourCrossing);
  _edge2->addCrossing(theirCrossing);
}

bool FBEdgeOverlap::doesContainParameter(FBFloat parameter, std::shared_ptr<FBBezierCurve> edge,
                                         bool extendsBeforeStart, bool extendsAfterEnd) {
  // By the time this is called, we know the crossing is on one of our edges.
  if (extendsBeforeStart && extendsAfterEnd) {
    return true; // The crossing is on the edge somewhere, and the overlap extens past this edge in
                // both directions, so its safe to say the crossing is contained
  }

  FBRange parameterRange = {};
  if (edge == _edge1) {
    parameterRange = _range->parameterRange1();
  } else {
    parameterRange = _range->parameterRange2();
  }

  bool inLeftSide = extendsBeforeStart ? parameter >= 0.0 : parameter > parameterRange.minimum;
  bool inRightSide = extendsAfterEnd ? parameter <= 1.0 : parameter < parameterRange.maximum;
  return inLeftSide && inRightSide;
}

} // namespace fb