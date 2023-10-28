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
		{
			m_vInitPosition = _float3(1000.f, 1000.f, 0.f);
			m_isFinished = true;
			m_fTime = 0.f;
			m_fTimeAcc = 0.f;
			return true;
		}
			
		return false;
	}
	void Reset(_float3 vPosition, _float fFinishTime);

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
	_bool						m_isFinished = { false };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CBalloon_Timer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END