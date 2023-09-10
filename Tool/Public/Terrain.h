#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CNavigation;
class CVIBuffer_Terrain;
END

BEGIN(Tool)

class CTerrain final : public CGameObject
{
private:
	explicit CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	CVIBuffer_Terrain* Get_Buffer() const { return m_pBuffer; }
	void Set_BrushingSize(_float fSize) { m_fBrushSize = fSize; }
	void Set_BrushingPoint(_float3 vPos) { m_vBrushingPoint = vPos; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
public:
	HRESULT Picking_On_Terrain(_Inout_ _float4* pPickPosition);

private:
	_bool				m_isRendering = { false };
	_float				m_fBrushSize = { 10.f };
	_float3				m_vBrushingPoint = { 0.f, 0.f, 0.f };

	vector<_float3> m_vecBrushPos; // 브러쉬로 칠해질 지형의 위치값 저장 벡터
	_float3* m_pBrushPos;
	_uint m_iBrushPosCnt = { 3 }; // m_vecBrushPos 사이즈

private:
	CShader* m_pShader = { nullptr };
	CTexture* m_pTexture = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CVIBuffer_Terrain* m_pBuffer = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShaderDynamicResources(); // 동적으로 할당한 버퍼의 값을 쉐이더로 전달해줌

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END