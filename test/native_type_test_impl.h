#ifndef NATIVE_TYPE_TEST_IMPL_H_
#define NATIVE_TYPE_TEST_IMPL_H_

#include <iostream>

namespace Native {
struct Vector3D {
  float x;
  float y;
  float z;

  Vector3D() {
    x = 0;
    y = 0;
    z = 0;
  }
  Vector3D(float _x, float _y, float _z) {
    this->x = _x;
    this->y = _y;
    this->z = _z;
  }

public:
friend std::ostream& operator<<(std::ostream& os, const Vector3D &c) {
  return os << c.x << c.y << c.z;
  }

};
}  // namespace Native

namespace Geometry {
struct Vector3D;
struct Vector3DAlt;
}  // namespace Geometry

namespace flatbuffers {
Geometry::Vector3D Pack(const Native::Vector3D &obj);
const Native::Vector3D UnPack(const Geometry::Vector3D &obj);
Geometry::Vector3DAlt PackVector3DAlt(const Native::Vector3D &obj);
const Native::Vector3D UnPackVector3DAlt(const Geometry::Vector3DAlt &obj);
}  // namespace flatbuffers

#endif // NATIVE_TYPE_TEST_IMPL_H_
