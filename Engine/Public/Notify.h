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

	//�ε����� �̿��� �˻�
	KEYFRAME* Find_Frame(_uint iFindFrame);
	//�±׸� �̿��� �˻�
	KEYFRAME* Find_Frame(const wchar_t* wszKeyFrameTag);
	//�߰�(����)
	HRESULT AddFrame(KEYFRAME::KEYFRAMETYPE eFrameType,wchar_t* wszSoundFrameTag,_float fSpeed = 0);

private:
	unordered_map<const wchar_t*, KEYFRAME>	m_KeyFrames;
	_uint				m_iNumKeyFrames = { 0 };

public:
	static CNotify* Create();
	virtual void Free() override;
};

END