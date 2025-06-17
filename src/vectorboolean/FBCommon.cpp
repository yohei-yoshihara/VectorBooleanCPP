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

#include "FBCommon.hpp"

#include <iostream>

namespace fb {
const FBPoint FBZeroPoint{0.0, 0.0};
const FBRect FBZeroRect{{0.0, 0.0}, {0.0, 0.0}};

FBPoint operator+(const FBPoint &p1, const FBPoint &p2) {
  return {p1.x + p2.x, p1.y + p2.y};
}

std::ostream& operator<<(std::ostream& os, const FBPoint& point) {
  os << '{' << point.x << ", " << point.y << '}';
  return os;
}

std::ostream& operator<<(std::ostream& os, const FBSize& size) {
  os << '{' << size.width << ", " << size.height << '}';
  return os;
}

std::ostream& operator<<(std::ostream& os, const FBRect& rect) {
  os << '{' << rect.origin << ", " << rect.size << '}';
  return os;
}

} // namespace fb
