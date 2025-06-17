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

#include "FBEdgeCrossing.hpp"

#include "FBBezierCurve.hpp"
#include "FBBezierIntersection.hpp"

namespace fb {

void FBEdgeCrossing::removeFromEdge() {
  if (!_edge.expired()) {
    _edge.lock()->removeCrossing(shared_from_this());
  }
}

std::shared_ptr<FBEdgeCrossing> FBEdgeCrossing::next() {
  if (_edge.expired()) {
    return nullptr;
  }
  return _edge.lock()->nextCrossing(shared_from_this());
}

std::shared_ptr<FBEdgeCrossing> FBEdgeCrossing::previous() {
  if (_edge.expired()) {
    return nullptr;
  }
  return _edge.lock()->previousCrossing(shared_from_this());
}

std::shared_ptr<FBEdgeCrossing> FBEdgeCrossing::nextNonself() {
  auto next = this->next();
  while (next != nullptr && next->isSelfCrossing()) {
    next = next->next();
  }
  return next;
}

std::shared_ptr<FBEdgeCrossing> FBEdgeCrossing::previousNonself() {
  auto previous = this->previous();
  while (previous != nullptr && previous->isSelfCrossing()) {
    previous = previous->previous();
  }
  return previous;
}

FBFloat FBEdgeCrossing::parameter() {
  if (_edge.lock() == _intersection->curve1()) {
    return _intersection->parameter1();
  }

  return _intersection->parameter2();
}

FBPoint FBEdgeCrossing::location() { return _intersection->location(); }

std::shared_ptr<FBBezierCurve> FBEdgeCrossing::curve() { return _edge.lock(); }

std::shared_ptr<FBBezierCurve> FBEdgeCrossing::leftCurve() {
  if (isAtStart()) {
    return nullptr;
  }

  if (_edge.lock() == _intersection->curve1()) {
    return _intersection->curve1LeftBezier();
  }

  return _intersection->curve2LeftBezier();
}

std::shared_ptr<FBBezierCurve> FBEdgeCrossing::rightCurve() {
  if (isAtEnd()) {
    return nullptr;
  }

  if (_edge.lock() == _intersection->curve1()) {
    return _intersection->curve1RightBezier();
  }

  return _intersection->curve2RightBezier();
}

bool FBEdgeCrossing::isAtStart() {
  if (_edge.lock() == _intersection->curve1()) {
    return _intersection->isAtStartOfCurve1();
  }

  return _intersection->isAtStartOfCurve2();
}

bool FBEdgeCrossing::isAtEnd() {
  if (_edge.lock() == _intersection->curve1()) {
    return _intersection->isAtStopOfCurve1();
  }

  return _intersection->isAtStopOfCurve2();
}

} // namespace fb
