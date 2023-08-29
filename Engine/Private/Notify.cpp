#include "Notify.h"
#include "Bone.h"

CNotify::CNotify()
{
}

HRESULT CNotify::Initialize()
{
	return S_OK;
}

void CNotify::Invalidate_Frame(_float fTimeAcc, _uint* pCurrentKeyFrameIndex, _float* fSpeed)
{
	if (0.f == fTimeAcc)
	{
		*pCurrentKeyFrameIndex = 0;
		//모든 키프레임의 활성화를 true로 바까줌
		for (auto frame : m_KeyFrames)
		{
			(frame.second).isEnable = true;
		}
	}

	while (fTimeAcc >= Find_Frame((*pCurrentKeyFrameIndex) + 1)->fTime)
		++(*pCurrentKeyFrameIndex);
	KEYFRAME* pKeyFrame = Find_Frame((*pCurrentKeyFrameIndex));
	if ((*pCurrentKeyFrameIndex) < m_iNumKeyFrames &&
		pKeyFrame != nullptr &&	pKeyFrame->isEnable)
	{
		pKeyFrame->isEnable = false;
		switch (pKeyFrame->eKeyFrameType)
		{
		case KEYFRAME::KF_NOTIFY:
			break;
		case KEYFRAME::KF_SOUND:
			break;
		case KEYFRAME::KF_SPEED:
			*fSpeed = static_cast<SPEEDFRAME*>(pKeyFrame)->fSpeed;
			break;
		}
	}
}

KEYFRAME* CNotify::Find_Frame(_uint iFindFrame)
{
	auto iter = find_if(m_KeyFrames.begin(), m_KeyFrames.end(), [&](auto pValue) {
		_uint iCurrentIndex = 0;
		return ((iCurrentIndex++) == iFindFrame);
		});

	if (iter == m_KeyFrames.end())
	{
		return nullptr;
	}

	return &(iter->second);
}

KEYFRAME* CNotify::Find_Frame(const wchar_t* wszKeyFrameTag)
{
	auto iter = find_if(m_KeyFrames.begin(), m_KeyFrames.end(), CTag_Finder(wszKeyFrameTag));

	if (iter == m_KeyFrames.end())
	{
		return nullptr;
	}

	return &(iter->second);
}

HRESULT CNotify::AddFrame(KEYFRAME::KEYFRAMETYPE eFrameType, wchar_t* wszSoundFrameTag, _float fSpeed)
{
	return E_NOTIMPL;
}

CNotify* CNotify::Create()
{
	CNotify* pInstance = new CNotify();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CNotify");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNotify::Free()
{
	_int a = 0;
}
