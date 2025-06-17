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

#include "FBBezierPath.hpp"
#include "FBBezierGraph.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <format>
#include <fstream>
#include <sstream>
#include <vector>

namespace fb {

static const FBFloat c = (4.0 / 3.0 * (std::sqrt(2) - 1.0));

static std::array<FBPoint, 4> qarc(const FBPoint &center, FBFloat radiusX, FBFloat radiusY) {
  std::array<FBPoint, 4> points;
  points[0] = {center.x + 0.0 * radiusX, center.y + 1.0 * radiusY};
  points[1] = {center.x + c * radiusX, center.y + 1.0 * radiusY};
  points[2] = {center.x + 1.0 * radiusX, center.y + c * radiusY};
  points[3] = {center.x + 1.0 * radiusX, center.y + 0.0 * radiusY};
  return points;
}

std::array<FBPoint, 4> transform(const std::array<FBPoint, 4> &src, FBFloat sx, FBFloat sy) {
  std::array<FBPoint, 4> result;
  for (std::size_t i = 0; i < src.size(); ++i) {
    result[i] = {src[i].x * sx, src[i].y * sy};
  }
  return result;
}

FBBezierPath FBBezierPath::circle(const FBPoint &center, FBFloat radius) { return oval(center, radius, radius); }

FBBezierPath FBBezierPath::oval(const FBPoint &center, FBFloat radiusX, FBFloat radiusY) {
  auto p0 = qarc({0.0, 0.0}, radiusX, radiusY);
  auto p1 = transform(p0, 1.0, -1.0);
  auto p2 = transform(p0, -1.0, -1.0);
  auto p3 = transform(p0, -1.0, 1.0);
  FBBezierPath path;
  path.moveTo(center + p0[0]);
  path.curveTo({center + p0[1], center + p0[2], center + p0[3]});
  path.curveTo({center + p1[2], center + p1[1], center + p1[0]});
  path.curveTo({center + p2[1], center + p2[2], center + p2[3]});
  path.curveTo({center + p3[2], center + p3[1], center + p3[0]});
  path.close();
  return path;
}

FBBezierPath FBBezierPath::oval(const FBRect &rect) {
  auto radiusX = rect.size.width / 2.0;
  auto radiusY = rect.size.height / 2.0;
  FBPoint center = {rect.origin.x + radiusX, rect.origin.y + radiusY};
  return oval(center, radiusX, radiusY);
}

FBBezierPath FBBezierPath::rect(const FBRect &rect) { return FBBezierPath{rect}; }

FBBezierPath::FBBezierPath(const FBRect &rect) {
  moveTo(rect.origin);
  lineTo(FBPoint{rect.origin.x + rect.size.width, rect.origin.y});
  lineTo(FBPoint{rect.origin.x + rect.size.width, rect.origin.y + rect.size.height});
  lineTo(FBPoint{rect.origin.x, rect.origin.y + rect.size.height});
  close();
}

void FBBezierPath::moveTo(const FBPoint &endPoint) {
  _elements.push_back(Element{
      .type = Type::move,
      .points = {endPoint, FBPoint{0.0, 0.0}, FBPoint{0.0, 0.0}},
  });
}

void FBBezierPath::lineTo(const FBPoint &endPoint) {
  _elements.push_back(Element{
      .type = Type::line,
      .points = {endPoint, FBPoint{0.0, 0.0}, FBPoint{0.0, 0.0}},
  });
}

void FBBezierPath::curveTo(const FBPoint &endPoint, const FBPoint &controlPoint1, const FBPoint &controlPoint2) {
  _elements.push_back(Element{
      .type = Type::curve,
      .points = {controlPoint1, controlPoint2, endPoint},
  });
}

void FBBezierPath::close() {
  if (_elements.empty()) {
    return;
  }
  auto it = std::find_if(_elements.rbegin(), _elements.rend(), [](const Element &element){
    return element.type == Type::move;
  });
  auto startPoint = it != _elements.rend() ? it->points[0] : _elements[0].points[0];
  _elements.push_back(Element{
      .type = Type::close,
      .points = {startPoint, FBPoint{0.0, 0.0}, FBPoint{0.0, 0.0}},
  });
}

void FBBezierPath::curveTo(const std::array<FBPoint, 3> &points) { curveTo(points[2], points[0], points[1]); }

FBRect FBBezierPath::bounds() const {
  FBFloat minX = 1e10;
  FBFloat minY = 1e10;
  FBFloat maxX = -1e10;
  FBFloat maxY = -1e10;

  for (std::size_t i = 0; i < size(); ++i) {
    const auto &element = operator[](i);
    switch (element.type) {
    case FBBezierPath::Type::move:
    case FBBezierPath::Type::line:
    case FBBezierPath::Type::close: {
      minX = std::min(minX, element.points[0].x);
      minY = std::min(minY, element.points[0].y);
      maxX = std::max(maxX, element.points[0].x);
      maxY = std::max(maxY, element.points[0].y);
    } break;
    case FBBezierPath::Type::curve: {
      for (std::size_t i = 0; i < element.points.size(); ++i) {
        minX = std::min(minX, element.points[i].x);
        minY = std::min(minY, element.points[i].y);
        maxX = std::max(maxX, element.points[i].x);
        maxY = std::max(maxY, element.points[i].y);
      }
    } break;
    }
  }
  return FBRect{{minX, minY}, {maxX - minX, maxY - minY}};
}

std::string FBBezierPath::toSVGPath() const {
  std::stringstream ss;
  for (std::size_t i = 0; i < size(); ++i) {
    const auto &element = operator[](i);
    switch (element.type) {
    case FBBezierPath::Type::move: {
      ss << "M " << element.points[0].x << " " << element.points[0].y;
    } break;
    case FBBezierPath::Type::line: {
      ss << "L " << element.points[0].x << " " << element.points[0].y;
    } break;
    case FBBezierPath::Type::curve: {
      ss << "C " << element.points[0].x << " " << element.points[0].y << " " << element.points[1].x << " "
         << element.points[1].y << " " << element.points[2].x << " " << element.points[2].y;
    } break;
    case FBBezierPath::Type::close: {
      ss << "Z";
    } break;
    }
    if (i != size() - 1) {
      ss << ' ';
    }
  }
  return ss.str();
}

std::string FBBezierPath::toSVG() const {
  std::stringstream ss;
  auto bounds = this->bounds();
  ss << "<svg viewBox=\"" << bounds.origin.x << ", " << bounds.origin.y << ", " << bounds.size.width << ", "
     << bounds.size.height << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";
  ss << "  <path d=\"";
  ss << toSVGPath();
  ss << "\"/>\n";
  ss << "</svg>";
  return ss.str();
}

void FBBezierPath::writeSVG(const std::string &filename) const {
  std::ofstream of(filename, std::ios::out);
  of << toSVG() << std::endl;
  of.close();
}

FBBezierPath FBBezierPath::unionWithPath(const FBBezierPath &path) const {
  auto graph1 = std::make_shared<fb::FBBezierGraph>(*this);
  auto graph2 = std::make_shared<fb::FBBezierGraph>(path);
  return graph1->unionWithBezierGraph(graph2)->bezierPath();
}

FBBezierPath FBBezierPath::intersectWithPath(const FBBezierPath &path) const {
  auto graph1 = std::make_shared<fb::FBBezierGraph>(*this);
  auto graph2 = std::make_shared<fb::FBBezierGraph>(path);
  return graph1->intersectWithBezierGraph(graph2)->bezierPath();
}

FBBezierPath FBBezierPath::differenceWithPath(const FBBezierPath &path) const {
  auto graph1 = std::make_shared<fb::FBBezierGraph>(*this);
  auto graph2 = std::make_shared<fb::FBBezierGraph>(path);
  return graph1->differenceWithBezierGraph(graph2)->bezierPath();
}

FBBezierPath FBBezierPath::xorWithPath(const FBBezierPath &path) const {
  auto graph1 = std::make_shared<fb::FBBezierGraph>(*this);
  auto graph2 = std::make_shared<fb::FBBezierGraph>(path);
  return graph1->xorWithBezierGraph(graph2)->bezierPath();
}

std::string FBBezierPath::str(int indent) const {
  std::ostringstream ss;
  const auto &path = *this;

  ss << std::format("{}<FBBezierPath path=[", fb::indent(indent));
  if (indent >= 0) {
    ss << '\n';
  }
  for (std::size_t i = 0; i < path.size(); ++i) {
    const auto &element = path[i];
    switch (element.type) {
    case FBBezierPath::Type::move: {
      ss << std::format("{}move({}, {})", fb::indent(indent + 2), element.points[0].x, element.points[0].y);
    } break;
    case FBBezierPath::Type::line: {
      ss << std::format("{}line({}, {})", fb::indent(indent + 2), element.points[0].x, element.points[0].y);
    } break;
    case FBBezierPath::Type::curve: {
      ss << std::format("{}curve(({}, {}), ({}, {}), ({}, {}))", fb::indent(indent + 2), element.points[2].x, element.points[2].y,
                        element.points[0].x, element.points[0].y, element.points[1].x, element.points[1].y);
    } break;
    case FBBezierPath::Type::close: {
      ss << std::format("{}close({}, {})", fb::indent(indent + 2), element.points[0].x, element.points[0].y);
    } break;
    }
    if (i != path.size() - 1) {
      ss << ((indent >= 0) ? ",\n" : ", ");
    }
  }
  if (indent >= 0) {
    ss << '\n';
  }
  ss << std::format("{}]>", fb::indent(indent));

  return ss.str();
}

std::ostream &operator<<(std::ostream &os, const FBBezierPath &path) {
  os << path.str();
  return os;
}

} // namespace fb