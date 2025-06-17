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

#include "FBBezierIntersection.hpp"
#include "FBBezierCurve.hpp"
#include "FBGeometry.hpp"

namespace fb {

static const FBFloat FBPointCloseThreshold = 1e-7;
const FBFloat FBParameterCloseThreshold = 1e-4;

bool FBBezierIntersection::isTangent() const {
  // If we're at the end of a curve, it's not tangent, so skip all the calculations
  if (isAtEndPointOfCurve()) {
    return false;
  }

  computeCurve1();
  computeCurve2();

  // Compute the tangents at the intersection.
  FBPoint curve1LeftTangent =
      FBNormalizePoint(FBSubtractPoint(_curve1LeftBezier->controlPoint2(), _curve1LeftBezier->endPoint2()));
  FBPoint curve1RightTangent =
      FBNormalizePoint(FBSubtractPoint(_curve1RightBezier->controlPoint1(), _curve1RightBezier->endPoint1()));
  FBPoint curve2LeftTangent =
      FBNormalizePoint(FBSubtractPoint(_curve2LeftBezier->controlPoint2(), _curve2LeftBezier->endPoint2()));
  FBPoint curve2RightTangent =
      FBNormalizePoint(FBSubtractPoint(_curve2RightBezier->controlPoint1(), _curve2RightBezier->endPoint1()));

  // See if the tangents are the same. If so, then we're tangent at the intersection point
  return FBArePointsCloseWithOptions(curve1LeftTangent, curve2LeftTangent, FBPointCloseThreshold) ||
         FBArePointsCloseWithOptions(curve1LeftTangent, curve2RightTangent, FBPointCloseThreshold) ||
         FBArePointsCloseWithOptions(curve1RightTangent, curve2LeftTangent, FBPointCloseThreshold) ||
         FBArePointsCloseWithOptions(curve1RightTangent, curve2RightTangent, FBPointCloseThreshold);
}

bool FBBezierIntersection::isAtStartOfCurve1() const {
  return FBAreValuesCloseWithOptions(_parameter1, 0.0, FBParameterCloseThreshold) || _curve1->isPoint();
}
bool FBBezierIntersection::isAtStopOfCurve1() const {
  return FBAreValuesCloseWithOptions(_parameter1, 1.0, FBParameterCloseThreshold) || _curve1->isPoint();
}
bool FBBezierIntersection::isAtEndPointOfCurve1() const { return isAtStartOfCurve1() || isAtStopOfCurve1(); }

bool FBBezierIntersection::isAtStartOfCurve2() const {
  return FBAreValuesCloseWithOptions(_parameter2, 0.0, FBParameterCloseThreshold) || _curve2->isPoint();
}
bool FBBezierIntersection::isAtStopOfCurve2() const {
  return FBAreValuesCloseWithOptions(_parameter2, 1.0, FBParameterCloseThreshold) || _curve2->isPoint();
}

bool FBBezierIntersection::isAtEndPointOfCurve2() const { return isAtStartOfCurve2() || isAtStopOfCurve2(); }
bool FBBezierIntersection::isAtEndPointOfCurve() const { return isAtEndPointOfCurve1() || isAtEndPointOfCurve2(); }

void FBBezierIntersection::computeCurve1() const {
  if (!_needToComputeCurve1) {
    return;
  }

  auto result = _curve1->pointAtParameter(_parameter1);
  _location = std::get<0>(result);
  _curve1LeftBezier = std::get<1>(result);
  _curve1RightBezier = std::get<2>(result);

  _needToComputeCurve1 = false;
}

void FBBezierIntersection::computeCurve2() const {
  if (!_needToComputeCurve2) {
    return;
  }

  auto result = _curve2->pointAtParameter(_parameter2);
  _location = std::get<0>(result);
  _curve2LeftBezier = std::get<1>(result);
  _curve2RightBezier = std::get<2>(result);

  _needToComputeCurve2 = false;
}

} // namespace fb