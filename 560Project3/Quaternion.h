// Quaternion.h : header file
// Declares quaternion conversion and evaluation functions
// Learned through Jeff Lander's tutorial and 
// demonstration of inverse kinematics on darwin3d.com

#ifndef QUATERNION_H
#define QUATERNION_H

#include "MathFuncs.h"

// EulerToQuaternion()
// Converts a set of Euler angles to a Quaternion
void EulerToQuaternion(tVector* rot, tQuaternion* quat);

// EulerToQuaternion2()
// Same result as EulerToQuaternion, but with a different implementation
void EulerToQuaternion2(tVector* rot, tQuaternion* quat);

// QuatToAxisAngle()
// Converts a Quaternion to an Axis Angle representation
void QuatToAxisAngle(tQuaternion* quat, tQuaternion* axisAngle);

// SlerpQuat()
// Slerp function between two quaternions
void SlerpQuat(tQuaternion* quat1, tQuaternion* quat2, float slerp, tQuaternion* result);

#endif