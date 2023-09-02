#pragma once
/* =============================================== */
// 
//	정 : 개철민
//	부 :
//
/* =============================================== */

#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CAnimation final : public CBase
{
private:
	explicit CAnimation();
	explicit CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	// 현재 애니메이션 이름 반환
	const _tchar* Get_AnimationName() const { return m_szName; }
	// 애니메이션의 프레임 개수 반환
	_uint Get_AnimationFrames() { return m_iAnimationFrames; }
	// 채널 중 키프레임 최대치 기준, **현재 애니메이션의 프레임 반환**
	_uint Get_CurrentAnimationFrame() { return m_ChannelCurrentKeyFrames[m_iMaxFrameChannelIndex]; }
	_bool Get_Paused_State() { return m_isPaused; }
	_float Get_Ratio_Accumulation_Duration() { return(m_fTimeAcc /m_fDuration); }
	_float Get_Accmulation() { return m_fTimeAcc; }
	_float Get_Duration() { return m_fDuration; }
	_float* Get_Accmulation_Pointer() { return &m_fTimeAcc; }
	class CNotify* Get_Notify_Point() { return m_pNotify; }

	void Set_CurrentKeyFrameIndex(CModel::BONES& Bones, _uint iKeyFrameIndex);
	void Set_Pause(_bool isPause) { m_isPaused = isPause; }
	void Set_TickPerSecond(_float fMultiply)
	{
		if (0 >= fMultiply)
			return;

		m_fTickPerSecond = m_fOriginTickPerSecond * fMultiply;
	}
	void Set_Loop(_bool isLoop) { m_isLoop = isLoop; }
	// 애니메이션을 초기화 하는 함수
	void Reset()
	{
		for (auto& iCurrentKeyFrame : m_ChannelCurrentKeyFrames)
			iCurrentKeyFrame = 0;
		m_fTimeAcc = 0.0;
	}
	void TimeAccReset() { m_fTimeAcc = 0.0; }
	void Delete_Translation();
	void Delete_Rotation();
	
	//주로 겜오브제 이니셜라이즈에서 사용할 키프레임 찾아서 가져오는 함수.
	NOTIFYFRAME* Find_NotifyFrame(const _tchar* wszNotifyTag);
	SOUNDFRAME*  Find_SoundFrame(const _tchar* wszSoundTag);
	HRESULT		 Add_NotifyFrame(KEYFRAME::KEYFRAMETYPE eFrameType, wchar_t* wszNotifyTag, _float fActionTime, _float fSpeed = 0);

public:
	HRESULT Initialize(Engine::ANIMATION Animation, const CModel::BONES& Bones);
	_bool Invalidate_AccTime(_float fTimeDelta); 
	void Invalidate_TransformationMatrix(CModel::BONES& Bones, _float fTimeDelta);
	void Invalidate_TransformationMatrix_Lerp(CModel::BONES& Bones, _float fTimeDelta, _float LerpTimeAcc, _uint iRootIndex);
	void Invalidate_Frame(_float fTimeDelta);
private:
	_tchar						m_szName[MAX_STR] = TEXT("");
	// 애니메이션이 사용하는 채널(뼈)의 개수
	_uint						m_iNumChannels = { 0 };
	// 채널을 담고있는 벡터 컨테이너
	vector<class CChannel*>		m_Channels;
	// 노티파이(알람,사운드,충돌체,파티클,속도 재생용 채널 컨테이너)
	class CNotify*				m_pNotify = {nullptr};
	// 각 채널의 현재 키프레임 인덱스
	vector<_uint>				m_ChannelCurrentKeyFrames;
	// 노티파이의 현재 키프레임 인덱스
	_uint						m_iNotifyCurrentKeyFrame = { 0 };

	_float						m_fDuration = { 0.f };
	_float						m_fOriginTickPerSecond = { 0.f };
	_float						m_fTickPerSecond = { 0.f };
	_float						m_fTimeAcc = { 0.f };

	_bool						m_isLoop = { false };
	_bool						m_isPaused = { false };

	// 현재 애니메이션에 해당하는 채널의 프레임 중 가장 프레임이 많은 채널의 인덱스
	_uint						m_iMaxFrameChannelIndex = { 0 };
	// 애니메이션의 최대 프레임 개수
	_uint						m_iAnimationFrames = { 0 };

public:
	static CAnimation* Create(Engine::ANIMATION Animation, const CModel::BONES& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END