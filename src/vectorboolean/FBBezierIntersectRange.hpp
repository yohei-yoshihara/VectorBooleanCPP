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
#include "FBBezierIntersection.hpp"
#include "FBGeometry.hpp"

namespace fb {

class FBBezierCurve;

class FBBezierIntersectRange {
  std::shared_ptr<const FBBezierCurve> _curve1;               // input
  FBRange _parameterRange1;                                   // input
  mutable std::shared_ptr<FBBezierCurve> _curve1LeftBezier;   // cache
  mutable std::shared_ptr<FBBezierCurve> _curve1MiddleBezier; // cache
  mutable std::shared_ptr<FBBezierCurve> _curve1RightBezier;  // cache
  mutable bool _needToComputeCurve1 = true;

  std::shared_ptr<const FBBezierCurve> _curve2;               // input
  FBRange _parameterRange2;                                   // input
  bool _reversed;                                             // input
  mutable std::shared_ptr<FBBezierCurve> _curve2LeftBezier;   // cache
  mutable std::shared_ptr<FBBezierCurve> _curve2MiddleBezier; // cache
  mutable std::shared_ptr<FBBezierCurve> _curve2RightBezier;  // cache
  mutable bool _needToComputeCurve2 = true;

protected:
  void clearCache() const;
  void computeCurve1() const;
  void computeCurve2() const;

public:
  FBBezierIntersectRange(std::shared_ptr<const FBBezierCurve> curve1, FBRange parameterRange1,
                         std::shared_ptr<const FBBezierCurve> curve2, FBRange parameterRange2, bool reversed);

  std::shared_ptr<const FBBezierCurve> curve1() const { return _curve1; }
  FBRange parameterRange1() const { return _parameterRange1; }
  std::shared_ptr<FBBezierCurve> curve1LeftBezier() const;
  std::shared_ptr<FBBezierCurve> curve1RightBezier() const;
  std::shared_ptr<FBBezierCurve> curve1OverlappingBezier() const;

  std::shared_ptr<const FBBezierCurve> curve2() const { return _curve2; }
  FBRange parameterRange2() const { return _parameterRange2; }
  bool reversed() const { return _reversed; }
  std::shared_ptr<FBBezierCurve> curve2LeftBezier() const;
  std::shared_ptr<FBBezierCurve> curve2RightBezier() const;
  std::shared_ptr<FBBezierCurve> curve2OverlappingBezier() const;

  std::shared_ptr<FBBezierIntersection> middleIntersection() const;

  bool isAtStartOfCurve1() const;
  bool isAtStopOfCurve1() const;
  bool isAtStartOfCurve2() const;
  bool isAtStopOfCurve2() const;

  void merge(const std::shared_ptr<FBBezierIntersectRange> &other);
};

} // namespace fb