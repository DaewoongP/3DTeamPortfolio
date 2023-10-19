#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CMesh;
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CRigidBody;

END

BEGIN(Client)

class CPlayer;
class CPlayer_Information;

class CUI_Interaction;

class CGatherer final : public CGameObject
{
public: 
	// ������� �ֽ����ε� ��, ȣŬ���� ��, ������ ��, �ŸӸ� �� 
	enum GATHERING { ASHWINDEREGG, HORKLUMP, LEAPINGTOADSTOOLS, LEECH, GATHERING_END };

public:
	typedef struct tagMapObjectDesc
	{
		_float4x4 WorldMatrix; // ���� ���
		_uint iTagLen = { 0 }; // ���ڿ� ����
		_tchar wszTag[MAX_PATH] = TEXT(""); // ������Ʈ ����(�� ������Ʈ �̸�)
	}MAPOBJECTDESC;

private:
	explicit CGatherer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CGatherer(const CGatherer& rhs);
	virtual ~CGatherer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix) override;

private:
	CShader* m_pShader = { nullptr };
	CShader* m_pShadowShader = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CModel* m_pModel = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CTexture* m_pDissolveTexture = { nullptr };				 // ������� ������ �ؽ�ó

	CPlayer*			 m_pPlayer = { nullptr };			 // �÷��̾� �ּ�
	CPlayer_Information* m_pPlayerInformation = { nullptr }; // �÷��̾� �κ��丮�� ��ȣ�ۿ��ϱ� ���� �ּ�

	CLight* m_pLight_Horklump = { nullptr };				 // ȣŬ���� ���� ��

	CUI_Interaction* m_pUI_Interaction_Ash = { nullptr };
	CUI_Interaction* m_pUI_Interaction_Hor = { nullptr };
	CUI_Interaction* m_pUI_Interaction_Lea = { nullptr };
	CUI_Interaction* m_pUI_Interaction_Lee = { nullptr };

private:
	// ����ü �ø��� ���� Bounding Box�� ���� �ϱ����� �ּ�, �ִ� ����
	_float3			m_vMinPoint, m_vMaxPoint, m_vCenterPoint;
	_float			m_fRadius = { 0.f };
	_float			m_fDissolveAmount = { 0.f }; // ������ ��

	_bool			m_isGetItem = { true }; // ������ ȹ�� ���� ����
	_bool			m_isCol_with_Player = { false }; // �÷��̾�� �浹 ����
	_bool			m_isDissolveStart = { false };

	_uint			m_iCurrentLevel = { LEVEL_END };

	_float4x4		m_UIMatirx;

private:
	MAPOBJECTDESC	m_ObjectDesc;
	GATHERING		m_GatheringType = { CGatherer::GATHERING_END }; // ä���� ����

private:
	HRESULT Make_Notifies();
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);
	void	Check_MinMaxPoint(_float3 vPoint);

private:
	void Gatherer_Dead(); // ä���� ���, Notify

public:
	static CGatherer* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END