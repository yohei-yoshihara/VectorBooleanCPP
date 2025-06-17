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

#include <iostream>
#include <array>
#include <vector>

namespace fb {

class FBBezierPath {
public:
  enum class Type { move, line, curve, close };
  struct Element {
    Type type;
    std::array<FBPoint, 3> points;
  };

private:
  std::vector<Element> _elements;

public:
  static FBBezierPath circle(const FBPoint &center, FBFloat radius);
  static FBBezierPath oval(const FBPoint &center, FBFloat radiusX, FBFloat radiusY);
  static FBBezierPath oval(const FBRect &rect);
  static FBBezierPath rect(const FBRect &rect);

  FBBezierPath() = default;
  FBBezierPath(const FBRect &rect);
  void moveTo(const FBPoint &endPoint);
  void lineTo(const FBPoint &endPoint);
  void curveTo(const FBPoint &endPoint, const FBPoint &controlPoint1, const FBPoint &controlPoint2);
  void curveTo(const std::array<FBPoint, 3> &points);
  void close();
  std::size_t size() const { return _elements.size(); }
  const Element &operator[](std::size_t i) const { return _elements[i]; }

  FBRect bounds() const;
  std::string toSVGPath() const;
  std::string toSVG() const;
  void writeSVG(const std::string &filename) const;

  FBBezierPath unionWithPath(const FBBezierPath &path) const;
  FBBezierPath intersectWithPath(const FBBezierPath &path) const;
  FBBezierPath differenceWithPath(const FBBezierPath &path) const;
  FBBezierPath xorWithPath(const FBBezierPath &path) const;

  std::string str(int indent = -1) const;
};

std::ostream& operator<<(std::ostream& os, const FBBezierPath& path);

} // namespace fb