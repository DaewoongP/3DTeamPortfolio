#pragma once

/* =============================================== */
//	[CDefence]
//	- 객체의 방어력을 나타내는 클래스
//  - 각 마법의 속성 저항력으로 표현해도 좋을 듯
// 
//	정 : 주성환
//	부 :
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