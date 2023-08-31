#pragma once

/* =============================================== */
//	[CStatus]
//	- 객체의 상태를 나타내는 클래스
//	상태 컴포지트에 필요한 상태들을 추가하는 방식으로 만들었음
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "Composite.h"

BEGIN(Engine)

class ENGINE_DLL CStatus final : public CComposite
{
private:
	explicit CStatus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CStatus(const CStatus& rhs);
	virtual ~CStatus() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	static CStatus* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CStatus* Clone(void* pArg);
	virtual void Free() override;
};

END