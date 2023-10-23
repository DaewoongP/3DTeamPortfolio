#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CSky final : public CGameObject
{
private:
	explicit CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSky(const CSky& rhs);
	virtual ~CSky() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel*				m_pModel = { nullptr };
	CShader*			m_pShader = { nullptr };
	CRenderer*			m_pRenderer = { nullptr };

private:
	LEVELID				m_iCurrentLevelIndex = { LEVEL_END };
	_float2				m_vMoonPos;
	_int				m_iFrame = { 0 };
	_bool				m_isNight = { false };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CSky* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END