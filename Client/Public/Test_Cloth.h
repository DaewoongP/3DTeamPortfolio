#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Cloth;
END

class CTest_Cloth final : public CGameObject
{
private:
	explicit CTest_Cloth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTest_Cloth(const CTest_Cloth& rhs);
	virtual ~CTest_Cloth() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				m_pShader = { nullptr };
	CRenderer*				m_pRenderer = { nullptr };
	CVIBuffer_Cloth*		m_pBuffer = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

#ifdef _DEBUG
	void Tick_ImGui();
#endif // _DEBUG

public:
	static CTest_Cloth* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

