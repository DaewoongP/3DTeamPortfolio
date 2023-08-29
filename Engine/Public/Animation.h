#pragma once
#include "Model.h"

BEGIN(Engine)

class CAnimation final : public CBase
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
	NOTIFYFRAME* Find_NotifyFrame(const wchar_t* wszNotifyTag);
	SOUNDFRAME* Find_SoundFrame(const wchar_t* wszSoundTag);


public:
	HRESULT Initialize(Engine::ANIMATION Animation, const CModel::BONES& Bones);
	void Invalidate_TransformationMatrix(CModel::BONES& Bones, _float fTimeDelta);
	void Invalidate_Speed(_float fTimeDelta);
	void Invalidate_Notify(_float fTimeDelta);
	void Invalidate_Frame(_float fTimeDelta);
private:
	_tchar						m_szName[MAX_STR] = TEXT("");
	// �ִϸ��̼��� ����ϴ� ä��(��)�� ����
	_uint						m_iNumChannels = { 0 };
	// ä���� ����ִ� ���� �����̳�
	vector<class CChannel*>		m_Channels;
	// ��Ƽ����(�˶�,����,�浹ü,��ƼŬ,�ӵ� ����� ä�� �����̳�)
	class CNotify*				m_Notify;
	// �� ä���� ���� Ű������ �ε���
	vector<_uint>				m_ChannelCurrentKeyFrames;

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