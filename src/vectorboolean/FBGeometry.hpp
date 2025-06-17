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
FBFloat FBDistanceBetweenPoints(FBPoint point1, FBPoint point2);
FBFloat FBDistancePointToLine(FBPoint point, FBPoint lineStartPoint, FBPoint lineEndPoint);
FBPoint FBLineNormal(FBPoint lineStart, FBPoint lineEnd);
FBPoint FBLineMidpoint(FBPoint lineStart, FBPoint lineEnd);

FBPoint FBAddPoint(FBPoint point1, FBPoint point2);
FBPoint FBScalePoint(FBPoint point, FBFloat scale);
FBPoint FBUnitScalePoint(FBPoint point, FBFloat scale);
FBPoint FBSubtractPoint(FBPoint point1, FBPoint point2);
FBFloat FBDotMultiplyPoint(FBPoint point1, FBPoint point2);
FBFloat FBPointLength(FBPoint point);
FBFloat FBPointSquaredLength(FBPoint point);
FBPoint FBNormalizePoint(FBPoint point);
FBPoint FBNegatePoint(FBPoint point);
FBPoint FBRoundPoint(FBPoint point);

FBPoint FBRectGetTopLeft(FBRect rect);
FBPoint FBRectGetTopRight(FBRect rect);
FBPoint FBRectGetBottomLeft(FBRect rect);
FBPoint FBRectGetBottomRight(FBRect rect);

void FBExpandBoundsByPoint(FBPoint *topLeft, FBPoint *bottomRight, FBPoint point);
FBRect FBUnionRect(FBRect rect1, FBRect rect2);

bool FBArePointsClose(FBPoint point1, FBPoint point2);
bool FBArePointsCloseWithOptions(FBPoint point1, FBPoint point2, FBFloat threshold);
bool FBAreValuesClose(FBFloat value1, FBFloat value2);
bool FBAreValuesCloseWithOptions(FBFloat value1, FBFloat value2, FBFloat threshold);
bool FBIsValueGreaterThan(FBFloat value, FBFloat minimum);
bool FBIsValueLessThan(FBFloat value, FBFloat maximum);
bool FBIsValueGreaterThanEqual(FBFloat value, FBFloat minimum);
bool FBIsValueLessThanEqual(FBFloat value, FBFloat maximum);

extern bool FBLineBoundsMightOverlap(FBRect bounds1, FBRect bounds2);

//////////////////////////////////////////////////////////////////////////
// Angle Range structure provides a simple way to store angle ranges
//  and determine if a specific angle falls within.
//
typedef struct FBAngleRange {
  FBFloat minimum;
  FBFloat maximum;
} FBAngleRange;

FBAngleRange FBAngleRangeMake(FBFloat minimum, FBFloat maximum);
bool FBAngleRangeContainsAngle(FBAngleRange range, FBFloat angle);

FBFloat NormalizeAngle(FBFloat value);
FBFloat PolarAngle(FBPoint point);

//////////////////////////////////////////////////////////////////////////////////
// Parameter ranges
//

// FBRange is a range of parameter (t)
typedef struct FBRange {
  FBFloat minimum;
  FBFloat maximum;
} FBRange;

extern FBRange FBRangeMake(FBFloat minimum, FBFloat maximum);
extern bool FBRangeHasConverged(FBRange range, size_t places);
extern FBFloat FBRangeGetSize(FBRange range);
extern FBFloat FBRangeAverage(FBRange range);
extern FBFloat FBRangeScaleNormalizedValue(FBRange range, FBFloat value);
extern FBRange FBRangeUnion(FBRange range1, FBRange range2);

extern bool FBTangentsCross(FBPoint edge1Tangents[2], FBPoint edge2Tangents[2]);
extern bool FBAreTangentsAmbigious(FBPoint edge1Tangents[2], FBPoint edge2Tangents[2]);
} // namespace fb
