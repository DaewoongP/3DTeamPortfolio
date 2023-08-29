#pragma once
#include "Model.h"

BEGIN(Engine)

class CNotify final : public CBase
{
private:
	explicit CNotify();
	virtual ~CNotify() = default;

public:
	HRESULT Initialize();

	void	Invalidate_Frame(_float fTimeAcc, _Inout_ _uint* pCurrentKeyFrameIndex, _Inout_ _float* fSpeed);

	//인덱스를 이용한 검색
	KEYFRAME* Find_Frame(_uint iFindFrame);
	//태그를 이용한 검색
	KEYFRAME* Find_Frame(const wchar_t* wszKeyFrameTag);
	//추가(툴용)
	HRESULT AddFrame(KEYFRAME::KEYFRAMETYPE eFrameType,wchar_t* wszSoundFrameTag,_float fSpeed = 0);

private:
	unordered_map<const wchar_t*, KEYFRAME>	m_KeyFrames;
	_uint				m_iNumKeyFrames = { 0 };

public:
	static CNotify* Create();
	virtual void Free() override;
};

END