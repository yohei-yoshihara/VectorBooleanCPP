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
