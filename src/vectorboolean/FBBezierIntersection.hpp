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

namespace fb {

extern const FBFloat FBParameterCloseThreshold;

class FBBezierCurve;

class FBBezierIntersection {
  std::shared_ptr<const FBBezierCurve> _curve1; // input
  FBFloat _parameter1;                          // input
  std::shared_ptr<const FBBezierCurve> _curve2; // input
  FBFloat _parameter2;                          // input

  mutable FBPoint _location = {0, 0};
  mutable std::shared_ptr<FBBezierCurve> _curve1LeftBezier = nullptr;
  mutable std::shared_ptr<FBBezierCurve> _curve1RightBezier = nullptr;
  mutable std::shared_ptr<FBBezierCurve> _curve2LeftBezier = nullptr;
  mutable std::shared_ptr<FBBezierCurve> _curve2RightBezier = nullptr;
  mutable bool _needToComputeCurve1 = true;
  mutable bool _needToComputeCurve2 = true;

protected:
  void computeCurve1() const;
  void computeCurve2() const;

public:
  FBBezierIntersection(std::shared_ptr<const FBBezierCurve> curve1, FBFloat parameter1,
                       std::shared_ptr<const FBBezierCurve> curve2, FBFloat parameter2)
      : _curve1(curve1)
      , _parameter1(parameter1)
      , _curve2(curve2)
      , _parameter2(parameter2)
      , _needToComputeCurve1(true)
      , _needToComputeCurve2(true) {}
  std::shared_ptr<const FBBezierCurve> curve1() const { return _curve1; }
  FBFloat parameter1() const { return _parameter1; }
  std::shared_ptr<const FBBezierCurve> curve2() const { return _curve2; }
  FBFloat parameter2() const { return _parameter2; }

  FBPoint location() const {
    computeCurve1();
    return _location;
  }
  bool isTangent() const;
  std::shared_ptr<FBBezierCurve> curve1LeftBezier() const {
    computeCurve1();
    return _curve1LeftBezier;
  }
  std::shared_ptr<FBBezierCurve> curve1RightBezier() const {
    computeCurve1();
    return _curve1RightBezier;
  }
  std::shared_ptr<FBBezierCurve> curve2LeftBezier() const {
    computeCurve2();
    return _curve2LeftBezier;
  }
  std::shared_ptr<FBBezierCurve> curve2RightBezier() const {
    computeCurve2();
    return _curve2RightBezier;
  }

  bool isAtStartOfCurve1() const;
  bool isAtStopOfCurve1() const;
  bool isAtStartOfCurve2() const;
  bool isAtStopOfCurve2() const;

  bool isAtEndPointOfCurve1() const;
  bool isAtEndPointOfCurve2() const;
  bool isAtEndPointOfCurve() const;
};
} // namespace fb