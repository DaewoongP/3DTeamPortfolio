#pragma once
#include "Base.h"

BEGIN(Engine)

class CCamera : public CBase
{
public:
	enum TYPE { TYPE_STATIC, TYPE_MOVE, TYPE_END };

private:
	explicit CCamera() = default;
	virtual ~CCamera() = default;

public:
	HRESULT Initialize(TYPE eType, _float fFovY, _float fAspect, _float fNear, _float fFar);
	void Tick(_float fTimeDelta);

private:
	class CTransform*	m_pTransform = { nullptr };
	class CPipeLine*	m_pPipeLine = { nullptr };

private:
	TYPE				m_eType;
	_float				m_fFovY = { 0.f };
	_float				m_fAspect = { 0.f };
	_float				m_fNear = { 0.f };
	_float				m_fFar = { 0.f };

public:
	static CCamera* Create(TYPE eType, _float fFovY, _float fAspect, _float fNear, _float fFar);
	virtual void Free() override;
};

END