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

class FBBezierIntersection;
class FBBezierCurve;

// FBEdgeCrossing is used by the boolean operations code to hold data about
//  where two edges actually cross (as opposed to just intersect). The main
//  piece of data is the intersection, but it also holds a pointer to the
//  crossing's counterpart in the other FBBezierGraph
class FBEdgeCrossing : public std::enable_shared_from_this<FBEdgeCrossing> {
  std::shared_ptr<FBBezierIntersection> _intersection;
  std::weak_ptr<FBBezierCurve> _edge;
  std::weak_ptr<FBEdgeCrossing> _counterpart;
  bool _fromCrossingOverlap;
  bool _entry;
  bool _processed;
  bool _selfCrossing;
  size_t _index;

public:
  FBEdgeCrossing(std::shared_ptr<FBBezierIntersection> intersection)
      : _intersection(intersection) {}
  void removeFromEdge();

  std::shared_ptr<FBBezierCurve> edge() const { return _edge.lock(); }
  void setEdge(std::shared_ptr<FBBezierCurve> edge) { _edge = edge; }
  std::shared_ptr<FBEdgeCrossing> counterpart() const { return _counterpart.lock(); }
  void setCounterpart(std::shared_ptr<FBEdgeCrossing> counterpart) { _counterpart = counterpart; }
  FBFloat order() { return parameter(); };
  bool isEntry() const { return _entry; }
  void setEntry(bool entry) { _entry = entry; }
  bool isProcessed() const { return _processed; }
  void setProcessed(bool processed) { _processed = processed; }
  bool isSelfCrossing() const { return _selfCrossing; }
  void setSelfCrossing(bool selfCrossing) { _selfCrossing = selfCrossing; }
  bool fromCrossingOverlap() const { return _fromCrossingOverlap; }
  void setFromCrossingOverlap(bool fromCrossingOverlap) { _fromCrossingOverlap = fromCrossingOverlap; }
  std::size_t index() const { return _index; }
  void setIndex(std::size_t index) { _index = index; }

  std::shared_ptr<FBEdgeCrossing> next();
  std::shared_ptr<FBEdgeCrossing> previous();
  std::shared_ptr<FBEdgeCrossing> nextNonself();
  std::shared_ptr<FBEdgeCrossing> previousNonself();

  FBFloat parameter();
  std::shared_ptr<FBBezierCurve> curve();
  std::shared_ptr<FBBezierCurve> leftCurve();
  std::shared_ptr<FBBezierCurve> rightCurve();
  bool isAtStart();
  bool isAtEnd();
  FBPoint location();
};

} // namespace fb