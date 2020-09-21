#include "stdafx.h"
#include "SpriteAnimationController.h"



namespace XYZ {
	void SpriteAnimationController::UpdateSpriteAnimation(SpriteAnimation* anim)
	{
		anim->SetFrameInterval(m_Current->frameInterval.first, m_Current->frameInterval.second, m_Current->frameLen);
	}
}