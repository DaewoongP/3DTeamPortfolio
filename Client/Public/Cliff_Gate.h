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

class CCliff_Gate final : public CGameObject
{
public:
	typedef struct tagMapObjectDesc
	{
		_float4x4 WorldMatrix; // ���� ���
		_uint iTagLen = { 0 }; // ���ڿ� ����
		_tchar wszTag[MAX_PATH] = TEXT(""); // ������Ʈ ����(�� ������Ʈ �̸�)
	}MAPOBJECTDESC;

private:
	explicit CCliff_Gate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCliff_Gate(const CCliff_Gate& rhs);
	virtual ~CCliff_Gate() = default;

public:
	_bool Get_GateOpen() const { return m_isEffectOn; } // ��Ż ����Ʈ�� ��Ÿ���� ����Ʈ�� Ȱ��ȭ�� ������ ����
	_bool Get_All_LightStands_On() const { return m_isAllLightStandsOn; } // ȭ�ΰ� �� ��ȭ�Ǹ� true ����
	void  Set_Gate_Reparo_On() { m_isReparoOn = true; } // �ƽ��� ���ؼ� �Ѵܰ� �߰�

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
	_bool			m_isCheckOnce = { true }; // �ѹ��� ��ȣ�ۿ� ����
	_bool			m_isEffectOn = { false }; // ����Ʈ �� ���� ���ֱ�
	_bool			m_isAllLightStandsOn = { false }; // ȭ�ΰ� ���� ��ȭ�Ǿ�����
	_bool			m_isReparoOn = { false };	// �ƽ��� ���ؼ� �Ѵܰ� �߰�

	_uint			m_iLightStandsCnt = 0;  // ȭ���� ����

	vector<CLightStand*> m_pLightStands; // �ʿ� �����ϴ� ȭ�ε�

private:
	MAPOBJECTDESC	m_ObjectDesc;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);
	void	Check_FireOn();

public:
	static CCliff_Gate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END