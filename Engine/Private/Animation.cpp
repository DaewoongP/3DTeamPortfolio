#include "..\Public\Animation.h"
#include "Channel.h"
#include "Notify.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_iNumChannels(rhs.m_iNumChannels)
	, m_iNumNorify(rhs.m_iNumNorify)
	, m_Channels(rhs.m_Channels)
	, m_ChannelCurrentKeyFrames(rhs.m_ChannelCurrentKeyFrames)
	, m_iNotifyCurrentKeyFrame(rhs.m_iNotifyCurrentKeyFrame)
	, m_fDuration(rhs.m_fDuration)
	, m_fOriginTickPerSecond(rhs.m_fOriginTickPerSecond)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_fTimeAcc(rhs.m_fTimeAcc)
	, m_isLoop(rhs.m_isLoop)
	, m_isLerp(rhs.m_isLerp)
	, m_isRootAnim(rhs.m_isRootAnim)
	, m_vOffsetPosition(rhs.m_vOffsetPosition)
	, m_isPaused(rhs.m_isPaused)
	, m_iMaxFrameChannelIndex(rhs.m_iMaxFrameChannelIndex)
	, m_iAnimationFrames(rhs.m_iAnimationFrames)
{
	lstrcpy(m_szName, rhs.m_szName);
	
	for (auto& pChannel : m_Channels)
	{
		Safe_AddRef(pChannel);
	}
	CNotify* pNotify = (rhs.m_pNotify);
	m_pNotify = rhs.m_pNotify->Clone();
}

void CAnimation::Set_CurrentKeyFrameIndex(CModel::BONES& Bones, _uint iKeyFrameIndex)
{
	_uint iChannelIndex = { 0 };

	for (auto& pChannel : m_Channels)
	{
		if (nullptr == pChannel)
			return;
		m_ChannelCurrentKeyFrames[iChannelIndex] = iKeyFrameIndex;

		pChannel->Invalidate_TransformationMatrix(Bones,
			pChannel->Get_CurrentKeyFrameTime(iKeyFrameIndex),
			&m_ChannelCurrentKeyFrames[iChannelIndex]);

		++iChannelIndex;
	}
}

void CAnimation::Delete_Translation()
{
	for (auto& pChannel : m_Channels)
	{
		pChannel->Delete_Translation();
	}
}

void CAnimation::Delete_Rotation()
{
	for (auto& pChannel : m_Channels)
	{
		pChannel->Delete_Rotation();
	}
}

KEYFRAME* CAnimation::Find_NotifyFrame(const _tchar* wszNotifyTag)
{
	return m_pNotify->Find_Frame(wszNotifyTag);
}

HRESULT CAnimation::Add_NotifyFrame(KEYFRAME_GCM* data, CModel::BONES& Bones)
{
	if (m_pNotify == nullptr)
		return E_FAIL;
	m_iNumNorify++;
	return m_pNotify->AddFrame(data, Bones);
}

void CAnimation::Update_KeyFrame_By_Time()
{
	_uint iChannelIndex = { 0 };
	for (auto pChannel : m_Channels)
	{
		m_ChannelCurrentKeyFrames[iChannelIndex] = 0;
		++iChannelIndex;
	}
}

HRESULT CAnimation::Initialize(Engine::ANIMATION Animation, const CModel::BONES& Bones)
{
	// 애니메이션 정보 저장
	lstrcpy(m_szName, Animation.szName);
	m_fDuration = Animation.fDuration;

	m_fOriginTickPerSecond = Animation.fTickPerSecond;
	m_fTickPerSecond = Animation.fTickPerSecond;

	m_iNumChannels = Animation.iNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(Animation.Channels[i], Bones);

		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	m_ChannelCurrentKeyFrames.resize(m_iNumChannels);

	_uint iNumKeyFrame = { 0 };
	_uint iChannelIndex = { 0 };
	// 채널을 순회하면서 모든 채널중 가장 큰 키프레임을 찾아 해당 채널의 인덱스와 해당 프레임을 저장.
	for (auto& pChannel : m_Channels)
	{
		if (pChannel->Get_NumKeyFrames() > iNumKeyFrame)
		{
			iNumKeyFrame = pChannel->Get_NumKeyFrames();
			m_iMaxFrameChannelIndex = iChannelIndex;
		}

		++iChannelIndex;
	}
	m_iAnimationFrames = iNumKeyFrame;

	//노티파이를 생성해줍니다.
	m_pNotify = CNotify::Create();
	return S_OK;
}

HRESULT CAnimation::Initialize(Engine::ANIMATION_GCM Animation, const CModel::BONES& Bones)
{
	// 애니메이션 정보 저장
	lstrcpy(m_szName, Animation.szName);
	m_fDuration = Animation.fDuration;

	m_fOriginTickPerSecond = Animation.fTickPerSecond;
	m_fTickPerSecond = Animation.fTickPerSecond;

	m_isLoop = Animation.isLoop;
	m_isRootAnim = Animation.isRootAnim;
	m_isLerp = Animation.isLerp;

	m_vOffsetPosition = Animation.vOffsetPosition;

	m_iNumChannels = Animation.iNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(Animation.Channels[i], Bones);

		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	m_ChannelCurrentKeyFrames.resize(m_iNumChannels);

	_uint iNumKeyFrame = { 0 };
	_uint iChannelIndex = { 0 };
	// 채널을 순회하면서 모든 채널중 가장 큰 키프레임을 찾아 해당 채널의 인덱스와 해당 프레임을 저장.
	for (auto& pChannel : m_Channels)
	{
		if (pChannel->Get_NumKeyFrames() > iNumKeyFrame)
		{
			iNumKeyFrame = pChannel->Get_NumKeyFrames();
			m_iMaxFrameChannelIndex = iChannelIndex;
		}

		++iChannelIndex;
	}
	m_iAnimationFrames = iNumKeyFrame;

	//노티파이를 생성해줍니다.
	m_pNotify = CNotify::Create();
	m_iNumNorify = Animation.Notify->iNumKeyFrames;

	for (_uint i = 0; i < m_iNumNorify; ++i)
	{

		m_pNotify->AddFrame(Animation.Notify->tKeyFrame[i], Bones);
	}

	return S_OK;
}

_bool CAnimation::Invalidate_AccTime(_float fTimeDelta)
{
	_bool isEnd = false;
	if (m_isPaused)
		return isEnd;
	
	m_fTimeAcc += m_fTickPerSecond * fTimeDelta;
	
	if (m_fTimeAcc >= m_fDuration)
	{
		if (true == m_isLoop)
		{
			isEnd = true;
			m_fTimeAcc = 0.f;
		}
		else 
		{
			m_fTimeAcc = m_fDuration;
		}
		m_fTickPerSecond = 30.f;
	}
	return isEnd;
}

void CAnimation::Invalidate_TransformationMatrix(CModel::BONES& Bones, _float fTimeDelta, vector<_uint>* boneVec)
{
	_uint		iChannelIndex = 0;
	for (auto& pChannel : m_Channels)
	{
		if (nullptr == pChannel)
			return;

		if (boneVec != nullptr)
		{
			_int iFindIndex = pChannel->Get_BoneIndex();
			auto iter = find_if((*boneVec).begin(), (*boneVec).end(), [&](auto data) {
				if (data == iFindIndex)
					return true;
				return false;
				});

			if (iter == (*boneVec).end())
			{
				continue;
			}
		}

		pChannel->Invalidate_TransformationMatrix(Bones, m_fTimeAcc, &m_ChannelCurrentKeyFrames[iChannelIndex++]);
	}
}

void CAnimation::Invalidate_TransformationMatrix_Lerp(CModel::BONES& Bones, _float fTimeDelta, _float LerpTimeAcc, _uint iRootIndex, vector<_uint>* boneVec)
{
	_uint		iChannelIndex = 0;
	for (auto& pChannel : m_Channels)
	{
		if (nullptr == pChannel)
			return;

		if (boneVec != nullptr)
		{
			_int iFindIndex = pChannel->Get_BoneIndex();
			auto iter = find_if((*boneVec).begin(), (*boneVec).end(), [&](auto data) {
				if (data == iFindIndex)
					return true;
				return false;
				});

			if (iter == (*boneVec).end())
			{
				continue;
			}
		}

		if (pChannel->Get_BoneIndex() < iRootIndex+1 && m_isRootAnim)
		{
			pChannel->Invalidate_TransformationMatrix(Bones, m_fTimeAcc, &m_ChannelCurrentKeyFrames[iChannelIndex++]);
		}
		else
			pChannel->Invalidate_TransformationMatrix_Lerp(Bones, m_fTimeAcc, &m_ChannelCurrentKeyFrames[iChannelIndex++], LerpTimeAcc);
	}
}

void CAnimation::Invalidate_Frame(_float fTimeDelta, _float4x4 PivotMatrix,const _float4x4* pWorldMatrix)
{
	if (m_pNotify != nullptr)
	{
		m_pNotify->Invalidate_Frame(m_fTimeAcc, &m_iNotifyCurrentKeyFrame, &m_fTickPerSecond, pWorldMatrix, PivotMatrix);
	}
}

CAnimation* CAnimation::Create(Engine::ANIMATION Animation, const CModel::BONES& Bones)
{
	CAnimation* pInstance = New CAnimation();

	if (FAILED(pInstance->Initialize(Animation, Bones)))
	{
		MSG_BOX("Failed to Created CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation* CAnimation::Create(Engine::ANIMATION_GCM Animation, const CModel::BONES& Bones)
{
	CAnimation* pInstance = New CAnimation();

	if (FAILED(pInstance->Initialize(Animation, Bones)))
	{
		MSG_BOX("Failed to Created CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return New CAnimation(*this);
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	Safe_Release(m_pNotify);

	m_Channels.clear();
}