#pragma once
/* =============================================== */
// 
//	정 : 안철민
//	부 :
//
/* =============================================== */

#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CNotify final : public CBase
{
private:
	explicit CNotify();
	virtual ~CNotify() = default;

public:
	_uint	Get_NotifyFrameCount() { return m_iNumKeyFrames; }

public:
	HRESULT Initialize();

public:
	void			Invalidate_Frame(_float fTimeAcc, _Inout_ _uint* pCurrentKeyFrameIndex, _Inout_ _float* fSpeed);
	//인덱스를 이용한 검색
	KEYFRAME*		Find_Frame(_uint iFindFrame);
	const _tchar*	Find_Frame_Key(_uint iFindFrame);
	void			Delete_Frame(_uint iFindFrame);
	void			Edit_Frame(_uint iFindFrame, KEYFRAME::KEYFRAMETYPE eFrameType, _float fActionTime, _float fSpeed = 0);
	//태그를 이용한 검색
	KEYFRAME*		Find_Frame(const wchar_t* wszKeyFrameTag);
	//추가(툴용)
	HRESULT			AddFrame(KEYFRAME::KEYFRAMETYPE eFrameType, wchar_t* wszNotifyTag, _float fActionTime, _float fSpeed);

private:
	vector<pair<wstring, KEYFRAME*>>	m_KeyFrames;
	_uint				m_iNumKeyFrames = { 0 };

public:
	static CNotify* Create();
	virtual void Free() override;
};

END