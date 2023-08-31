#pragma once

/* =============================================== */
//	[CIgnite]
//	- 화상 상태 클래스
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CIgnite final : public CComponent
{
public:
	typedef struct tagStatusIgniteDesc
	{
		_float fDuration = {0.f};
	}STATUSIGNITEDESC;

private:
	explicit CIgnite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CIgnite(const CIgnite& rhs);
	virtual ~CIgnite() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

private:
	_float m_fDuration = { 0.f };

public:
	static CIgnite* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CIgnite* Clone(void* pArg);
	virtual void Free() override;
};

END