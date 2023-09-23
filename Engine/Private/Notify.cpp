#include "Notify.h"
#include "Bone.h"

CNotify::CNotify()
{
}

HRESULT CNotify::Initialize()
{
	return S_OK;
}

void CNotify::Invalidate_Frame(_float fTimeAcc, _Inout_ _uint* pCurrentKeyFrameIndex, _Inout_ _float* fSpeed)
{
	if (0.f == fTimeAcc)
	{
		*pCurrentKeyFrameIndex = 0;
		//모든 키프레임의 활성화를 true로 바까줌
		for (auto frame : m_KeyFrames)
		{
			(frame.second)->isEnable = true;
		}
	}

	//걸러져야할 때를 명시해줍시다.
	//내 키프레임이 0이 아니라면?
	if (m_iNumKeyFrames == 0|| (*pCurrentKeyFrameIndex)+1> m_iNumKeyFrames)
		return;
	KEYFRAME* pKeyFrame = Find_Frame((*pCurrentKeyFrameIndex));

	if (pKeyFrame->eKeyFrameType == KEYFRAME::KF_SPEED)
	{
		while (fTimeAcc >= Find_Frame((*pCurrentKeyFrameIndex))->fTime)
		{
			if (m_iNumKeyFrames <= (*pCurrentKeyFrameIndex) + 1)
				return;
			++(*pCurrentKeyFrameIndex);
		}

		_float		fRatio = 0;
		if ((*pCurrentKeyFrameIndex) == 0)
		{
			fRatio = (fTimeAcc - 0) /
				(Find_Frame((*pCurrentKeyFrameIndex))->fTime - 0);
		}
		else
		{
			fRatio = (fTimeAcc - Find_Frame((*pCurrentKeyFrameIndex-1))->fTime) /
				(Find_Frame((*pCurrentKeyFrameIndex))->fTime - Find_Frame((*pCurrentKeyFrameIndex-1))->fTime);
		}

		*fSpeed = Lerp(*fSpeed, static_cast<SPEEDFRAME*>(pKeyFrame)->fSpeed, fRatio);
	}
	else
	{
		while (fTimeAcc >= Find_Frame((*pCurrentKeyFrameIndex))->fTime)
		{
			if (m_iNumKeyFrames < (*pCurrentKeyFrameIndex) + 1)
				break;

			KEYFRAME* pKeyFrame = Find_Frame((*pCurrentKeyFrameIndex));
			if ((*pCurrentKeyFrameIndex) < m_iNumKeyFrames &&
				pKeyFrame != nullptr && pKeyFrame->isEnable)
			{
				pKeyFrame->isEnable = false;
				switch (pKeyFrame->eKeyFrameType)
				{
				case KEYFRAME::KF_NOTIFY:
				{
					m_iCurrentKeyFramesIndex = (*pCurrentKeyFrameIndex);
					static_cast<NOTIFYFRAME*>(pKeyFrame)->Action();
				}
					
					break;
				case KEYFRAME::KF_SOUND:
					break;
				}
			}
			if (m_iNumKeyFrames == (*pCurrentKeyFrameIndex) + 1)
				break;
			(*pCurrentKeyFrameIndex)++;
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
	return (iter->second);
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

void CNotify::Delete_Frame(_uint iFindFrame)
{
	_uint iCurrentIndex = 0;
	auto iter = find_if(m_KeyFrames.begin(), m_KeyFrames.end(), [&](auto pValue) {
		return ((iCurrentIndex++) == iFindFrame);
		});
	if (iter == m_KeyFrames.end())
	{
		return;
	}
	Safe_Delete(iter->second);
	m_KeyFrames.erase(iter);
	//키프레임이 중간에 삭제되면 애니메이션의 키프레임도 초기화해줘야함.
	m_iNumKeyFrames--;
}

void CNotify::Edit_Frame(_uint iFindFrame, KEYFRAME::KEYFRAMETYPE eFrameType, _float fActionTime, _float fSpeed)
{
	KEYFRAME* pKeyFrame = Find_Frame(iFindFrame);

	pKeyFrame->eKeyFrameType = eFrameType;
	pKeyFrame->fTime = fActionTime;
	if (eFrameType == KEYFRAME::KF_SPEED)
	{
		static_cast<SPEEDFRAME*>(pKeyFrame)->fSpeed = fSpeed;
	}

	//정렬
	sort(m_KeyFrames.begin(), m_KeyFrames.end(), [&](std::pair<wstring, KEYFRAME*>& frame1, std::pair<wstring, KEYFRAME*>& frame2) ->  bool {
		return (frame1.second->fTime < frame2.second->fTime);
		});
}

KEYFRAME* CNotify::Find_Frame(const wchar_t* wszKeyFrameTag)
{
	auto iter = find_if(m_KeyFrames.begin(), m_KeyFrames.end(), [&](pair<wstring, KEYFRAME*> Frame) {
		return (!lstrcmp(Frame.first.c_str(), wszKeyFrameTag));
	});
	if (iter == m_KeyFrames.end())
	{
		return nullptr;
	}
	return (iter->second);

	return nullptr;
}

HRESULT CNotify::AddFrame(KEYFRAME_GCM* data)
{
	KEYFRAME* keyFrameDesc;
	switch (data->eKeyFrameType)
	{
	case KEYFRAME::KF_SPEED:
	{
		SPEEDFRAME* speedFrameDesc = New SPEEDFRAME();
		speedFrameDesc->eKeyFrameType = KEYFRAME::KF_SPEED;
		speedFrameDesc->fTime = data->fTime;
		speedFrameDesc->fSpeed = reinterpret_cast<SPEEDFRAME_GCM*>(data)->fSpeed;
		speedFrameDesc->isEnable = true;
		keyFrameDesc = speedFrameDesc;
		wstring str = data->szName;
		pair< wstring, KEYFRAME*> NewPair = pair<wstring, KEYFRAME*>(str, keyFrameDesc);
		m_KeyFrames.push_back(NewPair);
		break;
	}
	case KEYFRAME::KF_NOTIFY:
	{
		NOTIFYFRAME* notifyFrameDesc = New NOTIFYFRAME();
		notifyFrameDesc->eKeyFrameType = KEYFRAME::KF_NOTIFY;
		notifyFrameDesc->fTime = data->fTime;
		notifyFrameDesc->isEnable = true;
		notifyFrameDesc->Action = [&] {(*this).Notify_NULL_WarningAlam(); };
		keyFrameDesc = notifyFrameDesc;
		wstring str = data->szName;
		pair< wstring, KEYFRAME*> NewPair = pair<wstring, KEYFRAME*>(str, keyFrameDesc);
		m_KeyFrames.push_back(NewPair);
		break;
	}
	case KEYFRAME::KF_SOUND:
	{
		SOUNDFRAME* soundFrameDesc = New SOUNDFRAME();
		soundFrameDesc->eKeyFrameType = KEYFRAME::KF_SOUND;
		soundFrameDesc->fTime = data->fTime;
		soundFrameDesc->isEnable = true;
		keyFrameDesc = soundFrameDesc;
		wstring str = data->szName;
		pair< wstring, KEYFRAME*> NewPair = pair<wstring, KEYFRAME*>(str, keyFrameDesc);
		m_KeyFrames.push_back(NewPair);
		break;
	}
	default:
		MSG_BOX("Failed Create Notify");
		break;
	}
	//순서대로 fTime순서대로 정렬함 해줍시다.
	m_iNumKeyFrames++;

	sort(m_KeyFrames.begin(), m_KeyFrames.end(), [&](std::pair<wstring, KEYFRAME*>& frame1, std::pair<wstring, KEYFRAME*>& frame2) ->  bool {
		return (frame1.second->fTime < frame2.second->fTime);
		});
	return S_OK;
}
void CNotify::Notify_NULL_WarningAlam()
{
	_char szName[MAX_PATH]="";

	WCharToChar(m_KeyFrames[m_iCurrentKeyFramesIndex].first.c_str(), szName);

	cout << szName << " : Please Input Notify Action" << endl;
}
CNotify* CNotify::Create()
{
	CNotify* pInstance = New CNotify();
	if (FAILED(pInstance->Initialize()))

	{
		MSG_BOX("Failed to Created CNotify");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNotify::Free()
{
	for (int i = 0; i < m_KeyFrames.size(); i++)
	{
		Safe_Delete(m_KeyFrames[i].second);
	}
}
