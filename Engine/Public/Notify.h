#pragma once
/* =============================================== */
// 
//	정 : 안철민
//	부 :
//
/* =============================================== */

#include "Model.h"

// 노티파이 함수 추가 예시
// static_cast<NOTIFYFRAME*>(m_pModel->Get_Animation()->Find_NotifyFrame(TEXT("hihi")))->Action = [&] {(*this).NotifyTest(); };


BEGIN(Engine)

class ENGINE_DLL CNotify final : public CBase
{
private:
	explicit CNotify();
	explicit CNotify(const CNotify& rhs);
	virtual ~CNotify() = default;

public:
	_uint	Get_NotifyFrameCount() { return m_iNumKeyFrames; }
	vector<pair<wstring, KEYFRAME*>>* Get_NotifyFrame() { return &m_KeyFrames; }
	void Reset();

public:
	HRESULT Initialize();

public:
	void			Invalidate_Frame(_float fTimeAcc, _Inout_ _uint * pCurrentKeyFrameIndex, _Inout_ _float * fSpeed, const _float4x4 * pWorldMatrix, _float4x4 PivotMatrix);
	//인덱스를 이용한 검색
	KEYFRAME* Find_Frame(_uint iFindFrame);
	const _tchar* Find_Frame_Key(_uint iFindFrame);
	void			Delete_Frame(_uint iFindFrame);
	void			Edit_Frame(_uint iFindFrame, KEYFRAME::KEYFRAMETYPE eFrameType, _float fActionTime, _float fSpeed, _char * szActionTag, _int iBondIndex, _float4x4 OffsetMatrix, const CModel::BONES & Bones);
	//태그를 이용한 검색
	KEYFRAME* Find_Frame(const wchar_t* wszKeyFrameTag);
	//추가(툴용)
	HRESULT			AddFrame(KEYFRAME_GCM * data, const CModel::BONES & Bones);

	void			BindBoneMatrixForParticle(const CModel::BONES & Bones);

public:
	void			Notify_NULL_WarningAlam();

private:
	SPEEDFRAME*			m_pPrevSpeedFrame = { nullptr };
	SPEEDFRAME*			m_pCurrentSpeedFrame = { nullptr };
	_uint				m_iPrevSpeedFramesIndex = { 999 };
	_uint				m_iCurrentSpeedFramesIndex = { 999 };
	_bool				m_isFinalSpeedFrame = { false };

	vector<pair<wstring, KEYFRAME*>>	m_KeyFrames;
	_uint				m_iNumKeyFrames = { 0 };
	_uint				m_iCurrentKeyFramesIndex = { 0 };

private:
	void Find_SpeedFrame();

public:
	static CNotify* Create();
	CNotify* Clone();
	virtual void Free() override;
};

END