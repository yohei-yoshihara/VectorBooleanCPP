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

#include "FBBezierGraph.hpp"

#include "FBBezierContour.hpp"
#include "FBBezierCurve.hpp"
#include "FBBezierIntersectRange.hpp"
#include "FBBezierPath.hpp"
#include "FBContourOverlap.hpp"
#include "FBCurveLocation.hpp"
#include "FBEdgeCrossing.hpp"

#include <sstream>
#include <format>

namespace fb {

////////////////////////////////////////////////////////////////////////
// Boolean operations
//
// The three main boolean operations (union, intersect, difference) follow
//  much the same algorithm. First, the places where the two graphs cross
//  (not just intersect) are marked on the graph with FBEdgeCrossing objects.
//  Next, we decide which sections of the two graphs should appear in the final
//  result. (There are only two kind of sections: those inside of the other graph,
//  and those outside.) We do this by walking all the crossings we created
//  and marking them as entering a section that should appear in the final result,
//  or as exiting the final result. We then walk all the crossings again, and
//  actually output the final result of the graphs that intersect.
//
//  The last part of each boolean operation deals with what do with contours
//  in each graph that don't intersect any other contours.
//
// The exclusive or boolean op is implemented in terms of union, intersect,
//  and difference. More specifically it subtracts the intersection of both
//  graphs from the union of both graphs.
//

std::shared_ptr<FBBezierGraph> FBBezierGraph::unionWithBezierGraph(std::shared_ptr<FBBezierGraph> graph) {
  // First insert FBEdgeCrossings into both graphs where the graphs
  //  cross.
  insertCrossingsWithBezierGraph(graph);
  insertSelfCrossings();
  graph->insertSelfCrossings();
  cleanupCrossingsWithBezierGraph(graph);

  // Handle the parts of the graphs that intersect first. Mark the parts
  //  of the graphs that are outside the other for the final result.
  markCrossingsAsEntryOrExitWithBezierGraph(graph, false);
  graph->markCrossingsAsEntryOrExitWithBezierGraph(shared_from_this(), false);

  // Walk the crossings and actually compute the final result for the intersecting parts
  auto result = bezierGraphFromIntersections();

  // Finally, process the contours that don't cross anything else. They're either
  //  completely contained in another contour, or disjoint.
  unionNonintersectingPartsIntoGraph(result, graph);

  // Clean up crossings so the graphs can be reused, e.g. XOR will reuse graphs.
  removeCrossings();
  graph->removeCrossings();
  removeOverlaps();
  graph->removeOverlaps();

  return result;
}

void FBBezierGraph::unionNonintersectingPartsIntoGraph(std::shared_ptr<FBBezierGraph> result,
                                                       std::shared_ptr<FBBezierGraph> graph) {
  // Finally, process the contours that don't cross anything else. They're either
  //  completely contained in another contour, or disjoint.
  auto ourNonintersectingContours = nonintersectingContours();
  auto theirNonintersectinContours = graph->nonintersectingContours();
  auto finalNonintersectingContours = ourNonintersectingContours;
  std::copy(theirNonintersectinContours.begin(), theirNonintersectinContours.end(),
            std::back_inserter(finalNonintersectingContours));
  unionEquivalentNonintersectingContours(ourNonintersectingContours, theirNonintersectinContours,
                                         finalNonintersectingContours);

  // Since we're doing a union, assume all the non-crossing contours are in, and remove
  //  by exception when they're contained by another contour.
  for (const auto &ourContour : ourNonintersectingContours) {
    // If the other graph contains our contour, it's redundant and we can just remove it
    auto clipContainsSubject = graph->containsContour(ourContour);
    if (clipContainsSubject) {
      finalNonintersectingContours.erase(
          remove(finalNonintersectingContours.begin(), finalNonintersectingContours.end(), ourContour),
          finalNonintersectingContours.end());
    }
  }
  for (const auto &theirContour : theirNonintersectinContours) {
    // If we contain this contour, it's redundant and we can just remove it
    auto subjectContainsClip = containsContour(theirContour);
    if (subjectContainsClip) {
      finalNonintersectingContours.erase(
          remove(finalNonintersectingContours.begin(), finalNonintersectingContours.end(), theirContour),
          finalNonintersectingContours.end());
    }
  }

  // Append the final nonintersecting contours
  for (const auto &contour : finalNonintersectingContours) {
    result->addContour(contour);
  }
}

void FBBezierGraph::unionEquivalentNonintersectingContours(
    std::vector<std::shared_ptr<FBBezierContour>> &ourNonintersectingContours,
    std::vector<std::shared_ptr<FBBezierContour>> &theirNonintersectingContours,
    std::vector<std::shared_ptr<FBBezierContour>> &results) {
  for (std::size_t ourIndex = 0; ourIndex < ourNonintersectingContours.size(); ++ourIndex) {
    auto ourContour = ourNonintersectingContours[ourIndex];
    for (std::size_t theirIndex = 0; theirIndex < theirNonintersectingContours.size(); ++theirIndex) {
      auto theirContour = theirNonintersectingContours[theirIndex];

      if (!ourContour->isEquivalent(theirContour)) {
        continue;
      }

      if (ourContour->inside() == theirContour->inside()) {
        // Redundant, so just remove one of them from the results
        results.erase(std::remove(results.begin(), results.end(), theirContour), results.end());
      } else {
        // One is a hole, one is a fill, so they cancel each other out. Remove both from the results
        results.erase(std::remove(results.begin(), results.end(), theirContour), results.end());
        results.erase(std::remove(results.begin(), results.end(), ourContour), results.end());
      }

      // Remove both from the inputs so they aren't processed later
      theirNonintersectingContours.erase(theirNonintersectingContours.begin() + theirIndex);
      ourNonintersectingContours.erase(ourNonintersectingContours.begin() + ourIndex);
      ourIndex--;
      break;
    }
  }
}

std::shared_ptr<FBBezierGraph> FBBezierGraph::intersectWithBezierGraph(std::shared_ptr<FBBezierGraph> graph) {
  // First insert FBEdgeCrossings into both graphs where the graphs cross.
  insertCrossingsWithBezierGraph(graph);
  insertSelfCrossings();
  graph->insertSelfCrossings();
  cleanupCrossingsWithBezierGraph(graph);

  // Handle the parts of the graphs that intersect first. Mark the parts
  //  of the graphs that are inside the other for the final result.
  markCrossingsAsEntryOrExitWithBezierGraph(graph, true);
  graph->markCrossingsAsEntryOrExitWithBezierGraph(shared_from_this(), true);

  // Walk the crossings and actually compute the final result for the intersecting parts
  auto result = bezierGraphFromIntersections();

  // Finally, process the contours that don't cross anything else. They're either
  //  completely contained in another contour, or disjoint.
  intersectNonintersectingPartsIntoGraph(result, graph);

  // Clean up crossings so the graphs can be reused, e.g. XOR will reuse graphs.
  removeCrossings();
  graph->removeCrossings();
  removeOverlaps();
  graph->removeOverlaps();

  return result;
}

void FBBezierGraph::intersectNonintersectingPartsIntoGraph(std::shared_ptr<FBBezierGraph> result,
                                                           std::shared_ptr<FBBezierGraph> graph) {
  // Finally, process the contours that don't cross anything else. They're either
  //  completely contained in another contour, or disjoint.
  auto ourNonintersectingContours = nonintersectingContours();
  auto theirNonintersectinContours = graph->nonintersectingContours();
  std::vector<std::shared_ptr<FBBezierContour>> finalNonintersectingContours;
  finalNonintersectingContours.reserve(ourNonintersectingContours.size() + theirNonintersectinContours.size());
  intersectEquivalentNonintersectingContours(ourNonintersectingContours, theirNonintersectinContours,
                                             finalNonintersectingContours);
  // Since we're doing an intersect, assume that most of these non-crossing contours shouldn't be in
  //  the final result.
  for (const auto &ourContour : ourNonintersectingContours) {
    // If their graph contains ourContour, then the two graphs intersect (logical AND) at
    // ourContour, so
    //  add it to the final result.
    auto clipContainsSubject = graph->containsContour(ourContour);
    if (clipContainsSubject) {
      finalNonintersectingContours.push_back(ourContour);
    }
  }
  for (const auto &theirContour : theirNonintersectinContours) {
    // If we contain theirContour, then the two graphs intersect (logical AND) at theirContour,
    //  so add it to the final result.
    auto subjectContainsClip = containsContour(theirContour);
    if (subjectContainsClip) {
      finalNonintersectingContours.push_back(theirContour);
    }
  }

  // Append the final nonintersecting contours
  for (const auto &contour : finalNonintersectingContours) {
    result->addContour(contour);
  }
}

void FBBezierGraph::intersectEquivalentNonintersectingContours(
    std::vector<std::shared_ptr<FBBezierContour>> &ourNonintersectingContours,
    std::vector<std::shared_ptr<FBBezierContour>> &theirNonintersectingContours,
    std::vector<std::shared_ptr<FBBezierContour>> &results) {
  for (std::size_t ourIndex = 0; ourIndex < ourNonintersectingContours.size(); ++ourIndex) {
    auto ourContour = ourNonintersectingContours[ourIndex];
    for (std::size_t theirIndex = 0; theirIndex < theirNonintersectingContours.size(); ++theirIndex) {
      auto theirContour = theirNonintersectingContours[theirIndex];

      if (!ourContour->isEquivalent(theirContour)) {
        continue;
      }

      if (ourContour->inside() == theirContour->inside()) {
        // Redundant, so just add one of them to our results
        results.push_back(ourContour);
      } else {
        // One is a hole, one is a fill, so the hole cancels the fill. Add the hole to the results
        if (theirContour->inside() == FBContourInsideHole) {
          // theirContour is the hole, so add it
          results.push_back(theirContour);
        } else {
          // ourContour is the hole, so add it
          results.push_back(ourContour);
        }
      }

      // Remove both from the inputs so they aren't processed later
      theirNonintersectingContours.erase(theirNonintersectingContours.begin() + theirIndex);
      ourNonintersectingContours.erase(ourNonintersectingContours.begin() + ourIndex);
      ourIndex--;
      break;
    }
  }
}

std::shared_ptr<FBBezierGraph> FBBezierGraph::differenceWithBezierGraph(std::shared_ptr<FBBezierGraph> graph) {
  // First insert FBEdgeCrossings into both graphs where the graphs cross.
  insertCrossingsWithBezierGraph(graph);
  insertSelfCrossings();
  graph->insertSelfCrossings();
  cleanupCrossingsWithBezierGraph(graph);

  // Handle the parts of the graphs that intersect first. We're subtracting
  //  graph from outselves. Mark the outside parts of ourselves, and the inside
  //  parts of them for the final result.
  markCrossingsAsEntryOrExitWithBezierGraph(graph, false);
  graph->markCrossingsAsEntryOrExitWithBezierGraph(shared_from_this(), true);

  // Walk the crossings and actually compute the final result for the intersecting parts
  auto result = bezierGraphFromIntersections();

  // Finally, process the contours that don't cross anything else. They're either
  //  completely contained in another contour, or disjoint.
  auto ourNonintersectingContours = nonintersectingContours();
  auto theirNonintersectinContours = graph->nonintersectingContours();
  std::vector<std::shared_ptr<FBBezierContour>> finalNonintersectingContours;
  finalNonintersectingContours.reserve(ourNonintersectingContours.size() + theirNonintersectinContours.size());
  differenceEquivalentNonintersectingContours(ourNonintersectingContours, theirNonintersectinContours,
                                              finalNonintersectingContours);

  // We're doing an subtraction, so assume none of the contours should be in the final result
  for (const auto &ourContour : ourNonintersectingContours) {
    // If ourContour isn't subtracted away (contained by) the other graph, it should stick around,
    //  so add it to our final result.
    auto clipContainsSubject = graph->containsContour(ourContour);
    if (!clipContainsSubject) {
      finalNonintersectingContours.push_back(ourContour);
    }
  }
  for (const auto &theirContour : theirNonintersectinContours) {
    // If our graph contains theirContour, then add theirContour as a hole.
    auto subjectContainsClip = containsContour(theirContour);
    if (subjectContainsClip) {
      finalNonintersectingContours.push_back(theirContour); // add it as a hole
    }
  }

  // Append the final nonintersecting contours
  for (const auto &contour : finalNonintersectingContours) {
    result->addContour(contour);
  }

  // Clean up crossings so the graphs can be reused
  removeCrossings();
  graph->removeCrossings();
  removeOverlaps();
  graph->removeOverlaps();

  return result;
}

void FBBezierGraph::differenceEquivalentNonintersectingContours(
    std::vector<std::shared_ptr<FBBezierContour>> &ourNonintersectingContours,
    std::vector<std::shared_ptr<FBBezierContour>> &theirNonintersectingContours,
    std::vector<std::shared_ptr<FBBezierContour>> &results) {
  for (std::size_t ourIndex = 0; ourIndex < ourNonintersectingContours.size(); ++ourIndex) {
    auto ourContour = ourNonintersectingContours[ourIndex];
    for (std::size_t theirIndex = 0; theirIndex < theirNonintersectingContours.size(); ++theirIndex) {
      auto theirContour = theirNonintersectingContours[theirIndex];

      if (!ourContour->isEquivalent(theirContour)) {
        continue;
      }

      if (ourContour->inside() != theirContour->inside()) {
        // Trying to subtract a hole from a fill or vice versa does nothing, so add the original to
        // the results
        results.push_back(ourContour);
      } else if (ourContour->inside() == FBContourInsideHole && theirContour->inside() == FBContourInsideHole) {
        // Subtracting a hole from a hole is redundant, so just add one of them to the results
        results.push_back(ourContour);
      } else {
        // Both are fills, and subtracting a fill from a fill removes both. So add neither to the
        // results
        //  Intentionally do nothing for this case.
      }

      // Remove both from the inputs so they aren't processed later
      theirNonintersectingContours.erase(theirNonintersectingContours.begin() + theirIndex);
      ourNonintersectingContours.erase(ourNonintersectingContours.begin() + ourIndex);
      ourIndex--;
      break;
    }
  }
}

void FBBezierGraph::markCrossingsAsEntryOrExitWithBezierGraph(std::shared_ptr<FBBezierGraph> otherGraph,
                                                              bool markInside) {
  // Walk each contour in ourself and mark the crossings with each intersecting contour as entering
  //  or exiting the final contour.
  for (auto contour : contours()) {
    auto intersectingContours = contour->intersectingContours();
    for (auto otherContour : intersectingContours) {
      // If the other contour is a hole, that's a special case where we flip marking inside/outside.
      //  For example, if we're doing a union, we'd normally mark the outside of contours. But
      //  if we're unioning with a hole, we want to cut into that hole so we mark the inside instead
      //  of outside.
      if (otherContour->inside() == FBContourInsideHole) {
        contour->markCrossingsAsEntryOrExitWithContour(otherContour, !markInside);
      } else {
        contour->markCrossingsAsEntryOrExitWithContour(otherContour, markInside);
      }
    }
  }
}

std::shared_ptr<FBBezierGraph> FBBezierGraph::xorWithBezierGraph(std::shared_ptr<FBBezierGraph> graph) {
  // XOR is done by combing union (OR), intersect (AND) and difference. Specifically
  //  we compute the union of the two graphs, the intersect of them, then subtract
  //  the intersect from the union.
  // Note that we reuse the resulting graphs, which is why it is important that operations
  //  clean up any crossings when their done, otherwise they could interfere with subsequent
  //  operations.

  // First insert FBEdgeCrossings into both graphs where the graphs
  //  cross.
  insertCrossingsWithBezierGraph(graph);
  insertSelfCrossings();
  graph->insertSelfCrossings();
  cleanupCrossingsWithBezierGraph(graph);

  // Handle the parts of the graphs that intersect first. Mark the parts
  //  of the graphs that are outside the other for the final result.
  markCrossingsAsEntryOrExitWithBezierGraph(graph, false);
  graph->markCrossingsAsEntryOrExitWithBezierGraph(shared_from_this(), false);

  // Walk the crossings and actually compute the final result for the intersecting parts
  auto allParts = bezierGraphFromIntersections();
  unionNonintersectingPartsIntoGraph(allParts, graph);

  markAllCrossingsAsUnprocessed();
  graph->markAllCrossingsAsUnprocessed();

  // Handle the parts of the graphs that intersect first. Mark the parts
  //  of the graphs that are inside the other for the final result.
  markCrossingsAsEntryOrExitWithBezierGraph(graph, true);
  graph->markCrossingsAsEntryOrExitWithBezierGraph(shared_from_this(), true);

  auto intersectingParts = bezierGraphFromIntersections();
  intersectNonintersectingPartsIntoGraph(intersectingParts, graph);

  // Clean up crossings so the graphs can be reused, e.g. XOR will reuse graphs.
  removeCrossings();
  graph->removeCrossings();
  removeOverlaps();
  graph->removeOverlaps();

  return allParts->differenceWithBezierGraph(intersectingParts);
}

void FBBezierGraph::insertCrossingsWithBezierGraph(std::shared_ptr<FBBezierGraph> other) {
  // Find all intersections and, if they cross the other graph, create crossings for them, and
  // insert
  //  them into each graph's edges.
  for (const auto &ourContour : contours()) {
    for (const auto &theirContour : other->contours()) {
      auto overlap = std::make_shared<FBContourOverlap>();

      for (auto ourEdge : ourContour->edges()) {
        for (auto theirEdge : theirContour->edges()) {
          // Find all intersections between these two edges (curves)
          std::shared_ptr<FBBezierIntersectRange> intersectRange = nullptr;
          ourEdge->intersectionsWithBezierCurve(theirEdge, &intersectRange,
                                                [&](std::shared_ptr<FBBezierIntersection> intersection, bool *stop) {
                                                  // If this intersection happens at one of the ends of the edges,
                                                  // then mark
                                                  //  that on the edge. We do this here because not all
                                                  //  intersections create
                                                  //  crossings, but we still need to know when the intersections
                                                  //  fall on end points
                                                  //  later on in the algorithm.
                                                  if (intersection->isAtStartOfCurve1()) {
                                                    ourEdge->setStartShared(true);
                                                  }
                                                  if (intersection->isAtStopOfCurve1()) {
                                                    ourEdge->next()->setStartShared(true);
                                                  }
                                                  if (intersection->isAtStartOfCurve2()) {
                                                    theirEdge->setStartShared(true);
                                                  }
                                                  if (intersection->isAtStopOfCurve2()) {
                                                    theirEdge->next()->setStartShared(true);
                                                  }

                                                  // Don't add a crossing unless one edge actually crosses the
                                                  // other
                                                  if (!ourEdge->crossesEdge(theirEdge, intersection)) {
                                                    return;
                                                  }

                                                  // Add crossings to both graphs for this intersection, and point
                                                  // them at each other
                                                  auto ourCrossing = std::make_shared<FBEdgeCrossing>(intersection);
                                                  auto theirCrossing = std::make_shared<FBEdgeCrossing>(intersection);
                                                  ourCrossing->setCounterpart(theirCrossing);
                                                  theirCrossing->setCounterpart(ourCrossing);
                                                  ourEdge->addCrossing(ourCrossing);
                                                  theirEdge->addCrossing(theirCrossing);
                                                });
          if (intersectRange != nil) {
            overlap->addOverlap(intersectRange, ourEdge, theirEdge);
          }
        } // end theirEdges
      } // end ourEdges

      // At this point we've found all intersections/overlaps between ourContour and theirContour

      // Determine if the overlaps constitute crossings
      if (!overlap->isComplete()) {
        // The contours aren't equivalent so see if they're crossings
        overlap->runsWithBlock([](const std::shared_ptr<FBEdgeOverlapRun> run, bool *stop) {
          if (!run->isCrossing()) {
            return;
          }

          // The two ends of the overlap run should serve as crossings
          run->addCrossings();
        });
      }

      ourContour->addOverlap(overlap);
      theirContour->addOverlap(overlap);
    } // end theirContours
  } // end ourContours
}

void FBBezierGraph::cleanupCrossingsWithBezierGraph(std::shared_ptr<FBBezierGraph> other) {
  // Remove duplicate crossings that can happen at end points of edges
  removeDuplicateCrossings();
  other->removeDuplicateCrossings();
  // Remove crossings that happen in the middle of overlaps that aren't crossings themselves
  removeCrossingsInOverlaps();
  other->removeCrossingsInOverlaps();
}

void FBBezierGraph::removeCrossingsInOverlaps() {
  for (auto ourContour : contours()) {
    for (auto ourEdge : ourContour->edges()) {
      ourEdge->crossingsCopyWithBlock([&](std::shared_ptr<FBEdgeCrossing> crossing, bool *stop) {
        if (crossing->fromCrossingOverlap()) {
          return;
        }

        if (ourContour->doesOverlapContainCrossing(crossing)) {
          auto counterpart = crossing->counterpart();
          crossing->removeFromEdge();
          counterpart->removeFromEdge();
        }
      });
    }
  }
}

void FBBezierGraph::removeDuplicateCrossings() {
  // Find any duplicate crossings. These will happen at the endpoints of edges.
  for (auto ourContour : contours()) {
    for (auto ourEdge : ourContour->edges()) {
      ourEdge->crossingsCopyWithBlock([&](std::shared_ptr<FBEdgeCrossing> crossing, bool *stop) {
        if (crossing->isAtStart() && crossing->edge()->previous()->lastCrossing()->isAtEnd()) {
          // Found a duplicate. Remove this crossing and its counterpart
          auto counterpart = crossing->counterpart();
          crossing->removeFromEdge();
          counterpart->removeFromEdge();
        }
        if (crossing->isAtEnd() && crossing->edge() != nullptr && crossing->edge()->next()->firstCrossing()->isAtStart()) {
          // Found a duplicate. Remove this crossing and its counterpart
          auto counterpart = crossing->edge()->next()->firstCrossing()->counterpart();
          crossing->edge()->next()->firstCrossing()->removeFromEdge();
          counterpart->removeFromEdge();
        }
      });
    }
  }
}

void FBBezierGraph::insertSelfCrossings() {
  // Find all intersections and, if they cross other contours in this graph, create crossings for
  // them, and insert
  //  them into each contour's edges.
  auto remainingContours = contours();
  while (remainingContours.size() > 0) {
    auto firstContour = remainingContours.back();
    for (auto secondContour : remainingContours) {
      // We don't handle self-intersections on the contour this way, so skip them here
      if (firstContour == secondContour) {
        continue;
      }

      if (!FBLineBoundsMightOverlap(firstContour->boundingRect(), secondContour->boundingRect())
          || !FBLineBoundsMightOverlap(firstContour->bounds(), secondContour->bounds())) {
        continue;
      }

      // Compare all the edges between these two contours looking for crossings
      for (auto firstEdge : firstContour->edges()) {
        for (auto secondEdge : secondContour->edges()) {
          // Find all intersections between these two edges (curves)
          firstEdge->intersectionsWithBezierCurve(
              secondEdge, nullptr, [&](std::shared_ptr<FBBezierIntersection> intersection, bool *stop) {
                // If this intersection happens at one of the ends of the edges,
                // then mark
                //  that on the edge. We do this here because not all
                //  intersections create
                //  crossings, but we still need to know when the intersections
                //  fall on end points
                //  later on in the algorithm.
                if (intersection->isAtStartOfCurve1()) {
                  firstEdge->setStartShared(true);
                } else if (intersection->isAtStopOfCurve1()) {
                  firstEdge->next()->setStartShared(true);
                }
                if (intersection->isAtStartOfCurve2()) {
                  secondEdge->setStartShared(true);
                } else if (intersection->isAtStopOfCurve2()) {
                  secondEdge->next()->setStartShared(true);
                }

                // Don't add a crossing unless one edge actually crosses the
                // other
                if (!firstEdge->crossesEdge(secondEdge, intersection)) {
                  return;
                }

                // Add crossings to both graphs for this intersection, and point
                // them at each other
                auto firstCrossing = std::make_shared<FBEdgeCrossing>(intersection);
                auto secondCrossing = std::make_shared<FBEdgeCrossing>(intersection);
                firstCrossing->setSelfCrossing(true);
                secondCrossing->setSelfCrossing(true);
                firstCrossing->setCounterpart(secondCrossing);
                secondCrossing->setCounterpart(firstCrossing);
                firstEdge->addCrossing(firstCrossing);
                secondEdge->addCrossing(secondCrossing);
              });
        }
      }
    }

    // We just compared this contour to all the others, so we don't need to do it again
    remainingContours.pop_back(); // do this at the end of the loop when we're done with it
  }

  // Go through and mark each contour if its a hole or filled region
  for (auto contour : _contours) {
    contour->setInside(contourInsides(contour));
  }
}

FBRect FBBezierGraph::bounds() const {
  // Compute the bounds of the graph by unioning together the bounds of the individual contours
  if (!FBEqualRects(_bounds, FBZeroRect)) {
    return _bounds;
  }
  if (_contours.size() == 0) {
    return FBZeroRect;
  }

  for (const auto &contour : _contours) {
    _bounds = FBUnionRect(_bounds, contour->bounds());
  }

  return _bounds;
}

FBContourInside FBBezierGraph::contourInsides(std::shared_ptr<FBBezierContour> testContour) {
  // Determine if this contour, which should reside in this graph, is a filled region or
  //  a hole. Determine this by casting a ray from one edges of the contour to the outside of
  //  the entire graph. Count how many times the ray intersects a contour in the graph. If it's
  //  an odd number, the test contour resides inside of filled region, meaning it must be a hole.
  //  Otherwise it's "outside" of the graph and creates a filled region.
  // Create the line from the first point in the contour to outside the graph

  // falseTE: this method requires insertSelfCrossings: to be call before it, and the self crossings
  //  to be in place to work

  auto testPoint = testContour->testPointForContainment();
  auto lineEndPoint = FBMakePoint(testPoint.x > FBMinX(bounds()) ? FBMinX(bounds()) - 10 : FBMaxX(bounds()) + 10,
                                  testPoint.y); /* just move us outside the bounds of the graph */
  auto testCurve = std::make_shared<FBBezierCurve>(testPoint, lineEndPoint);

  std::size_t intersectCount = 0;
  for (auto contour : contours()) {
    if (contour == testContour || contour->crossesOwnContour(testContour)) {
      continue; // don't test self intersections
    }

    intersectCount += contour->numberOfIntersectionsWithRay(testCurve);
  }
  return (intersectCount & 1) == 1 ? FBContourInsideHole : FBContourInsideFilled;
}

std::shared_ptr<FBCurveLocation> FBBezierGraph::closestLocationToPoint(const FBPoint &point) {
  std::shared_ptr<FBCurveLocation> closestLocation = nullptr;

  for (const auto &contour : _contours) {
    auto contourLocation = contour->closestLocationToPoint(point);
    if (contourLocation != nullptr
        && (closestLocation == nullptr || contourLocation->distance() < closestLocation->distance())) {
      closestLocation = contourLocation;
    }
  }

  if (closestLocation == nullptr) {
    return nullptr;
  }

  closestLocation->setGraph(shared_from_this());
  return closestLocation;
}

bool FBBezierGraph::containsContour(std::shared_ptr<FBBezierContour> testContour) {
  // Determine the container, if any, for the test contour. We do this by casting a ray from one end
  // of the graph to the other,
  //  and recording the intersections before and after the test contour. If the ray intersects with
  //  a contour an odd number of
  //  times on one side, we know it contains the test contour. After determine which contours
  //  contain the test contour, we simply
  //  pick the closest one to test contour.
  //
  // Things get a bit more complicated though. If contour shares and edge the test contour, then it
  // can be impossible to determine
  //  whom contains whom. Or if we hit the test contour at a location where edges joint together
  //  (i.e. end points).
  //  For this reason, we sit in a loop passing both horizontal and vertical rays through the graph
  //  until we can eliminate the number
  //  of potentially enclosing contours down to 1 or 0. Most times the first ray will find the
  //  correct answer, but in some degenerate
  //  cases it will take a few iterations.

  static const FBFloat FBRayOverlap = 10.0;

  // Do a relatively cheap bounds test first
  if (!FBLineBoundsMightOverlap(bounds(), testContour->bounds())) {
    return false;
  }

  // In the beginning all our contours are possible containers for the test contour.
  auto containers = _contours;

  // Each time through the loop we split the test contour into any increasing amount of pieces
  //  (halves, thirds, quarters, etc) and send a ray along the boundaries. In order to increase
  //  our changes of eliminate all but 1 of the contours, we do both horizontal and vertical rays.
  std::size_t count = std::max(static_cast<size_t>(ceil(FBWidth(testContour->bounds()))),
                          static_cast<size_t>(ceil(FBHeight(testContour->bounds()))));
  for (std::size_t fraction = 2; fraction <= (count * 2); fraction++) {
    auto didEliminate = false;

    // Send the horizontal rays through the test contour and (possibly) through parts of the graph
    auto verticalSpacing = FBHeight(testContour->bounds()) / static_cast<FBFloat>(fraction);
    for (auto y = FBMinY(testContour->bounds()) + verticalSpacing; y < FBMaxY(testContour->bounds());
         y += verticalSpacing) {
      // Construct a line that will reach outside both ends of both the test contour and graph
      auto ray = std::make_shared<FBBezierCurve>(
          FBMakePoint(std::min(FBMinX(bounds()), FBMinX(testContour->bounds())) - FBRayOverlap, y),
          FBMakePoint(std::max(FBMaxX(bounds()), FBMaxX(testContour->bounds())) + FBRayOverlap, y));
      // Eliminate any contours that aren't containers. It's possible for this method to fail, so
      // check the return
      auto eliminated = eliminateContainers(containers, testContour, ray);
      if (eliminated) {
        didEliminate = true;
      }
    }

    // Send the vertical rays through the test contour and (possibly) through parts of the graph
    auto horizontalSpacing = FBWidth(testContour->bounds()) / static_cast<FBFloat>(fraction);
    for (auto x = FBMinX(testContour->bounds()) + horizontalSpacing; x < FBMaxX(testContour->bounds());
         x += horizontalSpacing) {
      // Construct a line that will reach outside both ends of both the test contour and graph
      auto ray = std::make_shared<FBBezierCurve>(
          FBMakePoint(x, std::min(FBMinY(bounds()), FBMinY(testContour->bounds())) - FBRayOverlap),
          FBMakePoint(x, std::max(FBMaxY(bounds()), FBMaxY(testContour->bounds())) + FBRayOverlap));
      // Eliminate any contours that aren't containers. It's possible for this method to fail, so
      // check the return
      auto eliminated = eliminateContainers(containers, testContour, ray);
      if (eliminated) {
        didEliminate = true;
      }
    }

    // If we've eliminated all the contours, then nothing contains the test contour, and we're done
    if (containers.size() == 0) {
      return false;
    }
    // We were able to eliminate someone, and we're down to one, so we're done. If the
    // eliminateContainers: method
    //  failed, we can't make any assumptions about the contains, so just let it go again.
    if (didEliminate) {
      return (containers.size() & 1) == 1;
    }
  }

  // This is a curious case, because by now we've sent rays that went through every integral
  // cordinate of the test contour.
  //  Despite that eliminateContainers: failed each time, meaning one container has a shared edge
  //  for each ray test. It is likely
  //  that contour is equal (the same) as the test contour. Return nil, because if it is equal, it
  //  doesn't contain.
  return false;
}

bool FBBezierGraph::findBoundsOfContour(std::shared_ptr<FBBezierContour> testContour,
                                        std::shared_ptr<FBBezierCurve> ray, FBPoint *testMinimum,
                                        FBPoint *testMaximum) {
  // Find the bounds of test contour that lie on ray. Simply intersect the ray with test contour.
  // For a horizontal ray, the minimum is the point
  //  with the lowest x value, the maximum with the highest x value. For a vertical ray, use the
  //  high and low y values.

  bool horizontalRay = ray->endPoint1().y == ray->endPoint2().y; // ray has to be a vertical or horizontal line

  // First find all the intersections with the ray
  std::vector<std::shared_ptr<FBBezierIntersection>> rayIntersections;
  rayIntersections.reserve(9);
  for (auto edge : testContour->edges()) {
    ray->intersectionsWithBezierCurve(edge, nullptr,
                                      [&](std::shared_ptr<FBBezierIntersection> intersection, bool *stop) {
                                        rayIntersections.push_back(intersection);
                                      });
  }
  if (rayIntersections.size() == 0) {
    return false; // shouldn't happen
  }

  // Next go through and find the lowest and highest
  auto firstRayIntersection = rayIntersections[0];
  *testMinimum = firstRayIntersection->location();
  *testMaximum = *testMinimum;
  for (auto intersection : rayIntersections) {
    if (horizontalRay) {
      if (intersection->location().x < testMinimum->x) {
        *testMinimum = intersection->location();
      }
      if (intersection->location().x > testMaximum->x) {
        *testMaximum = intersection->location();
      }
    } else {
      if (intersection->location().y < testMinimum->y) {
        *testMinimum = intersection->location();
      }
      if (intersection->location().y > testMaximum->y) {
        *testMaximum = intersection->location();
      }
    }
  }
  return true;
}

bool FBBezierGraph::findCrossingsOnContainers(const std::vector<std::shared_ptr<FBBezierContour>> &containers,
                                              std::shared_ptr<FBBezierCurve> ray, FBPoint testMinimum,
                                              FBPoint testMaximum,
                                              std::vector<std::shared_ptr<FBEdgeCrossing>> &crossingsBeforeMinimum,
                                              std::vector<std::shared_ptr<FBEdgeCrossing>> &crossingsAfterMaximum) {
  // Find intersections where the ray intersects the possible containers, before the minimum point,
  // or after the maximum point. Store these
  //  as FBEdgeCrossings in the out parameters.
  bool horizontalRay = ray->endPoint1().y == ray->endPoint2().y; // ray has to be a vertical or horizontal line

  // Walk through each possible container, one at a time and see where it intersects
  std::vector<std::shared_ptr<FBEdgeCrossing>> ambiguousCrossings;
  ambiguousCrossings.reserve(10);
  for (auto container : containers) {
    for (auto containerEdge : container->edges()) {
      // See where the ray intersects this particular edge
      bool ambigious = false;
      ray->intersectionsWithBezierCurve(
          containerEdge, nullptr, [&](std::shared_ptr<FBBezierIntersection> intersection, bool *stop) {
            if (intersection->isTangent()) {
              return; // tangents don't count
            }

            // If the ray intersects one of the contours at a joint (end point),
            // then we won't be able
            //  to make any accurate conclusions, so bail now, and say we failed.
            if (intersection->isAtEndPointOfCurve2()) {
              ambigious = true;
              *stop = true;
              return;
            }

            // If the point likes inside the min and max bounds specified, just
            // skip over it. We only want to remember
            //  the intersections that fall on or outside of the min and max.
            if (horizontalRay && FBIsValueLessThan(intersection->location().x, testMaximum.x)
                && FBIsValueGreaterThan(intersection->location().x, testMinimum.x)) {
              return;
            } else if (!horizontalRay && FBIsValueLessThan(intersection->location().y, testMaximum.y)
                       && FBIsValueGreaterThan(intersection->location().y, testMinimum.y)) {
              return;
            }

            // Creat a crossing for it so we know what edge it is associated
            // with. Don't insert it into a graph or anything though.
            auto crossing = std::make_shared<FBEdgeCrossing>(intersection);
            crossing->setEdge(containerEdge);

            // Special case if the bounds are just a point, and this crossing is
            // on that point. In that case
            //  it could fall on either side, and we'll need to do some special
            //  processing on it later. For now,
            //  remember it, and move on to the next intersection.
            if (FBEqualPoints(testMaximum, testMinimum) && FBEqualPoints(testMaximum, intersection->location())) {
              ambiguousCrossings.push_back(crossing);
              return;
            }

            // This crossing falls outse the bounds, so add it to the appropriate
            // array

            if (horizontalRay && FBIsValueLessThanEqual(intersection->location().x, testMinimum.x)) {
              crossingsBeforeMinimum.push_back(crossing);
            } else if (!horizontalRay && FBIsValueLessThanEqual(intersection->location().y, testMinimum.y)) {
              crossingsBeforeMinimum.push_back(crossing);
            }
            if (horizontalRay && FBIsValueGreaterThanEqual(intersection->location().x, testMaximum.x)) {
              crossingsAfterMaximum.push_back(crossing);
            } else if (!horizontalRay && FBIsValueGreaterThanEqual(intersection->location().y, testMaximum.y)) {
              crossingsAfterMaximum.push_back(crossing);
            }
          });
      if (ambigious) {
        return false;
      }
    }
  }

  // Handle any intersects that are ambigious. i.e. the min and max are one point, and the
  // intersection is on that point.
  for (auto ambiguousCrossing : ambiguousCrossings) {
    // See how many times the given contour crosses on each side. Add the ambigious crossing to the
    // side that has less,
    //  in hopes of balancing it out.
    auto numberOfTimesContourAppearsBefore
        = numberOfTimesContour(ambiguousCrossing->edge()->contour(), crossingsBeforeMinimum);
    auto numberOfTimesContourAppearsAfter
        = numberOfTimesContour(ambiguousCrossing->edge()->contour(), crossingsAfterMaximum);
    if (numberOfTimesContourAppearsBefore < numberOfTimesContourAppearsAfter) {
      crossingsBeforeMinimum.push_back(ambiguousCrossing);
    } else {
      crossingsAfterMaximum.push_back(ambiguousCrossing);
    }
  }

  return true;
}

std::size_t FBBezierGraph::numberOfTimesContour(std::shared_ptr<FBBezierContour> contour,
                                                std::vector<std::shared_ptr<FBEdgeCrossing>> crossings) {
  // Count how many times a contour appears in a crossings array
  size_t count = 0;
  for (auto crossing : crossings) {
    if (crossing->edge()->contour() == contour) {
      ++count;
    }
  }
  return count;
}

bool FBBezierGraph::eliminateContainers(std::vector<std::shared_ptr<FBBezierContour>> &containers,
                                        std::shared_ptr<FBBezierContour> testContour,
                                        std::shared_ptr<FBBezierCurve> ray) {
  // This method attempts to eliminate all or all but one of the containers that might contain test
  // contour, using the ray specified.

  // First determine the exterior bounds of testContour on the given ray
  FBPoint testMinimum = FBZeroPoint;
  FBPoint testMaximum = FBZeroPoint;
  bool foundBounds = findBoundsOfContour(testContour, ray, &testMinimum, &testMaximum);
  if (!foundBounds) {
    return false;
  }

  // Find all the containers on either side of the otherContour
  std::vector<std::shared_ptr<FBEdgeCrossing>> crossingsBeforeMinimum;
  crossingsBeforeMinimum.reserve(containers.size());
  std::vector<std::shared_ptr<FBEdgeCrossing>> crossingsAfterMaximum;
  crossingsAfterMaximum.reserve(containers.size());
  auto foundCrossings = findCrossingsOnContainers(containers, ray, testMinimum, testMaximum, crossingsBeforeMinimum,
                                                  crossingsAfterMaximum);
  if (!foundCrossings) {
    return false;
  }

  // Remove containers that appear an even number of times on either side, because by the even/odd
  // rule
  //  they can't contain test contour.
  removeContoursThatDontContain(crossingsBeforeMinimum);
  removeContoursThatDontContain(crossingsAfterMaximum);

  // Remove containers that appear only on one side
  removeContourCrossings(crossingsBeforeMinimum, crossingsAfterMaximum);
  removeContourCrossings(crossingsAfterMaximum, crossingsBeforeMinimum);

  // Although crossingsBeforeMinimum and crossingsAfterMaximum contain different crossings, they
  // should contain the same
  //  contours, so just pick one to pull the contours from
  containers = contoursFromCrossings(crossingsBeforeMinimum);

  return true;
}

std::vector<std::shared_ptr<FBBezierContour>>
FBBezierGraph::contoursFromCrossings(const std::vector<std::shared_ptr<FBEdgeCrossing>> &crossings) {
  // Determine all the unique contours in the array of crossings
  std::vector<std::shared_ptr<FBBezierContour>> contours;
  contours.reserve(crossings.size());
  for (auto crossing : crossings) {
    auto it = std::find(contours.begin(), contours.end(), crossing->edge()->contour());
    if (it == contours.end()) {
      contours.push_back(crossing->edge()->contour());
    }
  }
  return contours;
}

void FBBezierGraph::removeContourCrossings(std::vector<std::shared_ptr<FBEdgeCrossing>> &crossings1,
                                           std::vector<std::shared_ptr<FBEdgeCrossing>> &crossings2) {
  // If a contour appears in crossings1, but not crossings2, remove all the associated crossings
  // from
  //  crossings1.
  std::vector<std::shared_ptr<FBBezierContour>> containersToRemove;
  containersToRemove.reserve(crossings1.size());
  for (auto crossingToTest : crossings1) {
    auto containerToTest = crossingToTest->edge()->contour();
    // See if this contour exists in the other array
    auto existsInOther = false;
    for (auto crossing : crossings2) {
      if (crossing->edge()->contour() == containerToTest) {
        existsInOther = true;
        break;
      }
    }
    // If it doesn't exist in our counterpart, mark it for death
    if (!existsInOther) {
      containersToRemove.push_back(containerToTest);
    }
  }
  removeCrossings(crossings1, containersToRemove);
}

void FBBezierGraph::removeContoursThatDontContain(std::vector<std::shared_ptr<FBEdgeCrossing>> &crossings) {
  // Remove contours that cross the ray an even number of times. By the even/odd rule this means
  //  they can't contain the test contour.
  std::vector<std::shared_ptr<FBBezierContour>> containersToRemove;
  containersToRemove.reserve(crossings.size());
  for (auto crossingToTest : crossings) {
    // For this contour, count how many times it appears in the crossings array
    auto containerToTest = crossingToTest->edge()->contour();
    std::size_t count = 0;
    for (auto crossing : crossings) {
      if (crossing->edge()->contour() == containerToTest) {
        ++count;
      }
    }
    // If it's not an odd number of times, it doesn't contain the test contour, so mark it for death
    if ((count % 2) != 1) {
      containersToRemove.push_back(containerToTest);
    }
  }
  removeCrossings(crossings, containersToRemove);
}

void FBBezierGraph::removeCrossings(std::vector<std::shared_ptr<FBEdgeCrossing>> &crossings,
                                    const std::vector<std::shared_ptr<FBBezierContour>> &containersToRemove) {
  // A helper method that goes through and removes all the crossings that appear on the specified
  //  contours.

  // First walk through and identify which crossings to remove
  std::vector<std::shared_ptr<FBEdgeCrossing>> crossingsToRemove;
  crossingsToRemove.reserve(crossings.size());
  for (auto contour : containersToRemove) {
    for (auto crossing : crossings) {
      if (crossing->edge()->contour() == contour) {
        crossingsToRemove.push_back(crossing);
      }
    }
  }
  // Now walk through and remove the crossings
  for (auto crossing : crossingsToRemove) {
    crossings.erase(std::remove(crossings.begin(), crossings.end(), crossing), crossings.end());
  }
}

void FBBezierGraph::markAllCrossingsAsUnprocessed() {
  for (auto contour : _contours) {
    for (auto edge : contour->edges()) {
      edge->crossingsCopyWithBlock(
          [](std::shared_ptr<FBEdgeCrossing> crossing, bool *stop) { crossing->setProcessed(false); });
    }
  }
}

std::shared_ptr<FBEdgeCrossing> FBBezierGraph::firstUnprocessedCrossing() {
  // Find the first crossing in our graph that has yet to be processed by the
  // bezierGraphFromIntersections
  //  method.

  for (auto contour : _contours) {
    for (auto edge : contour->edges()) {
      std::shared_ptr<FBEdgeCrossing> unprocessedCrossing = nil;
      edge->crossingsWithBlock([&](std::shared_ptr<FBEdgeCrossing> crossing, bool *stop) {
        if (crossing->isSelfCrossing()) {
          return;
        }
        if (!crossing->isProcessed()) {
          unprocessedCrossing = crossing;
          *stop = true;
        }
      });
      if (unprocessedCrossing != nil) {
        return unprocessedCrossing;
      }
    }
  }
  return nil;
}

std::shared_ptr<FBBezierGraph> FBBezierGraph::bezierGraphFromIntersections() {
  // This method walks the current graph, starting at the crossings, and outputs the final contours
  //  of the parts of the graph that actually intersect. The general algorithm is: start an crossing
  //  we haven't seen before. If it's marked as entry, start outputing edges moving forward (i.e.
  //  using edge.next)
  //  until another crossing is hit. (If a crossing is marked as exit, start outputting edges move
  //  backwards, using
  //  edge.previous.) Once the next crossing is hit, switch to the crossing's counter part in the
  //  other graph,
  //  and process it in the same way. Continue this until we reach a crossing that's been processed.

  auto result = std::make_shared<FBBezierGraph>();

  // Find the first crossing to start one
  auto crossing = firstUnprocessedCrossing();
  while (crossing != nil) {
    // This is the start of a contour, so create one
    auto contour = std::make_shared<FBBezierContour>();
    result->addContour(contour);

    // Keep going until we run into a crossing we've seen before.
    while (!crossing->isProcessed()) {
      crossing->setProcessed(true); // ...and we've just seen this one

      if (crossing->isEntry()) {
        // Keep going to next until meet a crossing
        contour->addCurve(crossing, crossing->nextNonself());
        if (crossing->nextNonself() == nil) {
          // We hit the end of the edge without finding another crossing, so go find the next
          // crossing
          auto edge = crossing->edge()->next();
          while (!edge->hasNonselfCrossings()) {
            // output this edge whole
            contour->addCurve(edge->clone()); // make a copy to add. contours don't share too good

            edge = edge->next();
          }
          // We have an edge that has at least one crossing
          crossing = edge->firstNonselfCrossing();
          contour->addCurve(nil, crossing); // add the curve up to the crossing
        } else {
          crossing = crossing->nextNonself(); // this edge has a crossing, so just move to it
        }
      } else {
        // Keep going to previous until meet a crossing
        contour->addReverseCurve(crossing->previousNonself(), crossing);
        if (crossing->previousNonself() == nil) {
          // we hit the end of the edge without finding another crossing, so go find the previous
          // crossing
          auto edge = crossing->edge()->previous();
          while (!edge->hasNonselfCrossings()) {
            // output this edge whole
            contour->addReverseCurve(edge);

            edge = edge->previous();
          }
          // We have an edge that has at least one edge
          crossing = edge->lastNonselfCrossing();
          contour->addReverseCurve(crossing, nil); // add the curve up to the crossing
        } else {
          crossing = crossing->previousNonself();
        }
      }

      // Switch over to counterpart in the other graph
      crossing->setProcessed(true);
      crossing = crossing->counterpart();
    }

    // See if there's another contour that we need to handle
    crossing = firstUnprocessedCrossing();
  }

  return result;
}

void FBBezierGraph::removeCrossings() {
  // Crossings only make sense for the intersection between two specific graphs. In order for this
  //  graph to be usable in the future, remove all the crossings
  for (auto contour : _contours) {
    for (auto edge : contour->edges()) {
      edge->removeAllCrossings();
    }
  }
}

void FBBezierGraph::removeOverlaps() {
  for (auto contour : _contours) {
    contour->removeAllOverlaps();
  }
}

void FBBezierGraph::addContour(std::shared_ptr<FBBezierContour> contour) {
  // Add a contour to ouselves, and force the bounds to be recalculated
  _contours.push_back(contour);
  _bounds = FBZeroRect;
}

std::vector<std::shared_ptr<FBBezierContour>> FBBezierGraph::nonintersectingContours() {
  // Find all the contours that have no crossings on them.
  std::vector<std::shared_ptr<FBBezierContour>> contours;
  contours.reserve(_contours.size());
  for (auto contour : this->contours()) {
    if (contour->intersectingContours().size() == 0) {
      contours.push_back(contour);
    }
  }
  return contours;
}

FBBezierGraph::FBBezierGraph(const FBBezierPath &path) {
  // A bezier graph is made up of contours, which are closed paths of curves. Anytime we
  //  see a move to in the NSBezierPath, that's a new contour.
  FBPoint lastPoint = FBZeroPoint;
  bool wasClosed = false;
  std::shared_ptr<FBBezierContour> contour = nil;
  for (size_t i = 0; i < path.size(); i++) {
    const auto &element = path[i];

    switch (element.type) {
    case FBBezierPath::Type::move: {
      // if previous contour wasn't closed, close it

      if (!wasClosed && contour != nil) {
        contour->close();
      }

      wasClosed = false;

      // Start a new contour
      contour = std::make_shared<FBBezierContour>();
      this->addContour(contour);

      lastPoint = element.points[0];
      break;
    }

    case FBBezierPath::Type::line: {
      // [MO] skip degenerate line segments
      if (!FBEqualPoints(element.points[0], lastPoint)) {
        // Convert lines to bezier curves as well. Just set control point to be in the line formed
        //  by the end points
        contour->addCurve(std::make_shared<FBBezierCurve>(lastPoint, element.points[0]));

        lastPoint = element.points[0];
      }
      break;
    }

    case FBBezierPath::Type::curve: {
      // GPC: skip degenerate case where all points are equal

      if (FBEqualPoints(element.points[2], lastPoint) && FBEqualPoints(element.points[2], element.points[0])
          && FBEqualPoints(element.points[2], element.points[1])) {
        continue;
      }

      contour->addCurve(
          std::make_shared<FBBezierCurve>(lastPoint, element.points[0], element.points[1], element.points[2]));

      lastPoint = element.points[2];
      break;
    }
    case FBBezierPath::Type::close:
      // [MO] attempt to close the bezier contour by
      // mapping closepaths to equivalent lineto operations,
      // though as with our NSLineToBezierPathElement processing,
      // we check so as not to add degenerate line segments which
      // blow up the clipping code.

      if (contour->edges().size()) {
        auto firstEdge = contour->edges()[0];
        auto firstPoint = firstEdge->endPoint1();

        // Skip degenerate line segments
        if (!FBEqualPoints(lastPoint, firstPoint)) {
          contour->addCurve(std::make_shared<FBBezierCurve>(lastPoint, firstPoint));
          wasClosed = true;
        }
      }
      lastPoint = FBZeroPoint;
      break;
    }
  }

  if (!wasClosed && contour != nil) {
    contour->close();
  }

  // まったくエッジのないContourが作成されてしまうので、エッジが0ならば削除する
  _contours.erase(
      std::remove_if(_contours.begin(), _contours.end(), [](const auto &contour) { return contour->edges().empty(); }),
      _contours.end());
}

FBBezierPath FBBezierGraph::bezierPath() const {
  // Convert this graph into a bezier path. This is straightforward, each contour
  //  starting with a move to and each subsequent edge being translated by doing
  //  a curve to.
  // Be sure to mark the winding rule as even odd, or interior contours (holes)
  //  won't get filled/left alone properly.
  FBBezierPath path;

  for (const auto &contour : _contours) {
    bool firstPoint = true;
    for (const auto &edge : contour->edges()) {
      if (firstPoint) {
        path.moveTo(edge->endPoint1());
        firstPoint = false;
      }

      if (edge->isStraightLine()) {
        path.lineTo(edge->endPoint2());
      } else {
        path.curveTo(edge->endPoint2(), edge->controlPoint1(), edge->controlPoint2());
      }
    }
    path.close(); // GPC: close each contour
  }

  return path;
}

std::string FBBezierGraph::str(int indent) const {
  std::ostringstream ss;
  ss << std::format("{}<FBBezierGraph: bounds={} contours=[", fb::indent(indent), bounds());
  if (indent >= 0) {
    ss << '\n';
  }
  for (std::size_t i = 0; i < _contours.size(); ++i) {
    ss << _contours[i]->str(indent >= 0 ? indent + 2 : indent);
    if (i != _contours.size() - 1) {
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
