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

TEST_CASE("two arc shapes union") {
  FBBezierPath path1;
  addArcShape(path1, {{25.f, 0.f}, {50.f, 100.f}});

  FBBezierPath path2;
  addArcShape(path2, {{0.f, 25.f}, {100.f, 50.f}});

  auto result = path1.unionWithPath(path2);

  CHECK_EQ(result.size(), 12);
  auto element0 = result[0];
  CHECK_EQ(element0.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element0.points[0].x - 72.112877), 1e-3);
  CHECK_LT(std::abs(element0.points[0].y - 25.000000), 1e-3);
  auto element1 = result[1];
  CHECK_EQ(element1.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element1.points[0].x - 73.938392), 1e-3);
  CHECK_LT(std::abs(element1.points[0].y - 16.825440), 1e-3);
  CHECK_LT(std::abs(element1.points[1].x - 75.000000), 1e-3);
  CHECK_LT(std::abs(element1.points[1].y - 8.412720), 1e-3);
  CHECK_LT(std::abs(element1.points[2].x - 75.000000), 1e-3);
  CHECK_LT(std::abs(element1.points[2].y - 0.000000), 1e-3);
  auto element2 = result[2];
  CHECK_EQ(element2.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element2.points[0].x - 25.000000), 1e-3);
  CHECK_LT(std::abs(element2.points[0].y - 0.000000), 1e-3);
  auto element3 = result[3];
  CHECK_EQ(element3.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element3.points[0].x - 25.000000), 1e-3);
  CHECK_LT(std::abs(element3.points[0].y - 25.000000), 1e-3);
  auto element4 = result[4];
  CHECK_EQ(element4.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element4.points[0].x - 0.000000), 1e-3);
  CHECK_LT(std::abs(element4.points[0].y - 25.000000), 1e-3);
  auto element5 = result[5];
  CHECK_EQ(element5.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element5.points[0].x - 0.000000), 1e-3);
  CHECK_LT(std::abs(element5.points[0].y - 75.000000), 1e-3);
  auto element6 = result[6];
  CHECK_EQ(element6.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element6.points[0].x - 8.412720), 1e-3);
  CHECK_LT(std::abs(element6.points[0].y - 75.000000), 1e-3);
  CHECK_LT(std::abs(element6.points[1].x - 16.825440), 1e-3);
  CHECK_LT(std::abs(element6.points[1].y - 73.938392), 1e-3);
  CHECK_LT(std::abs(element6.points[2].x - 25.000000), 1e-3);
  CHECK_LT(std::abs(element6.points[2].y - 72.112877), 1e-3);
  auto element7 = result[7];
  CHECK_EQ(element7.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element7.points[0].x - 25.000000), 1e-3);
  CHECK_LT(std::abs(element7.points[0].y - 100.000000), 1e-3);
  auto element8 = result[8];
  CHECK_EQ(element8.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element8.points[0].x - 32.216878), 1e-3);
  CHECK_LT(std::abs(element8.points[0].y - 100.000000), 1e-3);
  CHECK_LT(std::abs(element8.points[1].x - 47.767090), 1e-3);
  CHECK_LT(std::abs(element8.points[1].y - 83.333333), 1e-3);
  CHECK_LT(std::abs(element8.points[2].x - 59.622504), 1e-3);
  CHECK_LT(std::abs(element8.points[2].y - 59.622504), 1e-3);
  auto element9 = result[9];
  CHECK_EQ(element9.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element9.points[0].x - 83.333335), 1e-3);
  CHECK_LT(std::abs(element9.points[0].y - 47.767089), 1e-3);
  CHECK_LT(std::abs(element9.points[1].x - 100.000000), 1e-3);
  CHECK_LT(std::abs(element9.points[1].y - 32.216878), 1e-3);
  CHECK_LT(std::abs(element9.points[2].x - 100.000000), 1e-3);
  CHECK_LT(std::abs(element9.points[2].y - 25.000000), 1e-3);
  auto element10 = result[10];
  CHECK_EQ(element10.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element10.points[0].x - 72.112877), 1e-3);
  CHECK_LT(std::abs(element10.points[0].y - 25.000000), 1e-3);
  auto element11 = result[11];
  CHECK_EQ(element11.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element11.points[0].x - 72.112877), 1e-3);
  CHECK_LT(std::abs(element11.points[0].y - 25.000000), 1e-3);
}

TEST_CASE("two arc shapes intersect") {
  FBBezierPath path1;
  addArcShape(path1, {{25.f, 0.f}, {50.f, 100.f}});

  FBBezierPath path2;
  addArcShape(path2, {{0.f, 25.f}, {100.f, 50.f}});

  auto result = path1.intersectWithPath(path2);

  CHECK_EQ(result.size(), 6);
  auto element0 = result[0];
  CHECK_EQ(element0.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element0.points[0].x - 72.112877), 1e-3);
  CHECK_LT(std::abs(element0.points[0].y - 25.000000), 1e-3);
  auto element1 = result[1];
  CHECK_EQ(element1.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element1.points[0].x - 69.352755), 1e-3);
  CHECK_LT(std::abs(element1.points[0].y - 37.359676), 1e-3);
  CHECK_LT(std::abs(element1.points[1].x - 64.846304), 1e-3);
  CHECK_LT(std::abs(element1.points[1].y - 49.174905), 1e-3);
  CHECK_LT(std::abs(element1.points[2].x - 59.622504), 1e-3);
  CHECK_LT(std::abs(element1.points[2].y - 59.622504), 1e-3);
  auto element2 = result[2];
  CHECK_EQ(element2.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element2.points[0].x - 49.174908), 1e-3);
  CHECK_LT(std::abs(element2.points[0].y - 64.846303), 1e-3);
  CHECK_LT(std::abs(element2.points[1].x - 37.359677), 1e-3);
  CHECK_LT(std::abs(element2.points[1].y - 69.352755), 1e-3);
  CHECK_LT(std::abs(element2.points[2].x - 25.000000), 1e-3);
  CHECK_LT(std::abs(element2.points[2].y - 72.112877), 1e-3);
  auto element3 = result[3];
  CHECK_EQ(element3.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element3.points[0].x - 25.000000), 1e-3);
  CHECK_LT(std::abs(element3.points[0].y - 25.000000), 1e-3);
  auto element4 = result[4];
  CHECK_EQ(element4.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element4.points[0].x - 72.112877), 1e-3);
  CHECK_LT(std::abs(element4.points[0].y - 25.000000), 1e-3);
  auto element5 = result[5];
  CHECK_EQ(element5.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element5.points[0].x - 72.112877), 1e-3);
  CHECK_LT(std::abs(element5.points[0].y - 25.000000), 1e-3);
}

TEST_CASE("two arc shapes difference") {
  FBBezierPath path1;
  addArcShape(path1, {{25.f, 0.f}, {50.f, 100.f}});

  FBBezierPath path2;
  addArcShape(path2, {{0.f, 25.f}, {100.f, 50.f}});

  auto result = path1.differenceWithPath(path2);

  CHECK_EQ(result.size(), 11);
  auto element0 = result[0];
  CHECK_EQ(element0.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element0.points[0].x - 72.112877), 1e-3);
  CHECK_LT(std::abs(element0.points[0].y - 25.000000), 1e-3);
  auto element1 = result[1];
  CHECK_EQ(element1.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element1.points[0].x - 73.938392), 1e-3);
  CHECK_LT(std::abs(element1.points[0].y - 16.825440), 1e-3);
  CHECK_LT(std::abs(element1.points[1].x - 75.000000), 1e-3);
  CHECK_LT(std::abs(element1.points[1].y - 8.412720), 1e-3);
  CHECK_LT(std::abs(element1.points[2].x - 75.000000), 1e-3);
  CHECK_LT(std::abs(element1.points[2].y - 0.000000), 1e-3);
  auto element2 = result[2];
  CHECK_EQ(element2.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element2.points[0].x - 25.000000), 1e-3);
  CHECK_LT(std::abs(element2.points[0].y - 0.000000), 1e-3);
  auto element3 = result[3];
  CHECK_EQ(element3.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element3.points[0].x - 25.000000), 1e-3);
  CHECK_LT(std::abs(element3.points[0].y - 25.000000), 1e-3);
  auto element4 = result[4];
  CHECK_EQ(element4.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element4.points[0].x - 72.112877), 1e-3);
  CHECK_LT(std::abs(element4.points[0].y - 25.000000), 1e-3);
  auto element5 = result[5];
  CHECK_EQ(element5.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element5.points[0].x - 72.112877), 1e-3);
  CHECK_LT(std::abs(element5.points[0].y - 25.000000), 1e-3);
  auto element6 = result[6];
  CHECK_EQ(element6.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element6.points[0].x - 59.622504), 1e-3);
  CHECK_LT(std::abs(element6.points[0].y - 59.622504), 1e-3);
  auto element7 = result[7];
  CHECK_EQ(element7.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element7.points[0].x - 47.767090), 1e-3);
  CHECK_LT(std::abs(element7.points[0].y - 83.333333), 1e-3);
  CHECK_LT(std::abs(element7.points[1].x - 32.216878), 1e-3);
  CHECK_LT(std::abs(element7.points[1].y - 100.000000), 1e-3);
  CHECK_LT(std::abs(element7.points[2].x - 25.000000), 1e-3);
  CHECK_LT(std::abs(element7.points[2].y - 100.000000), 1e-3);
  auto element8 = result[8];
  CHECK_EQ(element8.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element8.points[0].x - 25.000000), 1e-3);
  CHECK_LT(std::abs(element8.points[0].y - 72.112877), 1e-3);
  auto element9 = result[9];
  CHECK_EQ(element9.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element9.points[0].x - 37.359677), 1e-3);
  CHECK_LT(std::abs(element9.points[0].y - 69.352755), 1e-3);
  CHECK_LT(std::abs(element9.points[1].x - 49.174908), 1e-3);
  CHECK_LT(std::abs(element9.points[1].y - 64.846303), 1e-3);
  CHECK_LT(std::abs(element9.points[2].x - 59.622508), 1e-3);
  CHECK_LT(std::abs(element9.points[2].y - 59.622503), 1e-3);
  auto element10 = result[10];
  CHECK_EQ(element10.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element10.points[0].x - 59.622504), 1e-3);
  CHECK_LT(std::abs(element10.points[0].y - 59.622504), 1e-3);
}

TEST_CASE("two arc shapes xor") {
  FBBezierPath path1;
  addArcShape(path1, {{25.f, 0.f}, {50.f, 100.f}});

  FBBezierPath path2;
  addArcShape(path2, {{0.f, 25.f}, {100.f, 50.f}});

  auto result = path1.xorWithPath(path2);

  CHECK_EQ(result.size(), 18);
  auto element0 = result[0];
  CHECK_EQ(element0.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element0.points[0].x - 72.112877), 1e-3);
  CHECK_LT(std::abs(element0.points[0].y - 25.000000), 1e-3);
  auto element1 = result[1];
  CHECK_EQ(element1.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element1.points[0].x - 73.938392), 1e-3);
  CHECK_LT(std::abs(element1.points[0].y - 16.825440), 1e-3);
  CHECK_LT(std::abs(element1.points[1].x - 75.000000), 1e-3);
  CHECK_LT(std::abs(element1.points[1].y - 8.412720), 1e-3);
  CHECK_LT(std::abs(element1.points[2].x - 75.000000), 1e-3);
  CHECK_LT(std::abs(element1.points[2].y - 0.000000), 1e-3);
  auto element2 = result[2];
  CHECK_EQ(element2.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element2.points[0].x - 25.000000), 1e-3);
  CHECK_LT(std::abs(element2.points[0].y - 0.000000), 1e-3);
  auto element3 = result[3];
  CHECK_EQ(element3.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element3.points[0].x - 25.000000), 1e-3);
  CHECK_LT(std::abs(element3.points[0].y - 25.000000), 1e-3);
  auto element4 = result[4];
  CHECK_EQ(element4.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element4.points[0].x - 0.000000), 1e-3);
  CHECK_LT(std::abs(element4.points[0].y - 25.000000), 1e-3);
  auto element5 = result[5];
  CHECK_EQ(element5.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element5.points[0].x - 0.000000), 1e-3);
  CHECK_LT(std::abs(element5.points[0].y - 75.000000), 1e-3);
  auto element6 = result[6];
  CHECK_EQ(element6.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element6.points[0].x - 8.412720), 1e-3);
  CHECK_LT(std::abs(element6.points[0].y - 75.000000), 1e-3);
  CHECK_LT(std::abs(element6.points[1].x - 16.825440), 1e-3);
  CHECK_LT(std::abs(element6.points[1].y - 73.938392), 1e-3);
  CHECK_LT(std::abs(element6.points[2].x - 25.000000), 1e-3);
  CHECK_LT(std::abs(element6.points[2].y - 72.112877), 1e-3);
  auto element7 = result[7];
  CHECK_EQ(element7.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element7.points[0].x - 25.000000), 1e-3);
  CHECK_LT(std::abs(element7.points[0].y - 100.000000), 1e-3);
  auto element8 = result[8];
  CHECK_EQ(element8.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element8.points[0].x - 32.216878), 1e-3);
  CHECK_LT(std::abs(element8.points[0].y - 100.000000), 1e-3);
  CHECK_LT(std::abs(element8.points[1].x - 47.767090), 1e-3);
  CHECK_LT(std::abs(element8.points[1].y - 83.333333), 1e-3);
  CHECK_LT(std::abs(element8.points[2].x - 59.622504), 1e-3);
  CHECK_LT(std::abs(element8.points[2].y - 59.622504), 1e-3);
  auto element9 = result[9];
  CHECK_EQ(element9.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element9.points[0].x - 83.333335), 1e-3);
  CHECK_LT(std::abs(element9.points[0].y - 47.767089), 1e-3);
  CHECK_LT(std::abs(element9.points[1].x - 100.000000), 1e-3);
  CHECK_LT(std::abs(element9.points[1].y - 32.216878), 1e-3);
  CHECK_LT(std::abs(element9.points[2].x - 100.000000), 1e-3);
  CHECK_LT(std::abs(element9.points[2].y - 25.000000), 1e-3);
  auto element10 = result[10];
  CHECK_EQ(element10.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element10.points[0].x - 72.112877), 1e-3);
  CHECK_LT(std::abs(element10.points[0].y - 25.000000), 1e-3);
  auto element11 = result[11];
  CHECK_EQ(element11.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element11.points[0].x - 72.112877), 1e-3);
  CHECK_LT(std::abs(element11.points[0].y - 25.000000), 1e-3);
  auto element12 = result[12];
  CHECK_EQ(element12.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element12.points[0].x - 72.112877), 1e-3);
  CHECK_LT(std::abs(element12.points[0].y - 25.000000), 1e-3);
  auto element13 = result[13];
  CHECK_EQ(element13.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element13.points[0].x - 69.352755), 1e-3);
  CHECK_LT(std::abs(element13.points[0].y - 37.359676), 1e-3);
  CHECK_LT(std::abs(element13.points[1].x - 64.846304), 1e-3);
  CHECK_LT(std::abs(element13.points[1].y - 49.174905), 1e-3);
  CHECK_LT(std::abs(element13.points[2].x - 59.622504), 1e-3);
  CHECK_LT(std::abs(element13.points[2].y - 59.622504), 1e-3);
  auto element14 = result[14];
  CHECK_EQ(element14.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element14.points[0].x - 49.174908), 1e-3);
  CHECK_LT(std::abs(element14.points[0].y - 64.846303), 1e-3);
  CHECK_LT(std::abs(element14.points[1].x - 37.359677), 1e-3);
  CHECK_LT(std::abs(element14.points[1].y - 69.352755), 1e-3);
  CHECK_LT(std::abs(element14.points[2].x - 25.000000), 1e-3);
  CHECK_LT(std::abs(element14.points[2].y - 72.112877), 1e-3);
  auto element15 = result[15];
  CHECK_EQ(element15.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element15.points[0].x - 25.000000), 1e-3);
  CHECK_LT(std::abs(element15.points[0].y - 25.000000), 1e-3);
  auto element16 = result[16];
  CHECK_EQ(element16.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element16.points[0].x - 72.112877), 1e-3);
  CHECK_LT(std::abs(element16.points[0].y - 25.000000), 1e-3);
  auto element17 = result[17];
  CHECK_EQ(element17.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element17.points[0].x - 72.112877), 1e-3);
  CHECK_LT(std::abs(element17.points[0].y - 25.000000), 1e-3);
}