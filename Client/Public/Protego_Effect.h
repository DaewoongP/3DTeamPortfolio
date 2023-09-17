
#pragma once

#include "GameObject.h"
#include "Engine_Defines.h"
#include "Client_Defines.h"
#include "Protego.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CCollider;
class CNavigation;
class CModel;
class CVIBuffer_GeoSphere;
class CVIBuffer_Rect;
class CParticleSystem;
class CTexture_Flipbook;
END

BEGIN(Client)
class CProtego_Effect final : public CGameObject
{
	typedef CMagicBall::MAGICBALLDESC MAGICBALLDESC;
private:
	enum STATE { ENTER, STAY, EXIT, STATE_END};

private:
	CProtego_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProtego_Effect(const CProtego_Effect& rhs);
	virtual ~CProtego_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint _iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Play_ForceField();
	void Exit_ForceField();
	void Hit_Effect(_float3 vPosition);

private:
	void Tick_Enter(const _float& fTimeDelta);
	void Tick_Stay(const _float& fTimeDelta);
	void Tick_Exit(const _float& fTimeDelta);
private:
	void Late_Tick_Enter(const _float& fTimeDelta);
	void Late_Tick_Stay(const _float& fTimeDelta);
	void Late_Tick_Exit(const _float& fTimeDelta);

private:
	void Find_And_Add_Texture(const _tchar* pPath);
#ifdef _DEBUG
	void Tick_Imgui();
#endif // _DEBUG
private: /* For. Component */
	CShader* m_pShader = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CTexture* m_pTexture[5] = {};
	CVIBuffer_GeoSphere* m_pBuffer = { nullptr };
	CParticleSystem* m_pDefaultConeBoom_Particle = { nullptr };
	CTexture_Flipbook* m_pTextureFlipbook = { nullptr };
private: // 파티클 Transform

private: // Protego의 정보
	const MAGICBALLDESC* m_pMagicBallDesc;

private:
	_uint m_iLevel = { 0 };
	STATE m_eCurState = { ENTER };
	_float4 m_vColor1 = { 1.f, 1.f, 1.f, 1.f };
	_float4 m_vColor2 = { 1.f, 1.f, 1.f, 1.f };
	_float m_fScale = { 0.1f };
	_float m_fTimeAcc = { 0.f };
	_float m_fEnterDuration = { 0.1f };
	_float m_fExitDuration = { 0.1f };
	_float m_fRimPower = { 4.7f };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CProtego_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

