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

#include <iosfwd>
#include <sstream>
#include <string>
#include <format>
#include <functional>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>

namespace fb {

using FBFloat = double;

struct FBPoint {
  FBFloat x = 0.0;
  FBFloat y = 0.0;
};
FBPoint operator+(const FBPoint &p1, const FBPoint &p2);
std::ostream& operator<<(std::ostream& os, const FBPoint& point);
extern const FBPoint FBZeroPoint;

struct FBSize {
  FBFloat width;
  FBFloat height;
};
std::ostream& operator<<(std::ostream& os, const FBSize& point);

struct FBRect {
  FBPoint origin{0.0, 0.0};
  FBSize size{0.0, 0.0};
};
std::ostream& operator<<(std::ostream& os, const FBRect& point);

extern const FBRect FBZeroRect;

inline FBPoint FBMakePoint(FBFloat x, FBFloat y) { return FBPoint(x, y); }

inline bool FBEqualPoints(const FBPoint &p1, const FBPoint &p2) { return p1.x == p2.x && p1.y == p2.y; }

inline bool FBEqualSizes(const FBSize &s1, const FBSize &s2) { return s1.width == s2.width && s1.height == s2.height; }

inline FBRect FBMakeRect(FBFloat x, FBFloat y, FBFloat width, FBFloat height) {
  return FBRect{.origin = FBPoint(x, y), .size = FBSize{.width = width, .height = height}};
}

inline bool FBEqualRects(const FBRect &rect1, const FBRect &rect2) {
  return FBEqualPoints(rect1.origin, rect2.origin) && FBEqualSizes(rect1.size, rect2.size);
}

inline FBFloat FBMinX(const FBRect &rect) { return rect.origin.x; }

inline FBFloat FBMaxX(const FBRect &rect) { return rect.origin.x + rect.size.width; }

inline FBFloat FBMinY(const FBRect &rect) { return rect.origin.y; }

inline FBFloat FBMaxY(const FBRect &rect) { return rect.origin.y + rect.size.height; }

inline FBFloat FBWidth(const FBRect &rect) { return rect.size.width; }

inline FBFloat FBHeight(const FBRect &rect) { return rect.size.height; }

inline bool FBPointInRect(const FBPoint &p, const FBRect &rect) {
  return rect.origin.x <= p.x && p.x < (rect.origin.x + rect.size.width) && rect.origin.y <= p.y &&
         p.y < (rect.origin.y + rect.size.height);
}

inline bool FBIsEmptyRect(FBRect rect) {
    return (rect.size.width <= 0.0 || rect.size.height <= 0.0);
}

template<typename T>
std::string arrayFormatter(const std::vector<std::shared_ptr<T>> &array) {
  std::ostringstream ss;
  ss << '[';
  for (std::size_t i = 0; i < array.size(); i++) {
    ss << std::format("{}", *array[i]);
    if (i == array.size() - 1) {
      ss << ", ";
    }
  }
  ss << ']';
  return ss.str();
}

inline constexpr std::string indent(int indent) {
  return indent > 0 ? std::string(indent, ' ') : std::string();
}

} // namespace fb

template<>
struct std::formatter<fb::FBPoint> : std::formatter<std::string> {
  auto format(fb::FBPoint p, std::format_context& ctx) const {
    return std::formatter<std::string>::format(
      std::format("{{{}, {}}}", p.x, p.y),
      ctx);
  }
};

template<>
struct std::formatter<fb::FBSize> : std::formatter<std::string> {
  auto format(fb::FBSize size, std::format_context& ctx) const {
    return std::formatter<std::string>::format(
      std::format("{{{}, {}}}", size.width, size.height),
      ctx);
  }
};

template<>
struct std::formatter<fb::FBRect> : std::formatter<std::string> {
  auto format(fb::FBRect rect, std::format_context& ctx) const {
    return std::formatter<std::string>::format(
      std::format("{{{}, {}}}", rect.origin, rect.size),
      ctx);
  }
};


