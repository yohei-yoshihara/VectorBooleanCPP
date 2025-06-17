#include "vectorboolean/VectorBoolean.hpp"

using namespace fb;

int main(int argc, char *argv[]) {
  FBBezierPath path1;
  path1.moveTo({0, 0});
  path1.lineTo({10, 0});
  path1.curveTo({0, 10}, {10, 5}, {5, 10});
  path1.close();
  path1.writeSVG("path1.svg");
  std::cout << path1.str(0) << std::endl;

  FBBezierPath path2{FBRect{{3, 3}, {7, 7}}};
  path2.writeSVG("path2.svg");

  auto p1 = std::make_shared<FBBezierGraph>(path1);
  std::cout << p1->str(0) << std::endl;
  auto p2 = std::make_shared<FBBezierGraph>(path2);
  std::cout << p2->str(0) << std::endl;

  auto p3 = p1->unionWithBezierGraph(p2);
  std::cout << p3->str(0) << std::endl;

  auto path3 = p3->bezierPath();
  path3.writeSVG("path3.svg");

  return 0;
}
