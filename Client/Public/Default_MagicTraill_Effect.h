#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTrail;
class CParticleSystem;
END

BEGIN(Client)

class CDefault_MagicTraill_Effect final : public CGameObject
{
public:
	typedef struct InitDesc {
		_float3 vInitPosition;
	}INITDESC;
private:
	CDefault_MagicTraill_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDefault_MagicTraill_Effect(const CDefault_MagicTraill_Effect& rhs);
	virtual ~CDefault_MagicTraill_Effect() = default;

public:
	void	Ready_Stright(_float3 vTargerPosition, _float3 vStartPosition,_float fTrailLifeTime, _float fDistance);
	void	Ready_Spline(_float3 vTargerPosition, _float3 vStartPosition, _float fTrailLifeTime, _float fDistance, _float3 vAxis = _float3(1,1,1));
	void	Ready_Spin(_float3 vTargerPosition, _float3 vStartPosition, _float fTrailLifeTime, _float fDistance);

public:
	void	Set_Position(_float3 vPos);

	_bool	Stright_Move(_float fTimeDelta);
	_bool	Spin_Move(_float fTimeDelta);
	_bool	Spline_Move(_float fTimeDelta);

	void	Set_Trail_HeadColor(_float3 vColor);
	void	Set_Trail_TailColor(_float3 vColor);

	void	Set_Glow_BallColor(_float3 vColor);
	void	GlowBall_Dead();

public:
	virtual HRESULT Initialize_Prototype(const _tchar* wszFilePath,_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private: /* For. Component */
	CTrail* m_pTrail = { nullptr };
	CParticleSystem* m_pGlowBall = { nullptr };

private:
	CTransform* m_pTrailTransform = { nullptr };
	CTransform* m_pGlowBallTransform = { nullptr };
	
private:
	_uint m_iLevel = { 0 };
	_tchar	m_wszName[MAX_PATH] = {};

	//For. Trail
	_float3 m_vStartPostion = {};
	_float3 m_vEndPostion = {};
	_float	m_fLerpAcc = { 0.f };
	_float	m_fTrailLifeTime = {1.0f};

	//For. Spline
	_float3 m_vSplineLerp[2] = {};
	_float  m_fTimeScalePerDitance = { 0.f };

	//For. Detect
	_bool	m_isEnd = { false };

private:
	HRESULT Add_Components();

public:
	static CDefault_MagicTraill_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* wszFilePath, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};

END

