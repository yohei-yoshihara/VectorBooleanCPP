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

TEST_CASE("complex shapes union") {
  FBBezierPath path1;
  addRectangle(path1, {{50., 50.}, {350., 300.}});
  addCircle(path1, {210., 200.}, 125.);

  FBBezierPath path2;
  addRectangle(path2, {{180., 5.}, {100., 400.}});

  auto result = path1.unionWithPath(path2);

  CHECK_EQ(result.size(), 24);
  auto element0 = result[0];
  CHECK_EQ(element0.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element0.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element0.points[0].y - 50.000000), 1e-3);
  auto element1 = result[1];
  CHECK_EQ(element1.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element1.points[0].x - 50.000000), 1e-3);
  CHECK_LT(std::abs(element1.points[0].y - 50.000000), 1e-3);
  auto element2 = result[2];
  CHECK_EQ(element2.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element2.points[0].x - 50.000000), 1e-3);
  CHECK_LT(std::abs(element2.points[0].y - 350.000000), 1e-3);
  auto element3 = result[3];
  CHECK_EQ(element3.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element3.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element3.points[0].y - 350.000000), 1e-3);
  auto element4 = result[4];
  CHECK_EQ(element4.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element4.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element4.points[0].y - 405.000000), 1e-3);
  auto element5 = result[5];
  CHECK_EQ(element5.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element5.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element5.points[0].y - 405.000000), 1e-3);
  auto element6 = result[6];
  CHECK_EQ(element6.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element6.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element6.points[0].y - 350.000000), 1e-3);
  auto element7 = result[7];
  CHECK_EQ(element7.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element7.points[0].x - 400.000000), 1e-3);
  CHECK_LT(std::abs(element7.points[0].y - 350.000000), 1e-3);
  auto element8 = result[8];
  CHECK_EQ(element8.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element8.points[0].x - 400.000000), 1e-3);
  CHECK_LT(std::abs(element8.points[0].y - 50.000000), 1e-3);
  auto element9 = result[9];
  CHECK_EQ(element9.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element9.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element9.points[0].y - 50.000000), 1e-3);
  auto element10 = result[10];
  CHECK_EQ(element10.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element10.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element10.points[0].y - 5.000000), 1e-3);
  auto element11 = result[11];
  CHECK_EQ(element11.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element11.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element11.points[0].y - 5.000000), 1e-3);
  auto element12 = result[12];
  CHECK_EQ(element12.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element12.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element12.points[0].y - 50.000000), 1e-3);
  auto element13 = result[13];
  CHECK_EQ(element13.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element13.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element13.points[0].y - 50.000000), 1e-3);
  auto element14 = result[14];
  CHECK_EQ(element14.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element14.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element14.points[0].y - 321.376768), 1e-3);
  auto element15 = result[15];
  CHECK_EQ(element15.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element15.points[0].x - 125.453539), 1e-3);
  CHECK_LT(std::abs(element15.points[0].y - 307.940020), 1e-3);
  CHECK_LT(std::abs(element15.points[1].x - 85.000000), 1e-3);
  CHECK_LT(std::abs(element15.points[1].y - 258.694225), 1e-3);
  CHECK_LT(std::abs(element15.points[2].x - 85.000000), 1e-3);
  CHECK_LT(std::abs(element15.points[2].y - 200.000000), 1e-3);
  auto element16 = result[16];
  CHECK_EQ(element16.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element16.points[0].x - 85.000000), 1e-3);
  CHECK_LT(std::abs(element16.points[0].y - 141.305775), 1e-3);
  CHECK_LT(std::abs(element16.points[1].x - 125.453539), 1e-3);
  CHECK_LT(std::abs(element16.points[1].y - 92.059980), 1e-3);
  CHECK_LT(std::abs(element16.points[2].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element16.points[2].y - 78.623232), 1e-3);
  auto element17 = result[17];
  CHECK_EQ(element17.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element17.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element17.points[0].y - 321.376768), 1e-3);
  auto element18 = result[18];
  CHECK_EQ(element18.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element18.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element18.points[0].y - 321.376768), 1e-3);
  auto element19 = result[19];
  CHECK_EQ(element19.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element19.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element19.points[0].y - 303.576676), 1e-3);
  auto element20 = result[20];
  CHECK_EQ(element20.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element20.points[0].x - 313.187685), 1e-3);
  CHECK_LT(std::abs(element20.points[0].y - 281.103620), 1e-3);
  CHECK_LT(std::abs(element20.points[1].x - 335.000000), 1e-3);
  CHECK_LT(std::abs(element20.points[1].y - 243.099080), 1e-3);
  CHECK_LT(std::abs(element20.points[2].x - 335.000000), 1e-3);
  CHECK_LT(std::abs(element20.points[2].y - 200.000000), 1e-3);
  auto element21 = result[21];
  CHECK_EQ(element21.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element21.points[0].x - 335.000000), 1e-3);
  CHECK_LT(std::abs(element21.points[0].y - 156.900920), 1e-3);
  CHECK_LT(std::abs(element21.points[1].x - 313.187685), 1e-3);
  CHECK_LT(std::abs(element21.points[1].y - 118.896380), 1e-3);
  CHECK_LT(std::abs(element21.points[2].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element21.points[2].y - 96.423324), 1e-3);
  auto element22 = result[22];
  CHECK_EQ(element22.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element22.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element22.points[0].y - 303.576676), 1e-3);
  auto element23 = result[23];
  CHECK_EQ(element23.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element23.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element23.points[0].y - 303.576676), 1e-3);
}

TEST_CASE("complex shapes intersect") {
  FBBezierPath path1;
  addRectangle(path1, {{50., 50.}, {350., 300.}});
  addCircle(path1, {210., 200.}, 125.);

  FBBezierPath path2;
  addRectangle(path2, {{180., 5.}, {100., 400.}});

  auto result = path1.intersectWithPath(path2);

  CHECK_EQ(result.size(), 14);
  auto element0 = result[0];
  CHECK_EQ(element0.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element0.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element0.points[0].y - 50.000000), 1e-3);
  auto element1 = result[1];
  CHECK_EQ(element1.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element1.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element1.points[0].y - 50.000000), 1e-3);
  auto element2 = result[2];
  CHECK_EQ(element2.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element2.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element2.points[0].y - 96.423324), 1e-3);
  auto element3 = result[3];
  CHECK_EQ(element3.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element3.points[0].x - 260.028046), 1e-3);
  CHECK_LT(std::abs(element3.points[0].y - 82.899307), 1e-3);
  CHECK_LT(std::abs(element3.points[1].x - 235.936514), 1e-3);
  CHECK_LT(std::abs(element3.points[1].y - 75.000000), 1e-3);
  CHECK_LT(std::abs(element3.points[2].x - 210.000000), 1e-3);
  CHECK_LT(std::abs(element3.points[2].y - 75.000000), 1e-3);
  auto element4 = result[4];
  CHECK_EQ(element4.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element4.points[0].x - 199.658631), 1e-3);
  CHECK_LT(std::abs(element4.points[0].y - 75.000000), 1e-3);
  CHECK_LT(std::abs(element4.points[1].x - 189.610572), 1e-3);
  CHECK_LT(std::abs(element4.points[1].y - 76.255804), 1e-3);
  CHECK_LT(std::abs(element4.points[2].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element4.points[2].y - 78.623232), 1e-3);
  auto element5 = result[5];
  CHECK_EQ(element5.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element5.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element5.points[0].y - 50.000000), 1e-3);
  auto element6 = result[6];
  CHECK_EQ(element6.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element6.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element6.points[0].y - 50.000000), 1e-3);
  auto element7 = result[7];
  CHECK_EQ(element7.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element7.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element7.points[0].y - 350.000000), 1e-3);
  auto element8 = result[8];
  CHECK_EQ(element8.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element8.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element8.points[0].y - 350.000000), 1e-3);
  auto element9 = result[9];
  CHECK_EQ(element9.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element9.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element9.points[0].y - 321.376768), 1e-3);
  auto element10 = result[10];
  CHECK_EQ(element10.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element10.points[0].x - 189.610572), 1e-3);
  CHECK_LT(std::abs(element10.points[0].y - 323.744196), 1e-3);
  CHECK_LT(std::abs(element10.points[1].x - 199.658631), 1e-3);
  CHECK_LT(std::abs(element10.points[1].y - 325.000000), 1e-3);
  CHECK_LT(std::abs(element10.points[2].x - 210.000000), 1e-3);
  CHECK_LT(std::abs(element10.points[2].y - 325.000000), 1e-3);
  auto element11 = result[11];
  CHECK_EQ(element11.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element11.points[0].x - 235.936514), 1e-3);
  CHECK_LT(std::abs(element11.points[0].y - 325.000000), 1e-3);
  CHECK_LT(std::abs(element11.points[1].x - 260.028046), 1e-3);
  CHECK_LT(std::abs(element11.points[1].y - 317.100693), 1e-3);
  CHECK_LT(std::abs(element11.points[2].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element11.points[2].y - 303.576676), 1e-3);
  auto element12 = result[12];
  CHECK_EQ(element12.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element12.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element12.points[0].y - 350.000000), 1e-3);
  auto element13 = result[13];
  CHECK_EQ(element13.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element13.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element13.points[0].y - 350.000000), 1e-3);
}

TEST_CASE("complex shapes difference") {
  FBBezierPath path1;
  addRectangle(path1, {{50., 50.}, {350., 300.}});
  addCircle(path1, {210., 200.}, 125.);

  FBBezierPath path2;
  addRectangle(path2, {{180., 5.}, {100., 400.}});

  auto result = path1.differenceWithPath(path2);

  CHECK_EQ(result.size(), 18);
  auto element0 = result[0];
  CHECK_EQ(element0.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element0.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element0.points[0].y - 50.000000), 1e-3);
  auto element1 = result[1];
  CHECK_EQ(element1.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element1.points[0].x - 50.000000), 1e-3);
  CHECK_LT(std::abs(element1.points[0].y - 50.000000), 1e-3);
  auto element2 = result[2];
  CHECK_EQ(element2.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element2.points[0].x - 50.000000), 1e-3);
  CHECK_LT(std::abs(element2.points[0].y - 350.000000), 1e-3);
  auto element3 = result[3];
  CHECK_EQ(element3.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element3.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element3.points[0].y - 350.000000), 1e-3);
  auto element4 = result[4];
  CHECK_EQ(element4.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element4.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element4.points[0].y - 321.376768), 1e-3);
  auto element5 = result[5];
  CHECK_EQ(element5.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element5.points[0].x - 125.453539), 1e-3);
  CHECK_LT(std::abs(element5.points[0].y - 307.940020), 1e-3);
  CHECK_LT(std::abs(element5.points[1].x - 85.000000), 1e-3);
  CHECK_LT(std::abs(element5.points[1].y - 258.694225), 1e-3);
  CHECK_LT(std::abs(element5.points[2].x - 85.000000), 1e-3);
  CHECK_LT(std::abs(element5.points[2].y - 200.000000), 1e-3);
  auto element6 = result[6];
  CHECK_EQ(element6.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element6.points[0].x - 85.000000), 1e-3);
  CHECK_LT(std::abs(element6.points[0].y - 141.305775), 1e-3);
  CHECK_LT(std::abs(element6.points[1].x - 125.453539), 1e-3);
  CHECK_LT(std::abs(element6.points[1].y - 92.059980), 1e-3);
  CHECK_LT(std::abs(element6.points[2].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element6.points[2].y - 78.623232), 1e-3);
  auto element7 = result[7];
  CHECK_EQ(element7.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element7.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element7.points[0].y - 50.000000), 1e-3);
  auto element8 = result[8];
  CHECK_EQ(element8.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element8.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element8.points[0].y - 50.000000), 1e-3);
  auto element9 = result[9];
  CHECK_EQ(element9.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element9.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element9.points[0].y - 50.000000), 1e-3);
  auto element10 = result[10];
  CHECK_EQ(element10.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element10.points[0].x - 400.000000), 1e-3);
  CHECK_LT(std::abs(element10.points[0].y - 50.000000), 1e-3);
  auto element11 = result[11];
  CHECK_EQ(element11.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element11.points[0].x - 400.000000), 1e-3);
  CHECK_LT(std::abs(element11.points[0].y - 350.000000), 1e-3);
  auto element12 = result[12];
  CHECK_EQ(element12.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element12.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element12.points[0].y - 350.000000), 1e-3);
  auto element13 = result[13];
  CHECK_EQ(element13.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element13.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element13.points[0].y - 303.576676), 1e-3);
  auto element14 = result[14];
  CHECK_EQ(element14.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element14.points[0].x - 313.187685), 1e-3);
  CHECK_LT(std::abs(element14.points[0].y - 281.103620), 1e-3);
  CHECK_LT(std::abs(element14.points[1].x - 335.000000), 1e-3);
  CHECK_LT(std::abs(element14.points[1].y - 243.099080), 1e-3);
  CHECK_LT(std::abs(element14.points[2].x - 335.000000), 1e-3);
  CHECK_LT(std::abs(element14.points[2].y - 200.000000), 1e-3);
  auto element15 = result[15];
  CHECK_EQ(element15.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element15.points[0].x - 335.000000), 1e-3);
  CHECK_LT(std::abs(element15.points[0].y - 156.900920), 1e-3);
  CHECK_LT(std::abs(element15.points[1].x - 313.187685), 1e-3);
  CHECK_LT(std::abs(element15.points[1].y - 118.896380), 1e-3);
  CHECK_LT(std::abs(element15.points[2].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element15.points[2].y - 96.423324), 1e-3);
  auto element16 = result[16];
  CHECK_EQ(element16.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element16.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element16.points[0].y - 50.000000), 1e-3);
  auto element17 = result[17];
  CHECK_EQ(element17.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element17.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element17.points[0].y - 50.000000), 1e-3);
}

TEST_CASE("complex shapes xor") {
  FBBezierPath path1;
  addRectangle(path1, {{50., 50.}, {350., 300.}});
  addCircle(path1, {210., 200.}, 125.);

  FBBezierPath path2;
  addRectangle(path2, {{180., 5.}, {100., 400.}});

  auto result = path1.xorWithPath(path2);

  CHECK_EQ(result.size(), 38);
  auto element0 = result[0];
  CHECK_EQ(element0.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element0.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element0.points[0].y - 50.000000), 1e-3);
  auto element1 = result[1];
  CHECK_EQ(element1.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element1.points[0].x - 50.000000), 1e-3);
  CHECK_LT(std::abs(element1.points[0].y - 50.000000), 1e-3);
  auto element2 = result[2];
  CHECK_EQ(element2.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element2.points[0].x - 50.000000), 1e-3);
  CHECK_LT(std::abs(element2.points[0].y - 350.000000), 1e-3);
  auto element3 = result[3];
  CHECK_EQ(element3.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element3.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element3.points[0].y - 350.000000), 1e-3);
  auto element4 = result[4];
  CHECK_EQ(element4.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element4.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element4.points[0].y - 405.000000), 1e-3);
  auto element5 = result[5];
  CHECK_EQ(element5.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element5.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element5.points[0].y - 405.000000), 1e-3);
  auto element6 = result[6];
  CHECK_EQ(element6.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element6.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element6.points[0].y - 350.000000), 1e-3);
  auto element7 = result[7];
  CHECK_EQ(element7.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element7.points[0].x - 400.000000), 1e-3);
  CHECK_LT(std::abs(element7.points[0].y - 350.000000), 1e-3);
  auto element8 = result[8];
  CHECK_EQ(element8.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element8.points[0].x - 400.000000), 1e-3);
  CHECK_LT(std::abs(element8.points[0].y - 50.000000), 1e-3);
  auto element9 = result[9];
  CHECK_EQ(element9.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element9.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element9.points[0].y - 50.000000), 1e-3);
  auto element10 = result[10];
  CHECK_EQ(element10.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element10.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element10.points[0].y - 5.000000), 1e-3);
  auto element11 = result[11];
  CHECK_EQ(element11.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element11.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element11.points[0].y - 5.000000), 1e-3);
  auto element12 = result[12];
  CHECK_EQ(element12.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element12.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element12.points[0].y - 50.000000), 1e-3);
  auto element13 = result[13];
  CHECK_EQ(element13.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element13.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element13.points[0].y - 50.000000), 1e-3);
  auto element14 = result[14];
  CHECK_EQ(element14.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element14.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element14.points[0].y - 321.376768), 1e-3);
  auto element15 = result[15];
  CHECK_EQ(element15.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element15.points[0].x - 125.453539), 1e-3);
  CHECK_LT(std::abs(element15.points[0].y - 307.940020), 1e-3);
  CHECK_LT(std::abs(element15.points[1].x - 85.000000), 1e-3);
  CHECK_LT(std::abs(element15.points[1].y - 258.694225), 1e-3);
  CHECK_LT(std::abs(element15.points[2].x - 85.000000), 1e-3);
  CHECK_LT(std::abs(element15.points[2].y - 200.000000), 1e-3);
  auto element16 = result[16];
  CHECK_EQ(element16.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element16.points[0].x - 85.000000), 1e-3);
  CHECK_LT(std::abs(element16.points[0].y - 141.305775), 1e-3);
  CHECK_LT(std::abs(element16.points[1].x - 125.453539), 1e-3);
  CHECK_LT(std::abs(element16.points[1].y - 92.059980), 1e-3);
  CHECK_LT(std::abs(element16.points[2].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element16.points[2].y - 78.623232), 1e-3);
  auto element17 = result[17];
  CHECK_EQ(element17.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element17.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element17.points[0].y - 321.376768), 1e-3);
  auto element18 = result[18];
  CHECK_EQ(element18.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element18.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element18.points[0].y - 321.376768), 1e-3);
  auto element19 = result[19];
  CHECK_EQ(element19.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element19.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element19.points[0].y - 303.576676), 1e-3);
  auto element20 = result[20];
  CHECK_EQ(element20.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element20.points[0].x - 313.187685), 1e-3);
  CHECK_LT(std::abs(element20.points[0].y - 281.103620), 1e-3);
  CHECK_LT(std::abs(element20.points[1].x - 335.000000), 1e-3);
  CHECK_LT(std::abs(element20.points[1].y - 243.099080), 1e-3);
  CHECK_LT(std::abs(element20.points[2].x - 335.000000), 1e-3);
  CHECK_LT(std::abs(element20.points[2].y - 200.000000), 1e-3);
  auto element21 = result[21];
  CHECK_EQ(element21.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element21.points[0].x - 335.000000), 1e-3);
  CHECK_LT(std::abs(element21.points[0].y - 156.900920), 1e-3);
  CHECK_LT(std::abs(element21.points[1].x - 313.187685), 1e-3);
  CHECK_LT(std::abs(element21.points[1].y - 118.896380), 1e-3);
  CHECK_LT(std::abs(element21.points[2].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element21.points[2].y - 96.423324), 1e-3);
  auto element22 = result[22];
  CHECK_EQ(element22.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element22.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element22.points[0].y - 303.576676), 1e-3);
  auto element23 = result[23];
  CHECK_EQ(element23.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element23.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element23.points[0].y - 303.576676), 1e-3);
  auto element24 = result[24];
  CHECK_EQ(element24.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element24.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element24.points[0].y - 50.000000), 1e-3);
  auto element25 = result[25];
  CHECK_EQ(element25.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element25.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element25.points[0].y - 50.000000), 1e-3);
  auto element26 = result[26];
  CHECK_EQ(element26.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element26.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element26.points[0].y - 96.423324), 1e-3);
  auto element27 = result[27];
  CHECK_EQ(element27.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element27.points[0].x - 260.028046), 1e-3);
  CHECK_LT(std::abs(element27.points[0].y - 82.899307), 1e-3);
  CHECK_LT(std::abs(element27.points[1].x - 235.936514), 1e-3);
  CHECK_LT(std::abs(element27.points[1].y - 75.000000), 1e-3);
  CHECK_LT(std::abs(element27.points[2].x - 210.000000), 1e-3);
  CHECK_LT(std::abs(element27.points[2].y - 75.000000), 1e-3);
  auto element28 = result[28];
  CHECK_EQ(element28.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element28.points[0].x - 199.658631), 1e-3);
  CHECK_LT(std::abs(element28.points[0].y - 75.000000), 1e-3);
  CHECK_LT(std::abs(element28.points[1].x - 189.610572), 1e-3);
  CHECK_LT(std::abs(element28.points[1].y - 76.255804), 1e-3);
  CHECK_LT(std::abs(element28.points[2].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element28.points[2].y - 78.623232), 1e-3);
  auto element29 = result[29];
  CHECK_EQ(element29.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element29.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element29.points[0].y - 50.000000), 1e-3);
  auto element30 = result[30];
  CHECK_EQ(element30.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element30.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element30.points[0].y - 50.000000), 1e-3);
  auto element31 = result[31];
  CHECK_EQ(element31.type, FBBezierPath::Type::move);
  CHECK_LT(std::abs(element31.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element31.points[0].y - 350.000000), 1e-3);
  auto element32 = result[32];
  CHECK_EQ(element32.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element32.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element32.points[0].y - 350.000000), 1e-3);
  auto element33 = result[33];
  CHECK_EQ(element33.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element33.points[0].x - 180.000000), 1e-3);
  CHECK_LT(std::abs(element33.points[0].y - 321.376768), 1e-3);
  auto element34 = result[34];
  CHECK_EQ(element34.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element34.points[0].x - 189.610572), 1e-3);
  CHECK_LT(std::abs(element34.points[0].y - 323.744196), 1e-3);
  CHECK_LT(std::abs(element34.points[1].x - 199.658631), 1e-3);
  CHECK_LT(std::abs(element34.points[1].y - 325.000000), 1e-3);
  CHECK_LT(std::abs(element34.points[2].x - 210.000000), 1e-3);
  CHECK_LT(std::abs(element34.points[2].y - 325.000000), 1e-3);
  auto element35 = result[35];
  CHECK_EQ(element35.type, FBBezierPath::Type::curve);
  CHECK_LT(std::abs(element35.points[0].x - 235.936514), 1e-3);
  CHECK_LT(std::abs(element35.points[0].y - 325.000000), 1e-3);
  CHECK_LT(std::abs(element35.points[1].x - 260.028046), 1e-3);
  CHECK_LT(std::abs(element35.points[1].y - 317.100693), 1e-3);
  CHECK_LT(std::abs(element35.points[2].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element35.points[2].y - 303.576676), 1e-3);
  auto element36 = result[36];
  CHECK_EQ(element36.type, FBBezierPath::Type::line);
  CHECK_LT(std::abs(element36.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element36.points[0].y - 350.000000), 1e-3);
  auto element37 = result[37];
  CHECK_EQ(element37.type, FBBezierPath::Type::close);
  CHECK_LT(std::abs(element37.points[0].x - 280.000000), 1e-3);
  CHECK_LT(std::abs(element37.points[0].y - 350.000000), 1e-3);
}
