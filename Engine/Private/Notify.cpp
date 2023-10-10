#include "Notify.h"
#include "Bone.h"
#include "GameInstance.h"

CNotify::CNotify()
{
}

CNotify::CNotify(const CNotify& rhs)
	: m_iNumKeyFrames(rhs.m_iNumKeyFrames)
	, m_iCurrentKeyFramesIndex(rhs.m_iCurrentKeyFramesIndex)
{
	KEYFRAME* pKeyframe = nullptr;
	for (auto Pair : rhs.m_KeyFrames)
	{
		switch (Pair.second->eKeyFrameType)
		{	
		case KEYFRAME::KF_SPEED :
			pKeyframe = New SPEEDFRAME(*static_cast<SPEEDFRAME*>(Pair.second));
			break;
		case KEYFRAME::KF_SOUND:
			pKeyframe = New SOUNDFRAME(*static_cast<SOUNDFRAME*>(Pair.second));
			break;
		case KEYFRAME::KF_NOTIFY:
			pKeyframe = New NOTIFYFRAME(*static_cast<NOTIFYFRAME*>(Pair.second));
			break;
		case KEYFRAME::KF_PARTICLE:
			//������� ��� bones�� �����ؼ� matrix�� ���� ���ε��������.
			pKeyframe = New PARTICLEFRAME(*static_cast<PARTICLEFRAME*>(Pair.second));
			break;
		}
		m_KeyFrames.push_back({ Pair.first,pKeyframe });
	}
}

HRESULT CNotify::Initialize()
{
	return S_OK;
}

void CNotify::Invalidate_Frame(_float fTimeAcc, _Inout_ _uint* pCurrentKeyFrameIndex, _Inout_ _float* fSpeed, const _float4x4* pWorldMatrix, _float4x4 PivotMatrix)
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
					break;
				}
				case KEYFRAME::KF_SOUND:
				{
					CGameInstance* pGameInstance = CGameInstance::GetInstance();
					Safe_AddRef(pGameInstance);
					Safe_Release(pGameInstance);
					//m_iCurrentKeyFramesIndex = (*pCurrentKeyFrameIndex);
					//static_cast<SOUNDFRAME*>(pKeyFrame)->Action();
					break;
				}
				case KEYFRAME::KF_PARTICLE:
				{
					m_iCurrentKeyFramesIndex = (*pCurrentKeyFrameIndex);
					CGameInstance* pGameInstance = CGameInstance::GetInstance();
					Safe_AddRef(pGameInstance);
					//���� ���� ���������.
					_float4x4 PositionMatrix = {};

					PARTICLEFRAME* pParticleFrame = static_cast<PARTICLEFRAME*>(pKeyFrame);
					//������ �ε��Ҷ� ������ ��Ʈ������ �ǹ� �����ְ�
					//index�� �̿��� bindbonematrix ����������.
					if (pParticleFrame->BindBoneMatrix == nullptr)
					{
						pGameInstance->Play_Particle((pParticleFrame->wszParticleTag), _float3(0,0,0));
						cout << "Please Bind Bone to ParticleNotify :" << Find_Frame_Key((*pCurrentKeyFrameIndex)) <<"\n";
					}
					else 
					{
						//���⿡ pivot matrix �����ֱ� �� �����;��ϳ�?
						PositionMatrix = pParticleFrame->OffsetMatrix * (*pParticleFrame->BindBoneMatrix)  * PivotMatrix * (*pWorldMatrix);

						pGameInstance->Play_Particle((pParticleFrame->wszParticleTag), pParticleFrame->OffsetMatrix, pParticleFrame->BindBoneMatrix, PivotMatrix, pWorldMatrix);
					}
					
					Safe_Release(pGameInstance);
					break;
				}
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
	//Ű�������� �߰��� �����Ǹ� �ִϸ��̼��� Ű�����ӵ� �ʱ�ȭ�������.
	m_iNumKeyFrames--;
}

void CNotify::Edit_Frame(_uint iFindFrame, KEYFRAME::KEYFRAMETYPE eFrameType, _float fActionTime, _float fSpeed,_char* szActionTag,_int iBondIndex,_float4x4 OffsetMatrix, const CModel::BONES& Bones)
{
	KEYFRAME* pKeyFrame = Find_Frame(iFindFrame);

	pKeyFrame->eKeyFrameType = eFrameType;
	pKeyFrame->fTime = fActionTime;
	if (eFrameType == KEYFRAME::KF_SPEED)
	{
		static_cast<SPEEDFRAME*>(pKeyFrame)->fSpeed = fSpeed;
	}

	if (eFrameType == KEYFRAME::KF_PARTICLE)
	{
		CharToWChar(szActionTag, static_cast<PARTICLEFRAME*>(pKeyFrame)->wszParticleTag);
		
		static_cast<PARTICLEFRAME*>(pKeyFrame)->iBoneIndex = iBondIndex;
		static_cast<PARTICLEFRAME*>(pKeyFrame)->OffsetMatrix = OffsetMatrix;
		static_cast<PARTICLEFRAME*>(pKeyFrame)->BindBoneMatrix = Bones[iBondIndex]->Get_CombinedTransformationMatrixPtr();
	}
	
	if (eFrameType == KEYFRAME::KF_SOUND)
	{
		CharToWChar(szActionTag, static_cast<SOUNDFRAME*>(pKeyFrame)->wszSoundTag);
	}

	//����
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

HRESULT CNotify::AddFrame(KEYFRAME_GCM* data,const CModel::BONES& Bones)
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
		lstrcpy(soundFrameDesc->wszSoundTag, reinterpret_cast<SOUNDFRAME_GCM*>(data)->wszSoundTag);
		keyFrameDesc = soundFrameDesc;
		wstring str = data->szName;
		pair< wstring, KEYFRAME*> NewPair = pair<wstring, KEYFRAME*>(str, keyFrameDesc);
		m_KeyFrames.push_back(NewPair);
		break;
	}
	case KEYFRAME::KF_PARTICLE:
	{
		PARTICLEFRAME* particleFrameDesc = New PARTICLEFRAME();
		particleFrameDesc->eKeyFrameType = KEYFRAME::KF_PARTICLE;
		particleFrameDesc->fTime = data->fTime;
		particleFrameDesc->isEnable = true;
		lstrcpy(particleFrameDesc->wszParticleTag, reinterpret_cast<PARTICLEFRAME_GCM*>(data)->wszParticleTag);
		particleFrameDesc->iBoneIndex = reinterpret_cast<PARTICLEFRAME_GCM*>(data)->iBoneIndex;
		particleFrameDesc->OffsetMatrix = reinterpret_cast<PARTICLEFRAME_GCM*>(data)->OffsetMatrix;
		//���⿡ ���� ��� ���� matrix�� ��������ϱ⶧����, �츮�� modelȤ�� bone�� �޾ƿ;���.
		particleFrameDesc->BindBoneMatrix = Bones[reinterpret_cast<PARTICLEFRAME_GCM*>(data)->iBoneIndex]->Get_CombinedTransformationMatrixPtr();
		keyFrameDesc = particleFrameDesc;
		wstring str = data->szName;
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

void CNotify::BindBoneMatrixForParticle(const CModel::BONES& Bones)
{
	for (auto& Pair : m_KeyFrames)
	{
		if (Pair.second->eKeyFrameType == KEYFRAME::KF_PARTICLE)
		{
			static_cast<PARTICLEFRAME*>(Pair.second)->BindBoneMatrix = Bones[static_cast<PARTICLEFRAME*>(Pair.second)->iBoneIndex]->Get_CombinedTransformationMatrixPtr();
		}
	}
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

CNotify* CNotify::Clone()
{
	return New CNotify(*this);
}

void CNotify::Free()
{
	for (int i = 0; i < m_KeyFrames.size(); i++)
	{
		Safe_Delete(m_KeyFrames[i].second);
	}
}
