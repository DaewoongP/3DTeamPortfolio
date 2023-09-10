#pragma once
/* =============================================== */
// 
//	�� : ��ö��
//	�� : �ּ�ȯ
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
	_bool Get_RootAnim_State() { return m_isRootAnim; }
	_float Get_Ratio_Accumulation_Duration() { return(m_fTimeAcc /m_fDuration); }
	_float Get_Accmulation() { return m_fTimeAcc; }
	_float Get_Duration() { return m_fDuration; }
	_float* Get_Accmulation_Pointer() { return &m_fTimeAcc; }
	_float  Get_TickPerSecond() { return m_fTickPerSecond; }
	_uint  Get_NumChannels() {	return m_iNumChannels;}
	_uint  Get_NumNotify() { return m_iNumNorify; }
	_bool Get_RootAnim() { return m_isRootAnim; }
	_bool Get_LerpAnim() { return m_isLerp; }
	_bool Get_LoopAnim() { return m_isLoop; }
	_bool* Get_RootAnim_Point() { return &m_isRootAnim; }
	_bool* Get_LerpAnim_Point() { return &m_isLerp; }
	_bool* Get_LoopAnim_Point() { return &m_isLoop; }
	vector<class CChannel*>& Get_ChannelVector_Point() { return m_Channels; }
	class CNotify* Get_Notify_Point() { return m_pNotify; }
	_float3 Get_OffsetPosition() { return m_vOffsetPosition; }
	_bool	sfadsfsdf() { return (m_fDuration < m_fTimeAcc); }


	void Set_AnimationName(_tchar* tag) { lstrcpy(m_szName, tag); }
	void Set_CurrentKeyFrameIndex(CModel::BONES& Bones, _uint iKeyFrameIndex);
	void Set_Pause(_bool isPause) { m_isPaused = isPause; }
	void Set_RootAnim(_bool isRootAnim) { m_isRootAnim = isRootAnim; }
	void Set_LerpAnim(_bool isLerp) { m_isLerp = isLerp; }
	void Set_LoopAnim(_bool isLoop) { m_isLoop = isLoop; }
	void Set_TickPerSecond(_float fMultiply)
	{
		if (0 >= fMultiply)
			return;

		m_fTickPerSecond = m_fOriginTickPerSecond * fMultiply;
	}
	void Set_Loop(_bool isLoop) { m_isLoop = isLoop; }
	void Set_OffsetPosition(float* pos) { m_vOffsetPosition.x = pos[0]; m_vOffsetPosition.y = pos[1]; m_vOffsetPosition.z = pos[2];}

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
	KEYFRAME*	Find_NotifyFrame(const _tchar* wszNotifyTag);
	HRESULT		 Add_NotifyFrame(KEYFRAME_GCM* data);
	void		 Update_KeyFrame_By_Time();

public:
	HRESULT Initialize(Engine::ANIMATION Animation, const CModel::BONES& Bones);
	HRESULT Initialize(Engine::ANIMATION_GCM Animation, const CModel::BONES& Bones);
	/* 
		���� �������� �ִϸ��̼��� �������� ������ Ȯ�� 
		���� �ִϸ��̼��� ��� ��ȯ���� �׻� false �̴�.
		�����ִϸ��̼��� �ƴѰ�� TimeAcc�� duration�� �Ѿ ��� true�� ��ȯ�Ѵ�.
	*/
	_bool Invalidate_AccTime(_float fTimeDelta); 
	void Invalidate_TransformationMatrix(CModel::BONES& Bones, _float fTimeDelta, vector<_uint>* boneVec = nullptr);
	void Invalidate_TransformationMatrix_Lerp(CModel::BONES& Bones, _float fTimeDelta, _float LerpTimeAcc, _uint iRootIndex, vector<_uint>* boneVec = nullptr);
	void Invalidate_Frame(_float fTimeDelta);

private:
	_tchar						m_szName[MAX_STR] = TEXT("");
	// �ִϸ��̼��� ����ϴ� ä��(��)�� ����
	_uint						m_iNumChannels = { 0 };
	_uint						m_iNumNorify = { 0 };
	// ä���� ����ִ� ���� �����̳�
	vector<class CChannel*>		m_Channels;
	// ��Ƽ����(�˶�,����,�浹ü,��ƼŬ,�ӵ� ����� ä�� �����̳�)
	class CNotify*				m_pNotify = { nullptr };
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
	_bool						m_isRootAnim = { true };
	_bool						m_isLerp = { true };

	_float3						m_vOffsetPosition = {};

	// ���� �ִϸ��̼ǿ� �ش��ϴ� ä���� ������ �� ���� �������� ���� ä���� �ε���
	_uint						m_iMaxFrameChannelIndex = { 0 };
	// �ִϸ��̼��� �ִ� ������ ����
	_uint						m_iAnimationFrames = { 0 };

public:
	static CAnimation* Create(Engine::ANIMATION Animation, const CModel::BONES& Bones);
	static CAnimation* Create(Engine::ANIMATION_GCM Animation, const CModel::BONES& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END