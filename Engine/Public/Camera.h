#pragma once
/* =============================================== */
// 
//	정 : 장현우
//	부 :
//
/* =============================================== */

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCamera : public CBase
{
public:
	enum TYPE { TYPE_MAIN, TYPE_CUTSCENE, TYPE_END };
	typedef struct tagCameraDesc
	{
		_float m_fFovY = { 0.f };
		_float m_fAspect = { 0.f };
		_float m_fNear = { 0.f };
		_float m_fFar = { 0.f };
	}CAMERADESC;

protected:
	explicit CCamera() = default;
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize(const CAMERADESC& CameraDesc);
	//트렌스폼을 사용하여 파이프라인 뷰행렬 적용
	//투영정보는 안 바꾸겠음
	virtual void Tick(const _float& fTimeDelta);

protected:
	class CTransform*	m_pTransform = { nullptr };
	class CPipeLine*	m_pPipeLine = { nullptr };

private:
	CAMERADESC m_CameraDesc;

public:
	static CCamera* Create(const CAMERADESC& CameraDesc);
	virtual void Free() override;
};

END