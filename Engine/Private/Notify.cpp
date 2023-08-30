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
	
	//마지막 키프레임인 경우 실행 X
	if (m_iNumKeyFrames ==0|| fTimeAcc >= Find_Frame(m_iNumKeyFrames - 1)->fTime)
		return;
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
	_uint iCurrentIndex = 0;
	auto iter = find_if(m_KeyFrames.begin(), m_KeyFrames.end(), [&](auto pValue) {
		return ((iCurrentIndex++) == iFindFrame);
		});

	if (iter == m_KeyFrames.end())
	{
		return nullptr;
	}

	return &(iter->second);
}

const _tchar* CNotify::Find_Frame_Key(_uint iFindFrame)
{
	_uint iCurrentIndex = 0;
	auto iter = find_if(m_KeyFrames.begin(), m_KeyFrames.end(), [&](auto pValue) {
		return ((iCurrentIndex++) == iFindFrame);
		});
	if (iter == m_KeyFrames.end())
	{
		return nullptr;
	}
	return (iter)->first.c_str();
}

KEYFRAME* CNotify::Find_Frame(const wchar_t* wszKeyFrameTag)
{
	auto iter = find_if(m_KeyFrames.begin(), m_KeyFrames.end(), [&](pair<wstring, KEYFRAME> Frame) {
		return (!lstrcmp(Frame.first.c_str(), wszKeyFrameTag));
	});

	if (iter == m_KeyFrames.end())
	{
		return nullptr;
	}

	return &(iter->second);
}

HRESULT CNotify::AddFrame(KEYFRAME::KEYFRAMETYPE eFrameType, wchar_t* wszNotifyTag, _float fActionTime, _float fSpeed)
{
	KEYFRAME keyFrameDesc;
	switch (eFrameType)
	{
	case KEYFRAME::KF_SPEED:
	{
		SPEEDFRAME speedFrameDesc;
		speedFrameDesc.eKeyFrameType = KEYFRAME::KF_SPEED;
		speedFrameDesc.fTime = fActionTime;
		speedFrameDesc.fSpeed = fSpeed;
		speedFrameDesc.isEnable = true;
		keyFrameDesc = speedFrameDesc;
		break;
	}
	case KEYFRAME::KF_NOTIFY:
	{
		NOTIFYFRAME notifyFrameDesc;
		notifyFrameDesc.eKeyFrameType = KEYFRAME::KF_NOTIFY;
		notifyFrameDesc.fTime = fActionTime;
		notifyFrameDesc.isEnable = true;
		keyFrameDesc = notifyFrameDesc;
		break;
	}
	case KEYFRAME::KF_SOUND:
	{
		SOUNDFRAME soundFrameDesc;
		soundFrameDesc.eKeyFrameType = KEYFRAME::KF_SOUND;
		soundFrameDesc.fTime = fActionTime;
		soundFrameDesc.isEnable = true;
		keyFrameDesc = soundFrameDesc;
		break;
	}
	default:
		MSG_BOX("Failed Create Notify");
		break;
	}
	//순서대로 fTime순서대로 정렬함 해줍시다.
	wstring str = wszNotifyTag;
	m_KeyFrames.push_back(pair(str, keyFrameDesc));
	m_iNumKeyFrames++;

	sort(m_KeyFrames.begin(), m_KeyFrames.end(), [&](std::pair<wstring, KEYFRAME>& frame1, std::pair<wstring, KEYFRAME>& frame2) ->  bool {
		return (frame1.second.fTime > frame2.second.fTime);
		});
	return S_OK;
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
