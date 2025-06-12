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

#include "FBBezierIntersectRange.hpp"
#include "FBBezierCurve.hpp"

namespace fb {

FBBezierIntersectRange::FBBezierIntersectRange(std::shared_ptr<const FBBezierCurve> curve1, FBRange parameterRange1,
                                               std::shared_ptr<const FBBezierCurve> curve2, FBRange parameterRange2,
                                               bool reversed)
    : _curve1(curve1)
    , _parameterRange1(parameterRange1)
    , _needToComputeCurve1(true)
    , _curve2(curve2)
    , _parameterRange2(parameterRange2)
    , _reversed(reversed)
    , _needToComputeCurve2(true) {}

std::shared_ptr<FBBezierCurve> FBBezierIntersectRange::curve1LeftBezier() const {
  computeCurve1();
  return _curve1LeftBezier;
}
std::shared_ptr<FBBezierCurve> FBBezierIntersectRange::curve1RightBezier() const {
  computeCurve1();
  return _curve1RightBezier;
}
std::shared_ptr<FBBezierCurve> FBBezierIntersectRange::curve1OverlappingBezier() const {
  computeCurve1();
  return _curve1MiddleBezier;
}

std::shared_ptr<FBBezierCurve> FBBezierIntersectRange::curve2LeftBezier() const {
  computeCurve2();
  return _curve2LeftBezier;
}
std::shared_ptr<FBBezierCurve> FBBezierIntersectRange::curve2RightBezier() const {
  computeCurve2();
  return _curve2RightBezier;
}
std::shared_ptr<FBBezierCurve> FBBezierIntersectRange::curve2OverlappingBezier() const {
  computeCurve2();
  return _curve2MiddleBezier;
}

void FBBezierIntersectRange::computeCurve1() const {
  if (!_needToComputeCurve1) {
    return;
  }

  auto result = _curve1->splitSubcurvesWithRange(_parameterRange1);
  _curve1LeftBezier = std::get<0>(result);
  _curve1MiddleBezier = std::get<1>(result);
  _curve1RightBezier = std::get<2>(result);

  _needToComputeCurve1 = false;
}

void FBBezierIntersectRange::computeCurve2() const {
  if (!_needToComputeCurve2) {
    return;
  }

  auto result = _curve1->splitSubcurvesWithRange(_parameterRange2);
  _curve2LeftBezier = std::get<0>(result);
  _curve2MiddleBezier = std::get<1>(result);
  _curve2RightBezier = std::get<2>(result);

  _needToComputeCurve2 = false;
}

bool FBBezierIntersectRange::isAtStartOfCurve1() const {
  return FBAreValuesCloseWithOptions(_parameterRange1.minimum, 0.0, FBParameterCloseThreshold);
}

bool FBBezierIntersectRange::isAtStopOfCurve1() const {
  return FBAreValuesCloseWithOptions(_parameterRange1.maximum, 1.0, FBParameterCloseThreshold);
}

bool FBBezierIntersectRange::isAtStartOfCurve2() const {
  return FBAreValuesCloseWithOptions(_parameterRange2.minimum, 0.0, FBParameterCloseThreshold);
}

bool FBBezierIntersectRange::isAtStopOfCurve2() const {
  return FBAreValuesCloseWithOptions(_parameterRange2.maximum, 1.0, FBParameterCloseThreshold);
}

std::shared_ptr<FBBezierIntersection> FBBezierIntersectRange::middleIntersection() const {
  return std::make_shared<FBBezierIntersection>(_curve1, (_parameterRange1.minimum + _parameterRange1.maximum) / 2.0,
                                                _curve2, (_parameterRange2.minimum + _parameterRange2.maximum) / 2.0);
}

void FBBezierIntersectRange::merge(const std::shared_ptr<FBBezierIntersectRange> &other) {
  // We assume the caller already knows we're talking about the same curves
  _parameterRange1 = FBRangeUnion(_parameterRange1, other->_parameterRange1);
  _parameterRange2 = FBRangeUnion(_parameterRange2, other->_parameterRange2);

  clearCache();
}

void FBBezierIntersectRange::clearCache() const {
  _needToComputeCurve1 = true;
  _needToComputeCurve2 = true;
  _curve1LeftBezier = nullptr;
  _curve1MiddleBezier = nullptr;
  _curve1RightBezier = nullptr;
  _curve2LeftBezier = nullptr;
  _curve2MiddleBezier = nullptr;
  _curve2RightBezier = nullptr;
}

} // namespace fb
