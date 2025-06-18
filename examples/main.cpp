#include "vectorboolean/VectorBoolean.hpp"

using namespace fb;

int main(int argc, char *argv[]) {
  auto path1 = FBBezierPath::circle(FBPoint{7, 7}, 5);
  path1.writeSVG("path1.svg");
  
  auto path2 = FBBezierPath::rect(FBRect{FBPoint{0, 0}, FBSize{5, 5}});
  path2.writeSVG("path2.svg");

  auto unionPath = path1.unionWithPath(path2);
  unionPath.writeSVG("union.svg");

  auto intersectPath = path1.intersectWithPath(path2);
  intersectPath.writeSVG("intersect.svg");

  auto differencePath = path1.differenceWithPath(path2);
  differencePath.writeSVG("difference.svg");

  auto xorPath = path1.xorWithPath(path2);
  xorPath.writeSVG("xor.svg");

  return 0;
}
