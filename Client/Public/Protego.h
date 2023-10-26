#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "MagicBall.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CVIBuffer_UVSphere;
class CTexture_Flipbook;
END

BEGIN(Client)

class CProtego final : public CMagicBall
{
private:
	explicit CProtego(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CProtego(const CProtego& rhs);
	virtual ~CProtego() = default;

public:
	void Set_Scale(_float fScale) { m_fScale = fScale; }

public:
	virtual HRESULT Initialize_Prototype(_uint _iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionStay(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Reset(MAGICBALLINITDESC& InitDesc) override;

	_float3 Hit_Effect(_float3 vPosition);
	void Break_Effect(_float3 vPosition);

protected:
	//모든 이펙트 비활성화
	virtual void Ready_Begin();
	//완드 이펙트 활성화 
	virtual void Ready_DrawMagic();
	//메인 이펙트 활성화
	virtual void Ready_CastMagic();
	//히트 이펙트 활성화
	virtual void Ready_Dying();

	//다음 상태로
	virtual void Tick_Begin(_float fTimeDelta);
	//완드 이펙트 재생(다음상태로는 다른곳에서 재생)
	virtual void Tick_DrawMagic(_float fTimeDelta);
	//메인 이펙트 재생(도달/충돌시 다음상태로) / 마법 발사 상태는 다 다르므로 알아서 처리하셈
	virtual void Tick_CastMagic(_float fTimeDelta);
	//히트 이펙트 재생(재생 종료시 사망)
	virtual void Tick_Dying(_float fTimeDelta);

private:
	void Find_And_Add_Texture(const _tchar* pPath);
	
private: /* For. Component */
	CShader* m_pShader = { nullptr };
	CTexture* m_pTexture[5] = {};
	CVIBuffer_UVSphere* m_pBuffer = { nullptr };
	CParticleSystem* m_pDefaultConeBoom_Particle = { nullptr };
	CTexture_Flipbook* m_pFlameBlastFlipbook = { nullptr };

	array<CParticleSystem*, 5> m_pBreakParticle = { nullptr };

private:
	_float4 m_vColor1 = { 1.f, 1.f, 1.f, 1.f };
	_float4 m_vColor2 = { 1.f, 1.f, 1.f, 1.f };
	_float m_fScale = { 1.0f };	
	_float m_fEnterDuration = { 0.1f };
	_float m_fExitDuration = { 0.1f };
	_float m_fRimPower = { 2.1f };
	_bool m_isHitEffect = { false };

	_float m_fTimeAcc = { 0.f };
	_float m_fHitTimeAcc = { 0.f };
	_float3 m_vCollisionPoint = { _float3() };
	_float3 m_vCollisionPointOffset = { _float3() };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	virtual HRESULT Add_RigidBody() override;

public:
	static CProtego* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint _iLevel);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END