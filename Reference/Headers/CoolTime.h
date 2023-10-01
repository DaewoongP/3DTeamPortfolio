#pragma once

/* =============================================== */
// 
//	정 : 전대인
//	부 :
//
/* =============================================== */

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCoolTime final : public CComponent
{
public:
	typedef struct tagCoolTimeDesc
	{
		_float fMaxCoolTime;
	}COOLTIME_DESC;

private:
	explicit CCoolTime(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCoolTime(const CCoolTime& rhs);
	virtual ~CCoolTime() = default;

public:
	_float Get_Current_CoolTime_Ratio() { return m_fCurCoolTime / m_fMaxCoolTime; }
	
	void Set_MaxCoolTime(_float _fMaxCoolTime);

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float _fTimeDelta) override;

public:
	_bool IsEnable() { return m_isEnable; }
	void Play_CoolTime();
	void Reset();

private:
	_bool m_isEnable = { true };
	_float m_fMaxCoolTime = { 0.f };
	_float m_fCurCoolTime = { 0.f };

public:
	static CCoolTime* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CCoolTime* Clone(void* pArg);
	virtual void Free() override;
};

END