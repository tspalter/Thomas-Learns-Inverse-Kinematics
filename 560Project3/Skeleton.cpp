// Skeleton.cpp : source file
// Structure definitions of hierarchical animation system
// Learned through Jeff Lander's tutorial and 
// demonstration of inverse kinematics on darwin3d.com

#include "pch.h"
#include "stdafx.h"
#include "MathFuncs.h"
#include "Skeleton.h"

// destroys the entire skeleton recursively starting from the root bone
void DestroySkeleton(tBone* root) {
	// local vars
	int loop;
	tBone* child;
	// we'll need to get rid of every child bone of root
	if (root->childCount > 0) {
		// iterate through the children of root
		child = root->children;
		for (loop = 0; loop < root->childCount; loop++, child++) {
			if (child->childCount > 0) {
				// destroy the additional children of child
				DestroySkeleton(child);
			}
			if (child->primChannel > NULL) {
				// dealloc child
				free(child->primChannel);
				child->primChannel = NULL;
			}
		}
		// fully iterated through root, now dealloc root
		free(root->children);
	}
	
	// now reset the root values
	root->primChanType = CHANNEL_TYPE_NONE;
	root->secChanType = CHANNEL_TYPE_NONE;
	root->primFrameCount = 0;
	root->secFrameCount = 0;
	root->primCurFrame = 0;
	root->secCurFrame = 0;
	root->primChannel = NULL;
	root->secChannel = NULL;

	root->visualCount = 0;					// Count of attached visual elements
	root->visuals = NULL;					// pointer to visuals
	root->childCount = 0;						// Count of attached bone elements
	root->children = NULL;					// Pointer to children
}

// Resets a given bone, while maintaining its parent bone
void ResetBone(tBone* bone, tBone* parent) {
	// reset scale factors
	bone->bScale.x = bone->bScale.y = bone->bScale.z = 1.0f;
	bone->scale.x = bone->scale.y = bone->scale.z = 1.0f;

	// reset rotation factors
	bone->bRot.x = bone->bRot.y = bone->bRot.z = 0.0f;
	bone->rot.x = bone->rot.y = bone->rot.z = 0.0f;

	// reset translation factors
	bone->bTrans.x = bone->bTrans.y = bone->bTrans.y = 0.0f;
	bone->trans.x = bone->trans.y = bone->trans.y = 0.0f;

	// reset channel information
	bone->primChanType = CHANNEL_TYPE_NONE;
	bone->secChanType = CHANNEL_TYPE_NONE;
	bone->primFrameCount = 0;
	bone->secFrameCount = 0;
	bone->primCurFrame = 0;
	bone->secCurFrame = 0;
	bone->primChannel = NULL;
	bone->secChannel = NULL;

	// reset visuals and parents/children
	bone->visualCount = 0;
	bone->visuals = NULL;
	bone->childCount = 0;
	bone->children = NULL;
	bone->parent = parent;
}

// set a bone's transforms for a given frame, specifically in the SRT channel
void BoneSetFrame(tBone* bone, int frame) {
	// local var
	float* offset;

	// check that bone is in a channel
	if (bone->primChannel != NULL) {
		offset = (float*)(bone->primChannel + (s_Channel_Type_Size[bone->primChanType] * frame));

		switch (bone->primChanType) {
			// TYPE_SRT has 9 floats in T(x,y,z), R(x,y,z), S(x,y,z) order
		case CHANNEL_TYPE_SRT:
			bone->trans.x = offset[0];
			bone->trans.y = offset[1];
			bone->trans.z = offset[2];

			bone->rot.x = offset[3];
			bone->rot.y = offset[4];
			bone->rot.z = offset[5];

			bone->scale.x = offset[6];
			bone->scale.y = offset[7];
			bone->scale.z = offset[8];
			break;
		}
	}
}

// advance bone to its next frame
void BoneAdvanceFrame(tBone* bone, int direction, BOOL doChildren) {
	// local vars
	int loop;
	tBone* child;

	// iterate through the bone's children
	if (bone->childCount > 0) {
		child = bone->children;
		for (loop = 0; loop < bone->childCount; loop++, child++) {
			// change the current frame, making sure to loop if necessary
			child->primCurFrame += direction;
			if (child->primCurFrame >= child->primFrameCount)
				child->primCurFrame = 0;
			if (child->primCurFrame < 0)
				child->primCurFrame += child->primFrameCount;
			// with the new current frame, set the bone to the proper transform
			BoneSetFrame(child, (int)child->primCurFrame);
			if (doChildren && child->childCount > 0)
				// advance all of the children for the bone recursively
				BoneAdvanceFrame(child, direction, doChildren);
		}
	}
}