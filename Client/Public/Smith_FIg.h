#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CRigidBody;
END

BEGIN(Client)
class CUI_Interaction;
class CScript;
END

BEGIN(Client)

class CSmith_Fig final : public CGameObject
{
private:
	enum SMITHFIGSCRIPT
	{
		SMITHFIGSCRIPT_POTION,
		SMITHFIGSCRIPT_TOWN,
		SMITHFIGSCRIPT_SECRET,
		SMITHFIGSCRIPT_BONE,
		SMITHFIGSCRIPT_END
	};

private:
	explicit CSmith_Fig(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSmith_Fig(const CSmith_Fig& rhs);
	virtual ~CSmith_Fig() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix) override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CShader* m_pShadowShaderCom = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CUI_Interaction* m_pUI_Interaction = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);
	
private:
	void	Check_Quest();
	void	Play_Script(_float fTimeDelta);

private:
	_bool	m_isColPlayer = { false };

private:
	_bool					m_isPlayScript = { false };
	_uint					m_iScriptIndex = 0;
	vector<CScript*>			m_pScripts;

	_bool					m_isUnlockQuest[SMITHFIGSCRIPT_END] = { false };
	_bool					m_isClearQuest[SMITHFIGSCRIPT_END] = { false };

public:
	static CSmith_Fig* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END