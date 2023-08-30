#include "..\Public\Animation.h"
#include "Channel.h"
#include "Notify.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_iNumChannels(rhs.m_iNumChannels)
	, m_Channels(rhs.m_Channels)
	, m_pNotify(rhs.m_pNotify)
	, m_ChannelCurrentKeyFrames(rhs.m_ChannelCurrentKeyFrames)
	, m_iNotifyCurrentKeyFrame(rhs.m_iNotifyCurrentKeyFrame)
	, m_fDuration(rhs.m_fDuration)
	, m_fOriginTickPerSecond(rhs.m_fOriginTickPerSecond)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_fTimeAcc(rhs.m_fTimeAcc)
	, m_isLoop(rhs.m_isLoop)
	, m_isPaused(rhs.m_isPaused)
	, m_iMaxFrameChannelIndex(rhs.m_iMaxFrameChannelIndex)
	, m_iAnimationFrames(rhs.m_iAnimationFrames)
{
	lstrcpy(m_szName, rhs.m_szName);

	for (auto& pChannel : m_Channels)
	{
		Safe_AddRef(pChannel);
	}
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

NOTIFYFRAME* CAnimation::Find_NotifyFrame(const _tchar* wszNotifyTag)
{
	return nullptr;
}

SOUNDFRAME* CAnimation::Find_SoundFrame(const _tchar* wszSoundTag)
{
	return nullptr;
}

HRESULT CAnimation::Add_NotifyFrame(KEYFRAME::KEYFRAMETYPE eFrameType, wchar_t* wszNotifyTag, _float fActionTime, _float fSpeed)
{
	if (m_pNotify == nullptr)
		return E_FAIL;
	return m_pNotify->AddFrame(eFrameType, wszNotifyTag, fActionTime, fSpeed);
}

HRESULT CAnimation::Initialize(Engine::ANIMATION Animation, const CModel::BONES& Bones)
{
	m_isLoop = true;

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

void CAnimation::Invalidate_TransformationMatrix(CModel::BONES& Bones, _float fTimeDelta)
{
	if (m_fTimeAcc >= m_fDuration)
	{
		if (true == m_isLoop)
		{
			m_fTimeAcc = 0.f;
		}
	}

	_uint		iChannelIndex = 0;

	for (auto& pChannel : m_Channels)
	{
		if (nullptr == pChannel)
			return;

		pChannel->Invalidate_TransformationMatrix(Bones, m_fTimeAcc, &m_ChannelCurrentKeyFrames[iChannelIndex++]);
	}
}

void CAnimation::Invalidate_Frame(_float fTimeDelta)
{
	if (m_isPaused)
		return;
	m_fTimeAcc += m_fTickPerSecond * fTimeDelta;
	if (m_fTimeAcc >= m_fDuration)
	{
		if (true == m_isLoop)
		{
			m_fTimeAcc = 0.f;
		}
	}
	//첫번째 채널에만 사운드용 데이터를 추가해줄거임.
	_uint		iChannelIndex = 0;
	if (m_pNotify != nullptr)
	{
		m_pNotify->Invalidate_Frame(m_fTimeAcc, &m_ChannelCurrentKeyFrames[iChannelIndex++], &m_fTickPerSecond);
	}
}

CAnimation* CAnimation::Create(Engine::ANIMATION Animation, const CModel::BONES& Bones)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(Animation, Bones)))
	{
		MSG_BOX("Failed to Created CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);
	Safe_Release(m_pNotify);

	m_Channels.clear();
}