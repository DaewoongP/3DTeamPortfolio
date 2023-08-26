#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_iNumChannels(rhs.m_iNumChannels)
	, m_Channels(rhs.m_Channels)
	, m_ChannelCurrentKeyFrames(rhs.m_ChannelCurrentKeyFrames)
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

HRESULT CAnimation::Initialize(Engine::ANIMATION* pAnimation, const CModel::BONES& Bones)
{
	m_isLoop = true;

	// �ִϸ��̼� ���� ����
	lstrcpy(m_szName, pAnimation->szName);
	m_fDuration = pAnimation->fDuration;

	m_fOriginTickPerSecond = pAnimation->fTickPerSecond;
	m_fTickPerSecond = pAnimation->fTickPerSecond;
	
	m_iNumChannels = pAnimation->iNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(pAnimation->Channels[i], Bones);

		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	m_ChannelCurrentKeyFrames.resize(m_iNumChannels);

	_uint iNumKeyFrame = { 0 };
	_uint iChannelIndex = { 0 };
	// ä���� ��ȸ�ϸ鼭 ��� ä���� ���� ū Ű�������� ã�� �ش� ä���� �ε����� �ش� �������� ����.
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

	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(CModel::BONES& Bones, _float fTimeDelta)
{
	m_fTimeAcc += m_fTickPerSecond * fTimeDelta;

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

CAnimation* CAnimation::Create(Engine::ANIMATION* pAnimation, const CModel::BONES& Bones)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAnimation, Bones)))
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

	m_Channels.clear();

}