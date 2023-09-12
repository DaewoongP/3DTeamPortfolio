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
		_float3 vPos; // ��ġ
		_float fRange; // �귯�� ����
		_uint iTextureIndex; // � �ؽ�ó����
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

	// �귯�� ����
	_float3				m_vBrushCurrentPos = { 0.f, 0.f, 0.f }; // ���� �귯�� Ŀ���� ��ġ
	_float				m_fBrushCurrentRange = { 10.f }; // ���� �귯�� Ŀ���� ����

	vector<BRUSH>		m_vecBrushPos; // �귯�� ���� ���� ����
	_uint				m_iBrushPosCnt; // ĥ���� ����
	_float3				m_vBrushPos[MAX_SHADERVECTOR]; // ĥ���� ��ġ
	_float				m_fBrushRange[MAX_SHADERVECTOR]; // ĥ���� ����

private:
	CShader*			m_pShader = { nullptr };
	CTexture*			m_pTexture = { nullptr };
	CRenderer*			m_pRenderer = { nullptr };
	CVIBuffer_Terrain*	m_pBuffer = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShaderDynamicResources(); // �������� �Ҵ��� ������ ���� ���̴��� ��������

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END