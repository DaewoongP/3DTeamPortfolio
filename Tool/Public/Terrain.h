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
	typedef struct BrushDesc
	{
		_float3 vPos; // 위치
		_float fRange; // 브러쉬 범위
		_uint iTextureIndex; // 어떤 텍스처인지
	}BRUSH;

private:
	explicit CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	CVIBuffer_Terrain* Get_Buffer() const { return m_pBuffer; }
	void Set_CurrentBrushingSize(_float fSize) { m_fBrushCurrentRange = fSize; }
	void Set_CurrentBrushingPoint(_float3 vPos) { m_vBrushCurrentPos = vPos; }

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

	// 브러쉬 정보
	_float3				m_vBrushCurrentPos = { 0.f, 0.f, 0.f }; // 현재 브러쉬 커서의 위치
	_float				m_fBrushCurrentRange = { 10.f }; // 현재 브러쉬 커서의 범위

	vector<BRUSH>		m_vecBrushPos; // 브러쉬 정보 저장 벡터
	_uint				m_iBrushPosCnt; // 칠해질 개수
	_float3				m_vBrushPos[MAX_SHADERVECTOR]; // 칠해질 위치
	_float				m_fBrushRange[MAX_SHADERVECTOR]; // 칠해질 범위

private:
	CShader*			m_pShader = { nullptr };
	CTexture*			m_pTexture = { nullptr };
	CRenderer*			m_pRenderer = { nullptr };
	CVIBuffer_Terrain*	m_pBuffer = { nullptr };

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