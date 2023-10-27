#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CBalloon_Timer final : public CGameObject
{
public:
	typedef struct tagBalloonTimerDesc
	{
		_float fTime;
		_float2 vScale;
		_float3 vPosition;
	}BALLOONTIMERDESC;

private:
	explicit CBalloon_Timer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBalloon_Timer(const CBalloon_Timer& rhs);
	virtual ~CBalloon_Timer() = default;

public:
	_bool Is_Finished() { 
		if (m_fTimeAcc > m_fTime)
			return true;
		return false;
	}

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*					m_pShader = { nullptr };
	CRenderer*					m_pRenderer = { nullptr };
	CVIBuffer_Point_Instance*	m_pBuffer = { nullptr };

private:
	_float						m_fTime = { 0.f };
	_float						m_fTimeAcc = { 0.f };
	_float3						m_vInitPosition;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CBalloon_Timer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END