#include "Framework.h"
#include "SkeletalMeshClip.h"



SkeletalMeshKeyframe* SkeletalMeshClip::Keyframe(wstring bonename)
{
	
	if (keyframeMap.count(name) < 1)
		return nullptr;


	return keyframeMap[bonename];
}
