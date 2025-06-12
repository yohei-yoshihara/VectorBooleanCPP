/*
Copyright (c) 2011 Andrew Finnell

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

//
//  Geometry.m
//  VectorBrush
//
//  Created by Andrew Finnell on 5/28/11.
//  Copyright 2011 Fortunate Bear, LLC. All rights reserved.
//

#include "FBGeometry.hpp"

namespace fb {

static const FBFloat FBPointClosenessThreshold = 1e-10;
static const FBFloat FBTangentClosenessThreshold = 1e-12;
static const FBFloat FBBoundsClosenessThreshold = 1e-9;

FBFloat FBDistanceBetweenPoints(FBPoint point1, FBPoint point2) {
  FBFloat xDelta = point2.x - point1.x;
  FBFloat yDelta = point2.y - point1.y;
  return sqrt(xDelta * xDelta + yDelta * yDelta);
}

FBFloat FBDistancePointToLine(FBPoint point, FBPoint lineStartPoint, FBPoint lineEndPoint) {
  FBFloat lineLength = FBDistanceBetweenPoints(lineStartPoint, lineEndPoint);
  if (lineLength == 0) {
    return 0;
  }
  FBFloat u = ((point.x - lineStartPoint.x) * (lineEndPoint.x - lineStartPoint.x) +
               (point.y - lineStartPoint.y) * (lineEndPoint.y - lineStartPoint.y)) /
              (lineLength * lineLength);
  FBPoint intersectionPoint = FBMakePoint(lineStartPoint.x + u * (lineEndPoint.x - lineStartPoint.x),
                                          lineStartPoint.y + u * (lineEndPoint.y - lineStartPoint.y));
  return FBDistanceBetweenPoints(point, intersectionPoint);
}

FBPoint FBAddPoint(FBPoint point1, FBPoint point2) { return FBMakePoint(point1.x + point2.x, point1.y + point2.y); }

FBPoint FBUnitScalePoint(FBPoint point, FBFloat scale) {
  FBPoint result = point;
  FBFloat length = FBPointLength(point);
  if (length != 0.0) {
    result.x *= scale / length;
    result.y *= scale / length;
  }
  return result;
}

FBPoint FBScalePoint(FBPoint point, FBFloat scale) { return FBMakePoint(point.x * scale, point.y * scale); }

FBFloat FBDotMultiplyPoint(FBPoint point1, FBPoint point2) { return point1.x * point2.x + point1.y * point2.y; }

FBPoint FBSubtractPoint(FBPoint point1, FBPoint point2) {
  return FBMakePoint(point1.x - point2.x, point1.y - point2.y);
}

FBFloat FBPointLength(FBPoint point) { return sqrt((point.x * point.x) + (point.y * point.y)); }

FBFloat FBPointSquaredLength(FBPoint point) { return (point.x * point.x) + (point.y * point.y); }

FBPoint FBNormalizePoint(FBPoint point) {
  FBPoint result = point;
  FBFloat length = FBPointLength(point);
  if (length != 0.0) {
    result.x /= length;
    result.y /= length;
  }
  return result;
}

FBPoint FBNegatePoint(FBPoint point) { return FBMakePoint(-point.x, -point.y); }

FBPoint FBRoundPoint(FBPoint point) {
  FBPoint result = {round(point.x), round(point.y)};
  return result;
}

FBPoint FBLineNormal(FBPoint lineStart, FBPoint lineEnd) {
  return FBNormalizePoint(FBMakePoint(-(lineEnd.y - lineStart.y), lineEnd.x - lineStart.x));
}

FBPoint FBLineMidpoint(FBPoint lineStart, FBPoint lineEnd) {
  FBFloat distance = FBDistanceBetweenPoints(lineStart, lineEnd);
  FBPoint tangent = FBNormalizePoint(FBSubtractPoint(lineEnd, lineStart));
  return FBAddPoint(lineStart, FBUnitScalePoint(tangent, distance / 2.0));
}

FBPoint FBRectGetTopLeft(FBRect rect) { return FBMakePoint(FBMinX(rect), FBMinY(rect)); }

FBPoint FBRectGetTopRight(FBRect rect) { return FBMakePoint(FBMaxX(rect), FBMinY(rect)); }

FBPoint FBRectGetBottomLeft(FBRect rect) { return FBMakePoint(FBMinX(rect), FBMaxY(rect)); }

FBPoint FBRectGetBottomRight(FBRect rect) { return FBMakePoint(FBMaxX(rect), FBMaxY(rect)); }

void FBExpandBoundsByPoint(FBPoint *topLeft, FBPoint *bottomRight, FBPoint point) {
  if (point.x < topLeft->x) {
    topLeft->x = point.x;
  }
  if (point.x > bottomRight->x) {
    bottomRight->x = point.x;
  }
  if (point.y < topLeft->y) {
    topLeft->y = point.y;
  }
  if (point.y > bottomRight->y) {
    bottomRight->y = point.y;
  }
}

FBRect FBUnionRect(FBRect rect1, FBRect rect2) {
  FBPoint topLeft = FBRectGetTopLeft(rect1);
  FBPoint bottomRight = FBRectGetBottomRight(rect1);
  FBExpandBoundsByPoint(&topLeft, &bottomRight, FBRectGetTopLeft(rect2));
  FBExpandBoundsByPoint(&topLeft, &bottomRight, FBRectGetTopRight(rect2));
  FBExpandBoundsByPoint(&topLeft, &bottomRight, FBRectGetBottomRight(rect2));
  FBExpandBoundsByPoint(&topLeft, &bottomRight, FBRectGetBottomLeft(rect2));
  return FBMakeRect(topLeft.x, topLeft.y, bottomRight.x - topLeft.x, bottomRight.y - topLeft.y);
}

bool FBArePointsClose(FBPoint point1, FBPoint point2) {
  return FBArePointsCloseWithOptions(point1, point2, FBPointClosenessThreshold);
}

bool FBArePointsCloseWithOptions(FBPoint point1, FBPoint point2, FBFloat threshold) {
  return FBAreValuesCloseWithOptions(point1.x, point2.x, threshold) &&
         FBAreValuesCloseWithOptions(point1.y, point2.y, threshold);
}

bool FBAreValuesClose(FBFloat value1, FBFloat value2) {
  return FBAreValuesCloseWithOptions(value1, value2, FBPointClosenessThreshold);
}

bool FBAreValuesCloseWithOptions(FBFloat value1, FBFloat value2, FBFloat threshold) {
  FBFloat delta = value1 - value2;
  return (delta <= threshold) && (delta >= -threshold);
}

//////////////////////////////////////////////////////////////////////////
// Helper methods for angles
//
static const FBFloat FB2PI = 2.0 * M_PI;

// Normalize the angle between 0 and 2pi
FBFloat NormalizeAngle(FBFloat value) {
  while (value < 0.0) {
    value += FB2PI;
  }
  while (value >= FB2PI) {
    value -= FB2PI;
  }
  return value;
}

// Compute the polar angle from the cartesian point
FBFloat PolarAngle(FBPoint point) {
  FBFloat value = 0.0;
  if (point.x > 0.0) {
    value = atan(point.y / point.x);
  } else if (point.x < 0.0) {
    if (point.y >= 0.0) {
      value = atan(point.y / point.x) + M_PI;
    } else {
      value = atan(point.y / point.x) - M_PI;
    }
  } else {
    if (point.y > 0.0) {
      value = M_PI_2;
    } else if (point.y < 0.0) {
      value = -M_PI_2;
    } else {
      value = 0.0;
    }
  }
  return NormalizeAngle(value);
}

FBAngleRange FBAngleRangeMake(FBFloat minimum, FBFloat maximum) {
  FBAngleRange range = {minimum, maximum};
  return range;
}

static bool FBIsValueGreaterThanWithOptions(FBFloat value, FBFloat minimum, FBFloat threshold) {
  if (FBAreValuesCloseWithOptions(value, minimum, threshold)) {
    return false;
  }
  return value > minimum;
}

bool FBIsValueGreaterThan(FBFloat value, FBFloat minimum) {
  return FBIsValueGreaterThanWithOptions(value, minimum, FBTangentClosenessThreshold);
}

bool FBIsValueLessThan(FBFloat value, FBFloat maximum) {
  if (FBAreValuesCloseWithOptions(value, maximum, FBTangentClosenessThreshold)) {
    return false;
  }
  return value < maximum;
}

bool FBIsValueGreaterThanEqual(FBFloat value, FBFloat minimum) {
  if (FBAreValuesCloseWithOptions(value, minimum, FBTangentClosenessThreshold)) {
    return true;
  }
  return value >= minimum;
}

static bool FBIsValueLessThanEqualWithOptions(FBFloat value, FBFloat maximum, FBFloat threshold) {
  if (FBAreValuesCloseWithOptions(value, maximum, threshold)) {
    return true;
  }
  return value <= maximum;
}

bool FBIsValueLessThanEqual(FBFloat value, FBFloat maximum) {
  return FBIsValueLessThanEqualWithOptions(value, maximum, FBTangentClosenessThreshold);
}

bool FBAngleRangeContainsAngle(FBAngleRange range, FBFloat angle) {
  if (range.minimum <= range.maximum) {
    return FBIsValueGreaterThan(angle, range.minimum) && FBIsValueLessThan(angle, range.maximum);
  }

  // The range wraps around 0. See if the angle falls in the first half
  if (FBIsValueGreaterThan(angle, range.minimum) && angle <= FB2PI) {
    return true;
  }

  return angle >= 0.0 && FBIsValueLessThan(angle, range.maximum);
}

//////////////////////////////////////////////////////////////////////////////////
// Parameter ranges
//
FBRange FBRangeMake(FBFloat minimum, FBFloat maximum) {
  FBRange range = {minimum, maximum};
  return range;
}

bool FBRangeHasConverged(FBRange range, size_t places) {
  FBFloat factor = pow(10.0, places);
  int64_t minimum = (int64_t)(range.minimum * factor);
  int64_t maxiumum = (int64_t)(range.maximum * factor);
  return minimum == maxiumum;
}

FBFloat FBRangeGetSize(FBRange range) { return range.maximum - range.minimum; }

FBFloat FBRangeAverage(FBRange range) { return (range.minimum + range.maximum) / 2.0; }

FBFloat FBRangeScaleNormalizedValue(FBRange range, FBFloat value) {
  return (range.maximum - range.minimum) * value + range.minimum;
}

FBRange FBRangeUnion(FBRange range1, FBRange range2) {
  FBRange range = {std::min(range1.minimum, range2.minimum), std::max(range1.maximum, range2.maximum)};
  return range;
}

bool FBAreTangentsAmbigious(FBPoint edge1Tangents[2], FBPoint edge2Tangents[2]) {
  FBPoint normalEdge1[2] = {FBNormalizePoint(edge1Tangents[0]), FBNormalizePoint(edge1Tangents[1])};
  FBPoint normalEdge2[2] = {FBNormalizePoint(edge2Tangents[0]), FBNormalizePoint(edge2Tangents[1])};

  return FBArePointsCloseWithOptions(normalEdge1[0], normalEdge2[0], FBTangentClosenessThreshold) ||
         FBArePointsCloseWithOptions(normalEdge1[0], normalEdge2[1], FBTangentClosenessThreshold) ||
         FBArePointsCloseWithOptions(normalEdge1[1], normalEdge2[0], FBTangentClosenessThreshold) ||
         FBArePointsCloseWithOptions(normalEdge1[1], normalEdge2[1], FBTangentClosenessThreshold);
}

bool FBTangentsCross(FBPoint edge1Tangents[2], FBPoint edge2Tangents[2]) {
  // Calculate angles for the tangents
  FBFloat edge1Angles[] = {PolarAngle(edge1Tangents[0]), PolarAngle(edge1Tangents[1])};
  FBFloat edge2Angles[] = {PolarAngle(edge2Tangents[0]), PolarAngle(edge2Tangents[1])};

  // Count how many times edge2 angles appear between the self angles
  FBAngleRange range1 = FBAngleRangeMake(edge1Angles[0], edge1Angles[1]);
  size_t rangeCount1 = 0;
  if (FBAngleRangeContainsAngle(range1, edge2Angles[0])) {
    rangeCount1++;
  }
  if (FBAngleRangeContainsAngle(range1, edge2Angles[1])) {
    rangeCount1++;
  }

  // Count how many times self angles appear between the edge2 angles
  FBAngleRange range2 = FBAngleRangeMake(edge1Angles[1], edge1Angles[0]);
  size_t rangeCount2 = 0;
  if (FBAngleRangeContainsAngle(range2, edge2Angles[0])) {
    rangeCount2++;
  }
  if (FBAngleRangeContainsAngle(range2, edge2Angles[1])) {
    rangeCount2++;
  }

  // If each pair of angles split the other two, then the edges cross.
  return rangeCount1 == 1 && rangeCount2 == 1;
}

bool FBLineBoundsMightOverlap(FBRect bounds1, FBRect bounds2) {
  FBFloat left = std::max(FBMinX(bounds1), FBMinX(bounds2));
  FBFloat right = std::min(FBMaxX(bounds1), FBMaxX(bounds2));
  if (FBIsValueGreaterThanWithOptions(left, right, FBBoundsClosenessThreshold)) {
    return false; // no horizontal overlap
  }
  FBFloat top = std::max(FBMinY(bounds1), FBMinY(bounds2));
  FBFloat bottom = std::min(FBMaxY(bounds1), FBMaxY(bounds2));
  return FBIsValueLessThanEqualWithOptions(top, bottom, FBBoundsClosenessThreshold);
}

} // namespace fb