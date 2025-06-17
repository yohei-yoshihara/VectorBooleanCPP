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

namespace fb {

class FBBezierCurve;
class FBBezierContour;
class FBBezierGraph;

class FBCurveLocation {
  std::shared_ptr<FBBezierGraph> _graph;
  std::shared_ptr<FBBezierContour> _contour;
  std::shared_ptr<FBBezierCurve> _edge;
  FBFloat _parameter;
  FBFloat _distance;

public:
  FBCurveLocation(std::shared_ptr<FBBezierCurve> edge, FBFloat parameter, FBFloat distance)
      : _edge(edge)
      , _parameter(parameter)
      , _distance(distance) {}

  std::shared_ptr<FBBezierGraph> graph() const { return _graph; }
  void setGraph(std::shared_ptr<FBBezierGraph> graph) { _graph = graph; }
  std::shared_ptr<FBBezierContour> contour() const { return _contour; }
  void setContour(std::shared_ptr<FBBezierContour> contour) { _contour = contour; }
  std::shared_ptr<FBBezierCurve> edge() const { return _edge; }
  FBFloat parameter() const { return _parameter; }
  FBFloat distance() const { return _distance; }
};
} // namespace fb