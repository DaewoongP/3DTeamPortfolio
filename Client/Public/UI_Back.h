#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_Back final : public CUI
{
private:
	explicit CUI_Back(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Back(const CUI_Back& rhs);
	virtual ~CUI_Back() = default;

public:
	void Set_Rotation(_float3 vAxis, _float fRadian);
	void Set_RemoveBlack(_bool isRemove = true) { m_isRemoveBlack = isRemove; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	_bool		m_isRemoveBlack = { false };

public:
	static CUI_Back* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
