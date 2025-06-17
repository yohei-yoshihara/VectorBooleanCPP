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
#include <memory>

#include "FBBezierContour.hpp"

namespace fb {

class FBBezierPath;
class FBBezierContour;
class FBBezierCurve;
class FBEdgeCrossing;

class FBBezierGraph : public std::enable_shared_from_this<FBBezierGraph> {
private:
  std::vector<std::shared_ptr<FBBezierContour>> _contours;
  mutable FBRect _bounds;

protected:
  std::shared_ptr<FBCurveLocation> closestLocationToPoint(const FBPoint &point);

  void removeCrossingsInOverlaps();
  void removeDuplicateCrossings();
  void insertCrossingsWithBezierGraph(std::shared_ptr<FBBezierGraph> other);
  std::shared_ptr<FBEdgeCrossing> firstUnprocessedCrossing();
  void markCrossingsAsEntryOrExitWithBezierGraph(std::shared_ptr<FBBezierGraph> otherGraph, bool markInside);
  std::shared_ptr<FBBezierGraph> bezierGraphFromIntersections();
  void removeCrossings();
  void removeOverlaps();
  void cleanupCrossingsWithBezierGraph(std::shared_ptr<FBBezierGraph> other);

  void insertSelfCrossings();
  void markAllCrossingsAsUnprocessed();

  void unionNonintersectingPartsIntoGraph(std::shared_ptr<FBBezierGraph> result, std::shared_ptr<FBBezierGraph> graph);
  void
  unionEquivalentNonintersectingContours(std::vector<std::shared_ptr<FBBezierContour>> &ourNonintersectingContours,
                                         std::vector<std::shared_ptr<FBBezierContour>> &theirNonintersectingContours,
                                         std::vector<std::shared_ptr<FBBezierContour>> &results);
  void intersectNonintersectingPartsIntoGraph(std::shared_ptr<FBBezierGraph> result,
                                              std::shared_ptr<FBBezierGraph> graph);
  void intersectEquivalentNonintersectingContours(
      std::vector<std::shared_ptr<FBBezierContour>> &ourNonintersectingContours,
      std::vector<std::shared_ptr<FBBezierContour>> &theirNonintersectingContours,
      std::vector<std::shared_ptr<FBBezierContour>> &results);
  void differenceEquivalentNonintersectingContours(
      std::vector<std::shared_ptr<FBBezierContour>> &ourNonintersectingContours,
      std::vector<std::shared_ptr<FBBezierContour>> &theirNonintersectingContours,
      std::vector<std::shared_ptr<FBBezierContour>> &results);

  FBContourInside contourInsides(std::shared_ptr<FBBezierContour> contour);

  std::vector<std::shared_ptr<FBBezierContour>> nonintersectingContours();
  bool containsContour(std::shared_ptr<FBBezierContour> contour);
  bool eliminateContainers(std::vector<std::shared_ptr<FBBezierContour>> &containers,
                           std::shared_ptr<FBBezierContour> testContour, std::shared_ptr<FBBezierCurve> ray);
  bool findBoundsOfContour(std::shared_ptr<FBBezierContour> testContour, std::shared_ptr<FBBezierCurve> ray,
                           FBPoint *testMinimum, FBPoint *testMaximum);
  void removeContoursThatDontContain(std::vector<std::shared_ptr<FBEdgeCrossing>> &crossings);
  bool findCrossingsOnContainers(const std::vector<std::shared_ptr<FBBezierContour>> &containers,
                                 std::shared_ptr<FBBezierCurve> ray, FBPoint testMinimum, FBPoint testMaximum,
                                 std::vector<std::shared_ptr<FBEdgeCrossing>> &crossingsBeforeMinimum,
                                 std::vector<std::shared_ptr<FBEdgeCrossing>> &crossingsAfterMaximum);
  void removeCrossings(std::vector<std::shared_ptr<FBEdgeCrossing>> &crossings,
                       const std::vector<std::shared_ptr<FBBezierContour>> &containersToRemove);
  void removeContourCrossings(std::vector<std::shared_ptr<FBEdgeCrossing>> &crossings1,
                              std::vector<std::shared_ptr<FBEdgeCrossing>> &crossings2);
  std::vector<std::shared_ptr<FBBezierContour>>
  contoursFromCrossings(const std::vector<std::shared_ptr<FBEdgeCrossing>> &crossings);
  std::size_t numberOfTimesContour(std::shared_ptr<FBBezierContour> contour,
                                   std::vector<std::shared_ptr<FBEdgeCrossing>> crossings);

public:
  FBBezierGraph() = default;
  FBBezierGraph(const FBBezierPath &path);
  FBBezierPath bezierPath() const;
  const std::vector<std::shared_ptr<FBBezierContour>> &contour() const { return _contours; }
  FBRect bounds() const;
  void addContour(std::shared_ptr<FBBezierContour> contour);
  const std::vector<std::shared_ptr<FBBezierContour>> &contours() { return _contours; };

  std::shared_ptr<FBBezierGraph> unionWithBezierGraph(std::shared_ptr<FBBezierGraph> graph);
  std::shared_ptr<FBBezierGraph> intersectWithBezierGraph(std::shared_ptr<FBBezierGraph> graph);
  std::shared_ptr<FBBezierGraph> differenceWithBezierGraph(std::shared_ptr<FBBezierGraph> graph);
  std::shared_ptr<FBBezierGraph> xorWithBezierGraph(std::shared_ptr<FBBezierGraph> graph);

  std::string str(int indent = -1) const;
};

} // namespace fb

template <> struct std::formatter<fb::FBBezierGraph> : std::formatter<std::string> {
  auto format(const fb::FBBezierGraph &graph, std::format_context &ctx) const {
    return std::formatter<std::string>::format(graph.str(), ctx);
  }
};

template <> struct std::formatter<std::shared_ptr<fb::FBBezierGraph>> : std::formatter<std::string> {
  auto format(const std::shared_ptr<fb::FBBezierGraph> &graph, std::format_context &ctx) const {
    return std::formatter<std::string>::format(graph->str(), ctx);
  }
};
