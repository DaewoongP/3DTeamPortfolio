#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Tool)

class CDummy_UI final : public CGameObject
{
protected:
	explicit CDummy_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDummy_UI(const CDummy_UI& rhs);
	virtual ~CDummy_UI() = default;

public:
	_float2 Get_fXY() { return _float2(m_fX, m_fY); }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_fXY(_float fX, _float fY, _float fZ = 0) 
	{ 
		m_fX = fX;
		m_fY = fY;
		if (fZ != 0)
			m_fZ = fZ;
	}

	void Set_Size(_float fX, _float fY)
	{
		m_fSizeX = fX;
		m_fSizeY = fY;
	}

protected:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

protected:
	// 윈도우창의 실제 x좌표
	_float			m_fX = { 650.f };
	// 윈도우창의 실제 y좌표
	_float			m_fY = { 360.f };
	// UI 거리 체크용 z값
	_float			m_fZ = { 0.f };
	// 텍스처 x사이즈 설정
	_float			m_fSizeX = { 100.f };
	// 텍스처 y사이즈 설정
	_float			m_fSizeY = { 100.f };

	_float4x4		m_ViewMatrix;
	_float4x4		m_ProjMatrix;

private:
	_tchar	m_wszTextureName[MAX_STR] = {};

public:
	_bool	Is_In_Rect();

public:
	_float2	UIPos_To_WorldPos(_float fX, _float fY);
	_float2 WorldPos_To_UIPos(_float fX, _float fY);

protected:
	HRESULT Change_Position(_float fX, _float fY);
	HRESULT Change_Scale(_float fX, _float fY);

public:
	static CDummy_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _tchar* TextureComTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


