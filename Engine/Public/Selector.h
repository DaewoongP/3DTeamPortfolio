#pragma once

/* =============================================== */
//	[CSelector]
//	- CBehavior Ŭ���� ����� ���� ����.
// 
//	�� : �ּ�ȯ
//	�� :
//
/* =============================================== */

#include "Behavior.h"

BEGIN(Engine)

class ENGINE_DLL CSelector : public CBehavior
{
protected:
	explicit CSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSelector(const CSelector& rhs);
	virtual ~CSelector() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override { return S_OK; }
	virtual HRESULT Tick(const _float& fTimeDelta) override;

public:
	static CSelector* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSelector* Clone(void* pArg) override;
	virtual void Free() override;
};

END