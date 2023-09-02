#pragma once
/* =============================================== */
// 
//	�� : ��ö��
//	�� :
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
	// ���� �ִϸ��̼� �̸� ��ȯ
	const _tchar* Get_AnimationName() const { return m_szName; }
	// �ִϸ��̼��� ������ ���� ��ȯ
	_uint Get_AnimationFrames() { return m_iAnimationFrames; }
	// ä�� �� Ű������ �ִ�ġ ����, **���� �ִϸ��̼��� ������ ��ȯ**
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
	// �ִϸ��̼��� �ʱ�ȭ �ϴ� �Լ�
	void Reset()
	{
		for (auto& iCurrentKeyFrame : m_ChannelCurrentKeyFrames)
			iCurrentKeyFrame = 0;
		m_fTimeAcc = 0.0;
	}
	void TimeAccReset() { m_fTimeAcc = 0.0; }
	void Delete_Translation();
	void Delete_Rotation();
	
	//�ַ� �׿����� �̴ϼȶ������ ����� Ű������ ã�Ƽ� �������� �Լ�.
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
	// �ִϸ��̼��� ����ϴ� ä��(��)�� ����
	_uint						m_iNumChannels = { 0 };
	// ä���� ����ִ� ���� �����̳�
	vector<class CChannel*>		m_Channels;
	// ��Ƽ����(�˶�,����,�浹ü,��ƼŬ,�ӵ� ����� ä�� �����̳�)
	class CNotify*				m_pNotify = {nullptr};
	// �� ä���� ���� Ű������ �ε���
	vector<_uint>				m_ChannelCurrentKeyFrames;
	// ��Ƽ������ ���� Ű������ �ε���
	_uint						m_iNotifyCurrentKeyFrame = { 0 };

	_float						m_fDuration = { 0.f };
	_float						m_fOriginTickPerSecond = { 0.f };
	_float						m_fTickPerSecond = { 0.f };
	_float						m_fTimeAcc = { 0.f };

	_bool						m_isLoop = { false };
	_bool						m_isPaused = { false };

	// ���� �ִϸ��̼ǿ� �ش��ϴ� ä���� ������ �� ���� �������� ���� ä���� �ε���
	_uint						m_iMaxFrameChannelIndex = { 0 };
	// �ִϸ��̼��� �ִ� ������ ����
	_uint						m_iAnimationFrames = { 0 };

public:
	static CAnimation* Create(Engine::ANIMATION Animation, const CModel::BONES& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END