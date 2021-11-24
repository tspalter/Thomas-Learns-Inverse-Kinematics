// Skeleton.h : header file
// Structure definitions of hierarchical animation system
// Learned through Jeff Lander's tutorial and 
// demonstration of inverse kinematics on darwin3d.com

#ifndef SKELETON_H
#define SKELETON_H

// Bone Definitions
#define BONE_ID_ROOT 1 // ROOT BONE

// Channel Definitions
#define CHANNEL_TYPE_NONE			0		// NO CHANNEL APPLIED
#define CHANNEL_TYPE_SRT			1		// SCALE ROTATION AND TRANSLATION
#define CHANNEL_TYPE_TRANS			2		// CHANNEL HAS TRANSLATION (X Y Z) ORDER
#define CHANNEL_TYPE_RXYZ			4		// ROTATION (RX RY RZ) ORDER
#define CHANNEL_TYPE_RZXY			8		// ROTATION (RZ RX RY) ORDER
#define CHANNEL_TYPE_RYZX			16		// ROTATION (RY RZ RX) ORDER
#define CHANNEL_TYPE_RZYX			32		// ROTATION (RZ RY RX) ORDER
#define CHANNEL_TYPE_RXZY			64		// ROTATION (RX RZ RY) ORDER
#define CHANNEL_TYPE_RYXZ			128		// ROTATION (RY RX RZ) ORDER
#define CHANNEL_TYPE_S				256		// SCALE ONLY
#define CHANNEL_TYPE_T				512		// TRANSLATION ONLY (X Y Z) ORDER
#define CHANNEL_TYPE_INTERLEAVED	1024	// THIS DATA STREAM HAS MULTIPLE CHANNELS

// Count of number of floats for each channel type
static int s_Channel_Type_Size[] = {
	0,
	9,
	6,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3
};

#include "MathFuncs.h" // we need the typedefs

// Struct that defines a bone in the animation system
struct tBone {
	long id; // Bone ID
	char name[80]; // Bone name
	long flags; // Bone flags

	// Hierarchy info
	tBone* parent; // pointer to parent bone
	int childCount; // count of child bones
	tBone* children; // pointer to children

	// Transformation info
	tVector bScale; // base scale factors
	tVector bRot; // base rotation factors
	tVector bTrans; // base translation factors
	tVector scale; // current scale factors
	tVector rot; // current rotation factors
	tVector trans; // current translation factors
	tQuaternion quat; // quaternion for animation
	tMatrix matrix; // place to store the transformation matrix

	// Animation info
	long primChanType; // Type of primary channel that's attached
	float* primChannel; // Pointer to primary channel
	float primFrameCount; // Frames in primary channel
	float primSpeed; // Current playback speed
	float primCurFrame; // Current frame number in channel
	long secChanType; // Type of secondary channel that's attached
	float* secChannel; // Pointer to secondary channel
	float secFrameCount; // Frames in secondary channel
	float secSpeed; // Current playback speed
	float secCurFrame; // Current frame number in channel
	float animBlend; // Blending factor

	// Depth of Field constraints
	int minRx, maxRx; // X rotation limits
	int minRy, maxRy; // Y rotation limits
	int minRz, maxRz; // Z rotation limits
	float dampWidth, dampStrength; // Dampening settings

	// Visual elements
	int visualCount; // Count of attached visual elements
	long* visuals; // Pointer to visuals/bitmaps
	int* cVptr; // Pointer to control vertices
	float* cVweight; // Pointer to array of weight values

	// Collision elements
	float bbox[6]; // Bounding box (UL xyz, LR xyz)
	tVector center; // Center of object (mass)
	float bSphere; // Bounding sphere (radius)

	// Physics
	tVector length; // Bone length
	float mass; // Mass
	float friction; // Static friction
	float kFriction; // Kinetic friction
	float elast; // elasticity
};

// Functions

void DestroySkeleton(tBone* root);
void ResetBone(tBone* bone, tBone* parent);
void BoneSetFrame(tBone* bone, int frame);
void BoneAdvanceFrame(tBone* bone, int direction, BOOL doChildren);

#endif