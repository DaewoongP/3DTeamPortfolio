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
	enum STATE { ENTER, STAY, EXIT, STATE_END };

private:
	explicit CProtego(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CProtego(const CProtego& rhs);
	virtual ~CProtego() = default;

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

private:
	void Tick_Enter(const _float& fTimeDelta);
	void Tick_Stay(const _float& fTimeDelta);
	void Tick_Exit(const _float& fTimeDelta);

private:
	void Late_Tick_Enter(const _float& fTimeDelta);
	void Late_Tick_Stay(const _float& fTimeDelta);
	void Late_Tick_Exit(const _float& fTimeDelta);

	void Hit_Effect(_float3 vPosition);

private:
	void Find_And_Add_Texture(const _tchar* pPath);
#ifdef _DEBUG
	//void Tick_Imgui();
#endif // _DEBUG
private: /* For. Component */
	CShader* m_pShader = { nullptr };
	CTexture* m_pTexture[5] = {};
	CVIBuffer_UVSphere* m_pBuffer = { nullptr };
	CParticleSystem* m_pDefaultConeBoom_Particle = { nullptr };
	CTexture_Flipbook* m_pFlameBlastFlipbook = { nullptr };

private:
	STATE m_eCurState = { ENTER };
	_float4 m_vColor1 = { 1.f, 1.f, 1.f, 1.f };
	_float4 m_vColor2 = { 1.f, 1.f, 1.f, 1.f };
	_float m_fScale = { 2.3f };
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