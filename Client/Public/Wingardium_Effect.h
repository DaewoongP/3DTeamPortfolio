
#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTrail;
END


BEGIN(Client)
#define TrailCount 8
class CWingardium_Effect final : public CGameObject
{
private:
	//트레일 하나당 가질 값.
	typedef struct TrailParticleDesc {
		_float fCurrnetLifeTime = { 10.f }; // 생명 주기
		_float fSettingLifeTime = { 10.f }; // 설정 생명주기
		_float fAnimStart_Y = { 0 };
		_float fRadius = { 0 };
		_float fToY = { 0 };
		_float fToRadius = { 0 };
		_float fSpeed = { 0 };
		_bool isEnable = { false };

		_bool isFlip = { false };
	}TRAILPARTICLEDESC;

private:
	CWingardium_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWingardium_Effect(const CWingardium_Effect& rhs);
	virtual ~CWingardium_Effect() = default;

public:
	void Set_Position(_float3 vPos);

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	void Enable_Trail(_bool _isEnable);
	HRESULT Reset_Trail();
	void SetActionTrigger(_bool value) {m_bActionTrigger = value;}
	void TrailAction(_float3 pos,_float fTimeDelta);

private:
	//회전하는 트레일
	CTrail* m_pTrail[TrailCount] = { nullptr };
	CTransform* m_pTrailTransform[TrailCount] = { nullptr };
	TRAILPARTICLEDESC m_TrailDesc[TrailCount] = {};

	//바닥에서 effect위치까지 올 트레일
	CTrail* m_pTrailToOrigin[TrailCount/2] = { nullptr };
	CTransform* m_pTrailToOriginTransform[TrailCount/2] = { nullptr };
	TRAILPARTICLEDESC m_TrailToOriginDesc[TrailCount/2] = {};
	
	//윙가르디움의 발동 트리거
	_bool	m_bActionTrigger = { false };
	
	//바닥에서 나오는 파티클

private:
	_uint			m_iLevel = { 0 };

	_float			m_TrailPerfDelay = { 3 };
	_float			m_TrailCurrentDelay = { 0 };
	_float			m_fDuration = { 30.0f };

	_float3			m_vBottom = {};

	_uint			m_iCurrentActionParticle = { 0 };

private:
	HRESULT Add_Components();

public:
	static CWingardium_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};

END

