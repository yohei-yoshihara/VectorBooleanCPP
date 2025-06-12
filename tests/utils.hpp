#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include "vectorboolean/VectorBoolean.hpp"

void addRectangle(fb::FBBezierPath &path, const fb::FBRect &rect);
void addCircle(fb::FBBezierPath &path, const fb::FBPoint &center, fb::FBFloat radius);
void addArcShape(fb::FBBezierPath &path, const fb::FBRect &rect);

#endif