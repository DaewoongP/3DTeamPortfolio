#include "Notify.h"
#include "Bone.h"
#include "GameInstance.h"

CNotify::CNotify()
{
}

CNotify::CNotify(const CNotify& rhs)
	: m_iNumKeyFrames(rhs.m_iNumKeyFrames)
	, m_iCurrentKeyFramesIndex(rhs.m_iCurrentKeyFramesIndex)
	, m_iPrevSpeedFramesIndex(rhs.m_iPrevSpeedFramesIndex)
	, m_iCurrentSpeedFramesIndex(rhs.m_iCurrentSpeedFramesIndex)
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
			//복사생성 당시 bones에 접근해서 matrix를 새로 바인딩해줘야함.
			pKeyframe = New PARTICLEFRAME(*static_cast<PARTICLEFRAME*>(Pair.second));
			break;
		}
		m_KeyFrames.push_back({ Pair.first,pKeyframe });
	}
}

void CNotify::Reset()
{
	for (auto frame : m_KeyFrames)
	{
		(frame.second)->isEnable = true;
	}
}

HRESULT CNotify::Initialize()
{
	return S_OK;
}

void CNotify::Invalidate_Frame(_float fTimeAcc, _Inout_ _uint* pCurrentKeyFrameIndex, _Inout_ _float* fSpeed, _Inout_ vector<_int>* iSoundChannelVecconst,const _float4x4* pWorldMatrix, _float4x4 PivotMatrix)
{
	if (0.f == fTimeAcc)
	{
		*pCurrentKeyFrameIndex = 0;
		//첫 속도 프레임을 설정해줍니다.
		m_pCurrentSpeedFrame = nullptr;
		m_pPrevSpeedFrame = nullptr;
		m_iCurrentSpeedFramesIndex = 999;
		m_iPrevSpeedFramesIndex = 999;
		m_isFinalSpeedFrame = false;
		Find_SpeedFrame();

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

	//스피드갱신용도
	_float		fRatio = 0;

	if (m_pCurrentSpeedFrame != nullptr)
	{
		//만약 다음 속도 프레임이 없다면?
		if (m_pCurrentSpeedFrame->fTime < fTimeAcc&& !m_isFinalSpeedFrame)
		{
			//새로운 속도 프레임을 설정해줍니다.
			Find_SpeedFrame();
		}
		
		//만약 내 속도 프레임보다 앞질러갔으면?
		if ((m_iPrevSpeedFramesIndex) == 999)
		{
			fRatio = (fTimeAcc - 0) /
				(m_pCurrentSpeedFrame->fTime - 0);
		}
		else
		{
			fRatio = (fTimeAcc - m_pPrevSpeedFrame->fTime) /
				(m_pCurrentSpeedFrame->fTime - m_pPrevSpeedFrame->fTime);
		}

		if (fRatio <= 1&& fRatio >= 0)
		{
			*fSpeed = Lerp(*fSpeed, m_pCurrentSpeedFrame->fSpeed, fRatio);
		}
	}

	while (fTimeAcc > Find_Frame((*pCurrentKeyFrameIndex))->fTime)
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
				SOUNDFRAME* pParticleFrame = static_cast<SOUNDFRAME*>(pKeyFrame);
				_int returnValue = pGameInstance->Play_Sound(pParticleFrame->wszSoundTag, pParticleFrame->fVolum);
				if (returnValue != -1 && iSoundChannelVecconst != nullptr)
				{
					iSoundChannelVecconst->push_back(returnValue);
				}
					
				Safe_Release(pGameInstance);
				break;
			}
			case KEYFRAME::KF_PARTICLE:
			{
				m_iCurrentKeyFramesIndex = (*pCurrentKeyFrameIndex);
				CGameInstance* pGameInstance = CGameInstance::GetInstance();
				Safe_AddRef(pGameInstance);
				//생성 지점 만들어주자.
				_float4x4 PositionMatrix = {};

				PARTICLEFRAME* pParticleFrame = static_cast<PARTICLEFRAME*>(pKeyFrame);
				//데이터 로드할때 오프셋 메트릭스에 피벗 곱해주고
				//index를 이용해 bindbonematrix 만들어줘야함.
				if (pParticleFrame->BindBoneMatrix == nullptr)
				{
					pGameInstance->Play_Particle((pParticleFrame->wszParticleTag), _float3(0,0,0));
					cout << "Please Bind Bone to ParticleNotify :" << Find_Frame_Key((*pCurrentKeyFrameIndex)) <<"\n";
				}
				else 
				{
					//여기에 pivot matrix 곱해주기 모델 가져와야하네?
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

void CNotify::Edit_Frame(_uint iFindFrame, KEYFRAME* pTempFrame)
{
	KEYFRAME* pKeyFrame = Find_Frame(iFindFrame);

	pKeyFrame->eKeyFrameType = pTempFrame->eKeyFrameType;
	pKeyFrame->fTime = pTempFrame->fTime;

	if (pKeyFrame->eKeyFrameType == KEYFRAME::KF_SPEED)
	{
		static_cast<SPEEDFRAME*>(pKeyFrame)->fSpeed = static_cast<SPEEDFRAME*>(pTempFrame)->fSpeed;
	}

	if (pKeyFrame->eKeyFrameType == KEYFRAME::KF_PARTICLE)
	{
		PARTICLEFRAME* pParticleFrameData = static_cast<PARTICLEFRAME*>(pTempFrame);
		lstrcpy(static_cast<PARTICLEFRAME*>(pKeyFrame)->wszParticleTag, pParticleFrameData->wszParticleTag);
		
		static_cast<PARTICLEFRAME*>(pKeyFrame)->iBoneIndex = pParticleFrameData->iBoneIndex;
		static_cast<PARTICLEFRAME*>(pKeyFrame)->OffsetMatrix = pParticleFrameData->OffsetMatrix;
		static_cast<PARTICLEFRAME*>(pKeyFrame)->BindBoneMatrix = pParticleFrameData->BindBoneMatrix;
	}
	
	if (pKeyFrame->eKeyFrameType == KEYFRAME::KF_SOUND)
	{
		SOUNDFRAME* pSoundFrameData = static_cast<SOUNDFRAME*>(pTempFrame);

		lstrcpy(static_cast<SOUNDFRAME*>(pKeyFrame)->wszSoundTag, pSoundFrameData->wszSoundTag);
		static_cast<SOUNDFRAME*>(pKeyFrame)->fVolum = pSoundFrameData->fVolum;
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
		soundFrameDesc->fVolum = reinterpret_cast<SOUNDFRAME_GCM*>(data)->fVolum;
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
		//여기에 모델의 몇번 뼈의 matrix를 던져줘야하기때문에, 우리는 model혹은 bone을 받아와야함.
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
	//순서대로 fTime순서대로 정렬함 해줍시다.
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
void CNotify::Find_SpeedFrame()
{
	//만약 처음 찾는 속도 프레임이면 0부터, 아니라면 기존값부터 검색한다.
	_uint iCurrentIndex = 0;
	if (m_pCurrentSpeedFrame == nullptr)
		iCurrentIndex = 0;
	else
		iCurrentIndex = m_iCurrentSpeedFramesIndex;

	auto iter = find_if(m_KeyFrames.begin() + iCurrentIndex, m_KeyFrames.end(), [&](auto pValue) {
		iCurrentIndex++;
		return (pValue.second->eKeyFrameType == KEYFRAME::KF_SPEED);
		});

	if (iter != m_KeyFrames.end())
	{
		m_iPrevSpeedFramesIndex = m_iCurrentSpeedFramesIndex;
		m_iCurrentSpeedFramesIndex = iCurrentIndex;
		m_pPrevSpeedFrame = m_pCurrentSpeedFrame;
		m_pCurrentSpeedFrame = static_cast<SPEEDFRAME*>(iter->second);
	}
	else 
	{
		m_isFinalSpeedFrame = true;
	}
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
