#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CMesh;
class CModel;
class CShader;
class CRenderer;
class CParticleSystem;
END

BEGIN(Client)

class CLightStand;

class CVault_Gate final : public CGameObject
{
public:
	typedef struct tagMapObjectDesc
	{
		_float4x4 WorldMatrix; // ���� ���
		_uint iTagLen = { 0 }; // ���ڿ� ����
		_tchar wszTag[MAX_PATH] = TEXT(""); // ������Ʈ ����(�� ������Ʈ �̸�)
	}MAPOBJECTDESC;

private:
	explicit CVault_Gate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVault_Gate(const CVault_Gate& rhs);
	virtual ~CVault_Gate() = default;

public:
	_bool Get_GateOpen() const { return m_isCheckOnce; }
	_bool Get_All_LightStands_On() const { return m_isAllLightStandsOn; } // �ƽ��� ���� �� �ܰ� �߰�
	void Set_Gate_On() { m_isGateOpen = true; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix) override;

private:
	CShader* m_pShader = { nullptr };
	CShader* m_pShadowShader = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CModel* m_pModel = { nullptr };
	CParticleSystem* m_pEffect = { nullptr };

private:
	// ����ü �ø��� ���� Bounding Box�� ���� �ϱ����� �ּ�, �ִ� ����
	_float3			m_vMinPoint, m_vMaxPoint, m_vCenterPoint;
	_float			m_fRadius = { 0.f };

	_bool			m_isCheckOnce = { false }; // �ѹ��� ��ȣ�ۿ� ����
	_bool			m_isAllLightStandsOn = { false };
	_bool			m_isGateOpen = { false };

	_bool			m_isSound0 = { true }; // �� ������ �Ҹ�
	_uint			m_iSound0 = { 0 };	   // �� ������ �Ҹ� �ε���

	_bool			m_isSound1 = { true }; // �� ������ �� �Ҹ�
	
	vector<CLightStand*> m_pLightStands; // �ʿ� �����ϴ� ȭ�ε�

private:
	MAPOBJECTDESC	m_ObjectDesc;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);
	void	Check_MinMaxPoint(_float3 vPoint);
	void	Check_FireOn();

public:
	static CVault_Gate* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END