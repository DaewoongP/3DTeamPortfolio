#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI abstract : public CGameObject
{
protected:
	explicit CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

protected:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };

protected:
	// ������â�� ���� x��ǥ
	_float			m_fX = { 0.f };
	// ������â�� ���� y��ǥ
	_float			m_fY = { 0.f };
	// �ؽ�ó x������ ����
	_float			m_fSizeX = { 0.f };
	// �ؽ�ó y������ ����
	_float			m_fSizeY = { 0.f };

	_float4x4		m_ViewMatrix;
	_float4x4		m_ProjMatrix;

protected:
	HRESULT Change_Position(_float fX, _float fY);

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END