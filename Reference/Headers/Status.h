#pragma once

/* =============================================== */
//	[CStatus]
//	- ��ü�� ���¸� ��Ÿ���� Ŭ����
//	���� ������Ʈ�� �ʿ��� ���µ��� �߰��ϴ� ������� �������
// 
//	�� : �ּ�ȯ
//	�� :
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