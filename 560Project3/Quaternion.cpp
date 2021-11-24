// Quaternion.cpp : source file
// Declares quaternion conversion and evaluation functions
// Learned through Jeff Lander's tutorial and 
// demonstration of inverse kinematics on darwin3d.com

#include "pch.h"
#include "stdafx.h"
#include <math.h>
#include "Skeleton.h"
#include "Quaternion.h"

// Vector functions to help with Quaternion functions

// CopyVector()
// Copies a vector
void CopyVector(tVector* dest, tVector* src) {
	dest->x = src->x;
	dest->y = src->y;
	dest->z = src->z;
}

// ScaleVector()
// Scales a vector
void ScaleVector(tVector* vect, float scale) {
	vect->x *= scale;
	vect->y *= scale;
	vect->z *= scale;
}

// AddVectors()
// Adds two vectors
void AddVectors(tVector* vect1, tVector* vect2, tVector* dest) {
	dest->x = vect1->x + vect2->x;
	dest->y = vect1->y + vect2->y;
	dest->z = vect1->z + vect2->z;
}

// DotVectors()
// Takes the dot product of two vectors
float DotVectors(tVector* vect1, tVector* vect2) {
	return (vect1->x * vect2->x) + (vect1->y * vect2->y) + (vect1->z * vect2->z);
}

// CrossVectors()
// Computes the cross product of two vectors
void CrossVectors(tVector* vect1, tVector* vect2, tVector* dest) {
	dest->x = (vect1->y * vect2->z) - (vect1->z * vect2->y);
	dest->y = (vect1->z * vect2->x) - (vect1->x * vect2->z);
	dest->z = (vect1->x * vect2->y) - (vect1->y * vect2->x);
}

// MultQuaternions()
// Compute the product of two quaternions
void MultQuaternions(tQuaternion* quat1, tQuaternion* quat2, tQuaternion* dest) {
	// local vars
	tQuaternion v1, v2, v3, vf;

	CopyVector((tVector*)&v1, (tVector*)quat1); // Copy off the vector part of quat1
	ScaleVector((tVector*)&v1, quat2->w); // Multiply it by the scalar part of quat2

	CopyVector((tVector*)&v2, (tVector*)quat2); // Copy off the vector part of quat2
	ScaleVector((tVector*)&v2, quat1->w); // Multiply it by the scalar part of quat1

	// store the cross product in v3
	CrossVectors((tVector*)quat2, (tVector*)quat1, (tVector*)&v3);

	// Add together all of the vector values for the quaternion
	AddVectors((tVector*)&v1, (tVector*)&v2, (tVector*)&vf);
	AddVectors((tVector*)&v3, (tVector*)&vf, (tVector*)&vf);

	// calculate the w value
	vf.w = (quat1->w * quat2->w) - DotVectors((tVector*)quat1, (tVector*)quat2);

	// store vf into dest
	dest->x = vf.x;
	dest->y = vf.y;
	dest->z = vf.z;
	dest->w = vf.w;

}

// Optimization/re-structuring of the above function
// MultQuaternions2()
// Computes the product of two quaternions
void MultQuaternions2(tQuaternion* quat1, tQuaternion* quat2, tQuaternion* dest) {
	// create temporary quat for arithmatic purposes
	tQuaternion tmp;

	// calculate values
	tmp.x = quat2->w * quat1->x + quat2->x * quat1->w + quat2->y * quat1->z - quat2->z * quat1->y;
	tmp.y = quat2->w * quat1->y + quat2->y * quat1->w + quat2->z * quat1->x - quat2->x * quat1->z;
	tmp.z = quat2->w * quat1->z + quat2->z * quat1->w + quat2->x * quat1->y - quat2->y * quat1->x;
	tmp.w = quat2->w * quat1->w - quat2->x * quat1->x - quat2->y * quat1->y - quat2->z * quat1->z;

	// store tmp in dest
	dest->x = tmp.x;
	dest->y = tmp.y;
	dest->z = tmp.z;
	dest->w = tmp.w;
}

// NormalizeQuaternion()
// normalizes a quaternion
void NormalizeQuaternion(tQuaternion* quat) {
	// local vars
	float magnitude;

	// 1. find the magnitude
	magnitude = (quat->x * quat->x) + (quat->y * quat->y) + (quat->z * quat->z) + (quat->w * quat->w);

	// 2. divide by magnitude
	quat->x = quat->x / magnitude;
	quat->y = quat->y / magnitude;
	quat->z = quat->z / magnitude;
	quat->w = quat->w / magnitude;
}

// The next two equations are functionally the same, but provide different methods about computation

// EulerToQuaternion()
// Convert a set of Euler angles to a Quaternion
// Note:  this uses the quantum mechanics convention of (X,Y,Z)
// as a Yaw-Pitch-Roll system would need adjustments
void EulerToQuaternion(tVector* rot, tQuaternion* quat) {
	// local vars
	float rx, ry, rz, tx, ty, tz, cx, cy, cz, sx, sy, sz, cc, cs, sc, ss;

	// 1. Convert angles to radians
	rx = DEGTORAD(rot->x);
	ry = DEGTORAD(rot->y);
	rz = DEGTORAD(rot->z);

	// 2. get the half angles
	tx = rx * 0.5f;
	ty = ry * 0.5f;
	tz = rz * 0.5f;

	cx = (float)cos(tx);
	cy = (float)cos(ty);
	cz = (float)cos(tz);

	sx = (float)sin(tx);
	sy = (float)sin(ty);
	sz = (float)sin(tz);

	cc = cx * cz;
	cs = cx * sz;
	sc = sx * cz;
	ss = sx * sz;

	// calculate quaternion values
	quat->x = (cy * sc) - (sy * cs);
	quat->y = (cy * ss) - (sy * cc);
	quat->z = (cy * cs) - (sy * sc);
	quat->w = (cy * cc) - (sy * ss);

	// ensure that quaternion is normalized
	// may not be necessary in most cases, but is still safe
	NormalizeQuaternion(quat);
}

// EulerToQuaternion2()
// Convert a set of Euler angles to a Quaternion
// Note:  creates a series of quaternions and multiplies them together
void EulerToQuaternion2(tVector* rot, tQuaternion* quat) {
	// local vars
	float rx, ry, rz, ti, tj, tk;
	tQuaternion qx, qy, qz, qf;

	// 1. Convert angles to radians
	rx = DEGTORAD(rot->x);
	ry = DEGTORAD(rot->y);
	rz = DEGTORAD(rot->z);

	// 2. Get the half angles
	ti = rx * 0.5f;
	tj = ry * 0.5f;
	tk = rz * 0.5f;

	// calculate temp quaternions
	qx.x = (float)sin(ti); qx.y = 0.0f; qx.z = 0.0f; qx.w = (float)cos(ti);
	qy.x = 0.0f; qy.y = (float)sin(tj); qy.z = 0.0f; qy.w = (float)cos(tj);
	qz.x = 0.0f; qz.y = 0.0f; qz.z = (float)sin(tk); qz.w = (float)cos(tk);

	// multiply quaternions to get final value
	MultQuaternions(&qx, &qy, &qf);
	MultQuaternions(&qf, &qz, &qf);
	// Could also use MultQuaternions2 above

	// ensure quaternion is normalized
	// may not be necessary in most cases, but is still safe
	NormalizeQuaternion(&qf);

	quat->x = qf.x;
	quat->y = qf.y;
	quat->z = qf.z;
	quat->w = qf.w;
}

// QuatToAngleAxis()
// Convert a Quaternion to Axis Angle representation
void QuatToAxisAngle(tQuaternion* quat, tQuaternion* axisAngle) {
	// local vars
	float scale, tw;

	// determine the scale factor
	tw = (float)acos(quat->w) * 2;
	scale = (float)sin(tw / 2.0);

	// calculate vector values for axisAngle
	axisAngle->x = quat->x / scale;
	axisAngle->y = quat->y / scale;
	axisAngle->z = quat->z / scale;

	// convert the angle of rotation back to degrees
	axisAngle->w = RADTODEG(tw);
}

// difference at which to lerp instead of slerp
// can be played around with to see the differences between the two
#define DELTA 0.0001

// SlerpQuat()
// Slerp function between two quaternions
void SlerpQuat(tQuaternion* quat1, tQuaternion* quat2, float slerp, tQuaternion* result) {
	// local vars
	double omega, cosom, sinom, scale0, scale1;

	// use the dot product to get the cosine of the angle between the quaternions
	cosom = quat1->x * quat2->x + quat1->y * quat2->y + quat1->z * quat2->z + quat1->w * quat2->w;

	// checks for special cases
	// make sure the two quats are not exact opposites
	if ((1.0f + cosom) > DELTA) {
		// are they more than a little bit different?
		if ((1.0f - cosom) > DELTA) {
			// yes, do a slerp
			omega = acos(cosom);
			sinom = sin(omega);
			scale0 = sin((1.0f - slerp) * omega) / sinom;
			scale1 = sin(slerp * omega) / sinom;
		}
		else {
			// difference isn't big, do a lerp
			scale0 = 1.0f - slerp;
			scale1 = slerp;
		}

		// calculate resulting quaternion
		result->x = scale0 * quat1->x + scale1 * quat2->x;
		result->y = scale0 * quat1->y + scale1 * quat2->y;
		result->z = scale0 * quat1->z + scale1 * quat2->z;
		result->w = scale0 * quat1->w + scale1 * quat2->w;
	}
	else {
		// the quaternions are nearly opposite, so to avoid a divide by zero error,
		// we calculate a perpendicular quaternion and slerp that direction
		result->x = -quat2->y;
		result->y = quat2->x;
		result->z = -quat2->w;
		result->w = quat2->z;
		scale0 = sin((1.0 - slerp) * (float)HALF_PI);
		scale1 = sin(slerp * (float)HALF_PI);
		result->x = scale0 * quat1->x + scale1 * result->x;
		result->y = scale0 * quat1->y + scale1 * result->y;
		result->z = scale0 * quat1->z + scale1 * result->z;
		result->w = scale0 * quat1->w + scale1 * result->w;
	}
}