#pragma once

/* =============================================== */
//	[CDefence]
//	- ��ü�� ������ ��Ÿ���� Ŭ����
//  - �� ������ �Ӽ� ���׷����� ǥ���ص� ���� ��
// 
//	�� : �ּ�ȯ
//	�� :
//
/* =============================================== */

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CDefence final : public CComponent
{
public:
	typedef struct tagDeffenceDesc
	{
		_int iDeffence;
	}DEFFENCEDESC;

private:
	explicit CDefence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDefence(const CDefence& rhs);
	virtual ~CDefence() = default;

public:
	const _int& Get_Deffence() const {
		return m_iDeffence;
	}
	void Set_Deffence(const _int & iDeffence) {
		m_iDeffence = iDeffence;
	}

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;

private:
	_int m_iDeffence = { 0 };

public:
	static CDefence* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CDefence* Clone(void* pArg);
	virtual void Free() override;
};

END