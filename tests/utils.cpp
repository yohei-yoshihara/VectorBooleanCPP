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

#include "utils.hpp"

void addRectangle(fb::FBBezierPath &path, const fb::FBRect &rect) {
  path.moveTo(rect.origin);
  path.lineTo({rect.origin.x + rect.size.width, rect.origin.y});
  path.lineTo({rect.origin.x + rect.size.width, rect.origin.y + rect.size.height});
  path.lineTo({rect.origin.x, rect.origin.y + rect.size.height});
  path.close();
}

void addCircle(fb::FBBezierPath &path, const fb::FBPoint &center, fb::FBFloat radius) {
  static const fb::FBFloat FBMagicNumber = 0.55228475;
  fb::FBFloat controlPointLength = radius * FBMagicNumber;
  path.moveTo({center.x - radius, center.y});
  path.curveTo({center.x, center.y + radius}, {center.x - radius, center.y + controlPointLength},
               {center.x - controlPointLength, center.y + radius});
  path.curveTo({center.x + radius, center.y}, {center.x + controlPointLength, center.y + radius},
               {center.x + radius, center.y + controlPointLength});
  path.curveTo({center.x, center.y - radius}, {center.x + radius, center.y - controlPointLength},
               {center.x + controlPointLength, center.y - radius});
  path.curveTo({center.x - radius, center.y}, {center.x - controlPointLength, center.y - radius},
               {center.x - radius, center.y - controlPointLength});
}

void addArcShape(fb::FBBezierPath &path, const fb::FBRect &rect) {
  path.moveTo(rect.origin);
  path.lineTo({rect.origin.x + rect.size.width, rect.origin.y});
  path.curveTo(fb::FBPoint{rect.origin.x, rect.origin.y + rect.size.height},
               fb::FBPoint{rect.origin.x + rect.size.width, (rect.origin.y + rect.size.height) / 2.0},
               fb::FBPoint{(rect.origin.x + rect.size.width) / 2.0, rect.origin.y + rect.size.height});
  path.close();
}
