#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CMesh;
class CModel;
class CShader;
class CRenderer;
class CRigidBody;
END

BEGIN(Client)

class CPlayer;

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
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth() override;

private:
	CShader* m_pShader = { nullptr };
	CShader* m_pShadowShader = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CModel* m_pModel = { nullptr };

	CPlayer* m_pPlayer = { nullptr }; // �÷��̾� �ּ�

private:
	// ����ü �ø��� ���� Bounding Box�� ���� �ϱ����� �ּ�, �ִ� ����
	_float3			m_vMinPoint, m_vMaxPoint, m_vCenterPoint;
	_float			m_fRadius = { 0.f };
	_float			m_fDist_From_Player = { 0.f }; // ä������ �÷��̾���� �Ÿ�

private:
	MAPOBJECTDESC	m_ObjectDesc;
	GATHERING		m_GatheringType = { GATHERING_END }; // ä���� ����

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();
	void	Check_MinMaxPoint(_float3 vPoint);

public:
	static CGatherer* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END