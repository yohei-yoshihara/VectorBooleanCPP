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

#include "doctest.h"
#include "utils.hpp"
#include "vectorboolean/VectorBoolean.hpp"

using namespace fb;

TEST_CASE("touched rectangles union") {
  FBBezierPath path1;
  addRectangle(path1, {{0., 0.}, {100., 100.}});

  FBBezierPath path2;
  addRectangle(path2, {{100., 0.}, {100., 100.}});

  auto result = path1.unionWithPath(path2);

  CHECK_EQ(result.size(), 12);
  auto element0 = result[0];
  CHECK_EQ(element0.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element0.points[0].x - 0.000000), 1e-3);
  CHECK_LT(std::abs(element0.points[0].y - 0.000000), 1e-3);
  auto element1 = result[1];
  CHECK_EQ(element1.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element1.points[0].x - 100.000000), 1e-3);
  CHECK_LT(std::abs(element1.points[0].y - 0.000000), 1e-3);
  auto element2 = result[2];
  CHECK_EQ(element2.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element2.points[0].x - 100.000000), 1e-3);
  CHECK_LT(std::abs(element2.points[0].y - 100.000000), 1e-3);
  auto element3 = result[3];
  CHECK_EQ(element3.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element3.points[0].x - 0.000000), 1e-3);
  CHECK_LT(std::abs(element3.points[0].y - 100.000000), 1e-3);
  auto element4 = result[4];
  CHECK_EQ(element4.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element4.points[0].x - 0.000000), 1e-3);
  CHECK_LT(std::abs(element4.points[0].y - 0.000000), 1e-3);
  auto element5 = result[5];
  CHECK_EQ(element5.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element5.points[0].x - 0.000000), 1e-3);
  CHECK_LT(std::abs(element5.points[0].y - 0.000000), 1e-3);
  auto element6 = result[6];
  CHECK_EQ(element6.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element6.points[0].x - 100.000000), 1e-3);
  CHECK_LT(std::abs(element6.points[0].y - 0.000000), 1e-3);
  auto element7 = result[7];
  CHECK_EQ(element7.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element7.points[0].x - 200.000000), 1e-3);
  CHECK_LT(std::abs(element7.points[0].y - 0.000000), 1e-3);
  auto element8 = result[8];
  CHECK_EQ(element8.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element8.points[0].x - 200.000000), 1e-3);
  CHECK_LT(std::abs(element8.points[0].y - 100.000000), 1e-3);
  auto element9 = result[9];
  CHECK_EQ(element9.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element9.points[0].x - 100.000000), 1e-3);
  CHECK_LT(std::abs(element9.points[0].y - 100.000000), 1e-3);
  auto element10 = result[10];
  CHECK_EQ(element10.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element10.points[0].x - 100.000000), 1e-3);
  CHECK_LT(std::abs(element10.points[0].y - 0.000000), 1e-3);
  auto element11 = result[11];
  CHECK_EQ(element11.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element11.points[0].x - 100.000000), 1e-3);
  CHECK_LT(std::abs(element11.points[0].y - 0.000000), 1e-3);
}

TEST_CASE("touched rectangles intersect") {
  FBBezierPath path1;
  addRectangle(path1, {{0., 0.}, {100., 100.}});

  FBBezierPath path2;
  addRectangle(path2, {{100., 0.}, {100., 100.}});

  auto result = path1.intersectWithPath(path2);
  CHECK_EQ(result.size(), 0);
}

TEST_CASE("touched rectangles difference") {
  FBBezierPath path1;
  addRectangle(path1, {{0., 0.}, {100., 100.}});

  FBBezierPath path2;
  addRectangle(path2, {{100., 0.}, {100., 100.}});

  auto result = path1.differenceWithPath(path2);

  CHECK_EQ(result.size(), 6);
  auto element0 = result[0];
  CHECK_EQ(element0.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element0.points[0].x - 0.000000), 1e-3);
  CHECK_LT(std::abs(element0.points[0].y - 0.000000), 1e-3);
  auto element1 = result[1];
  CHECK_EQ(element1.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element1.points[0].x - 100.000000), 1e-3);
  CHECK_LT(std::abs(element1.points[0].y - 0.000000), 1e-3);
  auto element2 = result[2];
  CHECK_EQ(element2.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element2.points[0].x - 100.000000), 1e-3);
  CHECK_LT(std::abs(element2.points[0].y - 100.000000), 1e-3);
  auto element3 = result[3];
  CHECK_EQ(element3.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element3.points[0].x - 0.000000), 1e-3);
  CHECK_LT(std::abs(element3.points[0].y - 100.000000), 1e-3);
  auto element4 = result[4];
  CHECK_EQ(element4.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element4.points[0].x - 0.000000), 1e-3);
  CHECK_LT(std::abs(element4.points[0].y - 0.000000), 1e-3);
  auto element5 = result[5];
  CHECK_EQ(element5.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element5.points[0].x - 0.000000), 1e-3);
  CHECK_LT(std::abs(element5.points[0].y - 0.000000), 1e-3);
}

TEST_CASE("touched rectangles xor") {
  FBBezierPath path1;
  addRectangle(path1, {{0., 0.}, {100., 100.}});

  FBBezierPath path2;
  addRectangle(path2, {{100., 0.}, {100., 100.}});

  auto result = path1.xorWithPath(path2);

  CHECK_EQ(result.size(), 12);
  auto element0 = result[0];
  CHECK_EQ(element0.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element0.points[0].x - 0.000000), 1e-3);
  CHECK_LT(std::abs(element0.points[0].y - 0.000000), 1e-3);
  auto element1 = result[1];
  CHECK_EQ(element1.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element1.points[0].x - 100.000000), 1e-3);
  CHECK_LT(std::abs(element1.points[0].y - 0.000000), 1e-3);
  auto element2 = result[2];
  CHECK_EQ(element2.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element2.points[0].x - 100.000000), 1e-3);
  CHECK_LT(std::abs(element2.points[0].y - 100.000000), 1e-3);
  auto element3 = result[3];
  CHECK_EQ(element3.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element3.points[0].x - 0.000000), 1e-3);
  CHECK_LT(std::abs(element3.points[0].y - 100.000000), 1e-3);
  auto element4 = result[4];
  CHECK_EQ(element4.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element4.points[0].x - 0.000000), 1e-3);
  CHECK_LT(std::abs(element4.points[0].y - 0.000000), 1e-3);
  auto element5 = result[5];
  CHECK_EQ(element5.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element5.points[0].x - 0.000000), 1e-3);
  CHECK_LT(std::abs(element5.points[0].y - 0.000000), 1e-3);
  auto element6 = result[6];
  CHECK_EQ(element6.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element6.points[0].x - 100.000000), 1e-3);
  CHECK_LT(std::abs(element6.points[0].y - 0.000000), 1e-3);
  auto element7 = result[7];
  CHECK_EQ(element7.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element7.points[0].x - 200.000000), 1e-3);
  CHECK_LT(std::abs(element7.points[0].y - 0.000000), 1e-3);
  auto element8 = result[8];
  CHECK_EQ(element8.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element8.points[0].x - 200.000000), 1e-3);
  CHECK_LT(std::abs(element8.points[0].y - 100.000000), 1e-3);
  auto element9 = result[9];
  CHECK_EQ(element9.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element9.points[0].x - 100.000000), 1e-3);
  CHECK_LT(std::abs(element9.points[0].y - 100.000000), 1e-3);
  auto element10 = result[10];
  CHECK_EQ(element10.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element10.points[0].x - 100.000000), 1e-3);
  CHECK_LT(std::abs(element10.points[0].y - 0.000000), 1e-3);
  auto element11 = result[11];
  CHECK_EQ(element11.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element11.points[0].x - 100.000000), 1e-3);
  CHECK_LT(std::abs(element11.points[0].y - 0.000000), 1e-3);
}