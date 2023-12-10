#pragma once
#include "GameObject.h"
#include "Engine_Defines.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CCoolTime;
class CHealth;
class CRigidBody;
END

BEGIN(Client)
class CPotion abstract : public CGameObject
{
protected:
	typedef struct tagPotionCreateDesc
	{
		vector<INGREDIENT>	Ingredients;
		_float				fManufacturingTime = { 0.f };
		wstring				wstrModelPath = { TEXT("") };
		CModel::TYPE		eModelType = { CModel::TYPE_END };
		_float4x4			PivotMatrix = { _float4x4() };
		POTIONTAP			ePotionTap = { POTIONTAP_END };
	}POTION_CREATE_DESC;

protected:
	typedef struct tagPotionCloneDesc
	{
		_float4x4        OffsetMatrix;
		_float4x4        PivotMatrix;
		const _float4x4* pCombindTransformationMatrix;
		const _float4x4* pParentWorldMatrix;
	}POTION_CLONE_DESC;

protected:
	CPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPotion(const CPotion& rhs);
	virtual ~CPotion() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix) override;

private:
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);
	HRESULT Set_ShaderResources();
	HRESULT Add_Components();
	HRESULT Add_RigidBody();


protected:
	_uint		m_iLevel = { 0 };

protected: // About Player
	CPlayer* m_pPlayer = { nullptr };
	class CCustomModel* m_pPlayerModel = { nullptr };
	class CTransform* m_pPlayerTransform = { nullptr };
	class CPlayer_Information* m_pPlayerInformation = { nullptr };

protected: // For. Components
	CRenderer*	m_pRenderer = { nullptr };
	CModel*		m_pModel = { nullptr };
	CShader*	m_pShader = { nullptr };
	CShader* m_pShadowShader = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CCoolTime*	m_pLifeTime = { nullptr };
	CCoolTime*	m_pAttachedTime = { nullptr };

protected:
	POTION_CREATE_DESC			m_PotionCreateDesc = { POTION_CREATE_DESC() };
	_float	m_fDuration = { 0.f };
	
protected:
	_bool			 m_isAttached = { false };
	_float4x4        m_OffsetMatrix;
	_float4x4        m_PivotMatrix;
	const _float4x4* m_pCombindTransformationMatrix;
	const _float4x4* m_pParentWorldMatrix;

public:
	virtual void Free(void) override;
};
END
