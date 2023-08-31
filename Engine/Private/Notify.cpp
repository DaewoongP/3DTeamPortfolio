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
		//��� Ű�������� Ȱ��ȭ�� true�� �ٱ���
		for (auto frame : m_KeyFrames)
		{
			(frame.second)->isEnable = true;
		}
	}
	
	//�ɷ������� ���� ������ݽô�.
	//�� Ű�������� 0�� �ƴ϶��?
	if (m_iNumKeyFrames == 0)
		return;
	//���� �ð����� �� Ű�����Ӻ��� ���� ���ִٸ�?
	while (fTimeAcc >= Find_Frame((*pCurrentKeyFrameIndex))->fTime)
	{
		if (m_iNumKeyFrames < (*pCurrentKeyFrameIndex)+1)
			break;

		//���� ���������� �ȺҸ��µ�.
		KEYFRAME* pKeyFrame = Find_Frame((*pCurrentKeyFrameIndex));
		if ((*pCurrentKeyFrameIndex) < m_iNumKeyFrames &&
			pKeyFrame != nullptr && pKeyFrame->isEnable)
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
		if (m_iNumKeyFrames == (*pCurrentKeyFrameIndex) + 1)
			break;
		(*pCurrentKeyFrameIndex)++;
	}
	//�̰� ������ ���������� �̰� ����Ǵ����?
	//if (fTimeAcc <= Find_Frame((*pCurrentKeyFrameIndex))->fTime)
	//	return;

	
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
	m_KeyFrames.erase(iter);
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

HRESULT CNotify::AddFrame(KEYFRAME::KEYFRAMETYPE eFrameType, wchar_t* wszNotifyTag, _float fActionTime, _float fSpeed)
{
	KEYFRAME* keyFrameDesc;
	switch (eFrameType)
	{
	case KEYFRAME::KF_SPEED:
	{
		SPEEDFRAME* speedFrameDesc = new SPEEDFRAME();
		speedFrameDesc->eKeyFrameType = KEYFRAME::KF_SPEED;
		speedFrameDesc->fTime = fActionTime;
		speedFrameDesc->fSpeed = fSpeed;
		speedFrameDesc->isEnable = true;
		keyFrameDesc = speedFrameDesc;
		wstring str = wszNotifyTag;
		pair< wstring, KEYFRAME*> NewPair = pair<wstring, KEYFRAME*>(str, keyFrameDesc);
		m_KeyFrames.push_back(NewPair);
		break;
	}
	case KEYFRAME::KF_NOTIFY:
	{
		NOTIFYFRAME* notifyFrameDesc = new NOTIFYFRAME();
		notifyFrameDesc->eKeyFrameType = KEYFRAME::KF_NOTIFY;
		notifyFrameDesc->fTime = fActionTime;
		notifyFrameDesc->isEnable = true;
		keyFrameDesc = notifyFrameDesc;
		wstring str = wszNotifyTag;
		pair< wstring, KEYFRAME*> NewPair = pair<wstring, KEYFRAME*>(str, keyFrameDesc);
		m_KeyFrames.push_back(NewPair);
		break;
	}
	case KEYFRAME::KF_SOUND:
	{
		SOUNDFRAME* soundFrameDesc = new SOUNDFRAME();
		soundFrameDesc->eKeyFrameType = KEYFRAME::KF_SOUND;
		soundFrameDesc->fTime = fActionTime;
		soundFrameDesc->isEnable = true;
		keyFrameDesc = soundFrameDesc;
		wstring str = wszNotifyTag;
		pair< wstring, KEYFRAME*> NewPair = pair<wstring, KEYFRAME*>(str, keyFrameDesc);
		m_KeyFrames.push_back(NewPair);
		break;
	}
	default:
		MSG_BOX("Failed Create Notify");
		break;
	}
	//������� fTime������� ������ ���ݽô�.
	m_iNumKeyFrames++;

	sort(m_KeyFrames.begin(), m_KeyFrames.end(), [&](std::pair<wstring, KEYFRAME*>& frame1, std::pair<wstring, KEYFRAME*>& frame2) ->  bool {
		return (frame1.second->fTime < frame2.second->fTime);
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
	for (int i = 0; i < m_KeyFrames.size(); i++)
	{
		Safe_Delete(m_KeyFrames[i].second);
	}
}
