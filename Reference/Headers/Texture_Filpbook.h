
#pragma once
#include "GameObject.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CCollider;
class CNavigation;
class CModel;
class CVIBuffer_Rect;
END

BEGIN(Engine)
class ENGINE_DLL CTexture_Flipbook : public CGameObject
{
public:
	enum LOOP_OPTION { ONCE, LOOP, PING_PONG, LOOP_OPTION_END };

protected:
	CTexture_Flipbook(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture_Flipbook(const CTexture_Flipbook& rhs);
	virtual ~CTexture_Flipbook() = default;

public:
	void Set_Position(_float3 vPos);

	// For. Binding
	HRESULT Bind_DiffuseTexture(CShader* pShader, const _char* pConstantName);
	HRESULT Bind_NormalTexture(CShader* pShader, const _char* pConstantName);
	HRESULT Bind_FilpbookMaterialTexture(CShader* pShader, const _char* pConstantName);
	const _uint* Get_WidthLengthPtr() { return &m_iWidthLength; }
	const _uint* Get_HeightLengthPtr() { return &m_iHeightLength; }
	const _uint* Get_CurIndexPtr() { return &m_iCurIndex; }

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel, const _tchar* pFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Play(_float3 vPos);
	void Play(_float3 vPos, LOOP_OPTION eLoopOption);
	void Stop();

public:
	HRESULT Save(const _tchar* pFilePath);
	HRESULT Load(const _tchar* pFilePath);

private:
	void Tick_Once(_float fTimeDelta);
	void Tick_Loop(_float fTimeDelta);
	void Tick_PingPong(_float fTimeDelta);

protected: /* For. Component */
	CShader* m_pShader = { nullptr };
	CTexture* m_pDiffuseTexture = { nullptr };
	CTexture* m_pNormalTexture = { nullptr };
	CTexture* m_pFlipbookMaterialTexture = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CVIBuffer_Rect* m_pBuffer = { nullptr };

protected:
	// For. Loading
	_uint m_iLevel = { 0 };
	wstring m_wstrPath = TEXT("../../Resources/Effects/Textures/Flipbooks/T_FX_Oceansplash_D.png");

	// Row, Col
	_uint m_iCurIndex = { 0 };
	_uint m_iMaxIndex = { 1 };
	_uint m_iWidthLength = { 1 };
	_uint m_iHeightLength = { 1 };

	// ClipChannel
	string m_strClipChannel = { "Red" };

	// Color
	_bool m_isMixColor = { false };
	_float4 m_vColor = { 1.f, 1.f, 1.f, 1.f };

	// Enable
	_bool m_isEnable = { true };

	// LoopOption
	_bool m_isIncrease = { true };
	string m_strLoopOption = { "Loop" }; // Once, Loop, Ping-Pong

	// NormalTexture
	_bool m_isUseNormalTexture = { false };
	wstring m_wstrNormalPath = TEXT("../../Resources/Effects/Textures/Flipbooks/VFX_T_Dust_8x8_N.png");

	// Billboard DeleteY
	_bool m_isDeleteY = { false };

	// Update Interval
	_float m_fUpdateInterval = { 0.0159f };
	_float m_fTimeAcc = { 0.f };

	_bool m_isUseFlipbookMaterialTexture = { false };
	wstring m_wstrFlipbookMaterialPath = TEXT("../../Resources/Effects/Textures/Flipbooks/VFX_T_Fireball_Dir_01_D_FM.png");

protected:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTexture_Flipbook* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel, const _tchar* pFilePath);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END
