#pragma once
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <map>
#include <vector>
using std::string;
using std::map;
class Animation {

public:
	static	void SetBoneTransform(const aiAnimation& pAnimation, float AnimationTime, vector<Mesh>& meshes, const aiNode* pNode, const glm::mat4& ParentTransform) {
		string NodeName(pNode->mName.data);
		glm::mat4 NodeTransformation = glm::mat4(1.0f);

		aiMatrix4x4 tp1 = pNode->mTransformation;
		NodeTransformation = glm::transpose(glm::make_mat4(&tp1.a1));

		const aiNodeAnim* pNodeAnim = get_aiNodeAnimBName(pAnimation, NodeName);    

		if (pNodeAnim) {
			//Interpolate scalling and generate scaling transformation matrix
			aiVector3D Scaling;
			CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
			glm::mat4 ScalingM = glm::mat4(1.0f);

			ScalingM = glm::scale(ScalingM, glm::vec3(Scaling.x, Scaling.y, Scaling.z));

			//Interpolate rotation and generate rotation transformation matrix
			aiQuaternion RotationQ;
			CalcInterpolatedRotaion(RotationQ, AnimationTime, pNodeAnim);
			aiMatrix3x3 tp = RotationQ.GetMatrix();
			glm::mat4 RotationM = glm::transpose(glm::make_mat3(&tp.a1));

			//Interpolate translation and generate translation transformation matrix
			aiVector3D Translation;
			CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);

			glm::mat4 TranslationM = glm::mat4(1.0f);
			TranslationM = glm::translate(TranslationM, glm::vec3(Translation.x, Translation.y, Translation.z));

			NodeTransformation = TranslationM * RotationM * ScalingM;

		}

		glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

		BoneInfo* boneInfo = nullptr;
		boneInfo=get_BoneByName(meshes, NodeName);
		if (boneInfo) {
			boneInfo->finalTransformation = GlobalTransformation* boneInfo->offset;
		}

		for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
			SetBoneTransform(pAnimation, AnimationTime, meshes,pNode->mChildren[i], GlobalTransformation);
		}
	}
private:

	static	aiNodeAnim * get_aiNodeAnimBName(const aiAnimation & pAnimation, string nodeName) {
		for (unsigned int i = 0; i < pAnimation.mNumChannels; i++) {
			if (string(pAnimation.mChannels[i]->mNodeName.data)== nodeName) {
				return pAnimation.mChannels[i];
			}
		}
		return NULL;
	}

	static	BoneInfo* get_BoneByName(vector<Mesh>& meshes, string nodeName) {
		for (unsigned int i = 0; i < meshes.size(); i++) {
			for (unsigned int j = 0; j < meshes[i].bones.size(); j++) {
				if (meshes[i].bones[j].mName == nodeName) {
					return &(meshes[i].bones[j]);
				}
			}
		}
		return NULL;
	}

	static	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		if (pNodeAnim->mNumScalingKeys == 1) {
			Out = pNodeAnim->mScalingKeys[0].mValue;
			return;
		}

		unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
		unsigned int NextScalingIndex = (ScalingIndex + 1);
		assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
		float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
		const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}

	static	void CalcInterpolatedRotaion(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		// we need at least two values to interpolate...
		if (pNodeAnim->mNumRotationKeys == 1) {
			Out = pNodeAnim->mRotationKeys[0].mValue;
			return;
		}

		unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
		unsigned int NextRotationIndex = (RotationIndex + 1);
		assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
		float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
		const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
		aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
		Out = Out.Normalize();
	}

	static	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		if (pNodeAnim->mNumPositionKeys == 1) {
			Out = pNodeAnim->mPositionKeys[0].mValue;
			return;
		}

		unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
		unsigned int NextPositionIndex = (PositionIndex + 1);
		assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
		float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
		const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}

	static	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		assert(pNodeAnim->mNumScalingKeys > 0);
		for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
			if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
				return i;
			}
		}
		assert(0);
		return 0;
	}

	static	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		assert(pNodeAnim->mNumRotationKeys > 0);

		for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
			if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
				return i;
			}
		}

		assert(0);
		return 0;
	}

	static	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
			if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
				return i;
			}
		}
		assert(0);
		return 0;
	}

};