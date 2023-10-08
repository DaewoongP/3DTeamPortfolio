#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CMesh;
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CPlayer;

class CGull final : public CGameObject
{
	// GULL_IDLE1�� ���̸� �Դ� ���
	// GULL_IDLE2�� �ֺ��� �θ��� �Ÿ��� ���
	enum GULL_ANIMATION {
		GULL_ALERT, GULL_FLY, GULL_IDLE1, GULL_IDLE2,
		GULL_START_FLY, GULL_START_FLY_REVERSE, GULL_END
	};

public:
	typedef struct tagMapObjectDesc
	{
		_float4x4 WorldMatrix; // ���� ���
		_uint iTagLen = { 0 }; // ���ڿ� ����
		_tchar wszTag[MAX_PATH] = TEXT(""); // ������Ʈ ����(�� ������Ʈ �̸�)
	}MAPOBJECTDESC;

private:
	explicit CGull(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CGull(const CGull& rhs);
	virtual ~CGull() = default;

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

	CPlayer* m_pPlayer = { nullptr };			 // �÷��̾� �ּ�

private:
	// ����ü �ø��� ���� Bounding Box�� ���� �ϱ����� �ּ�, �ִ� ����
	_float3			m_vMinPoint, m_vMaxPoint, m_vCenterPoint;
	_float			m_fRadius = { 0.f };
	_float			m_fDist_From_Player = { 0.f }; // ���ű�� �÷��̾���� �Ÿ�

private:
	MAPOBJECTDESC	m_ObjectDesc;
	GULL_ANIMATION  m_GullAnimIndex = { GULL_END };
	GULL_ANIMATION  m_PreGullAnimIndex = { m_GullAnimIndex }; // ���� �ִϸ��̼� �ε���

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();
	void	Check_MinMaxPoint(_float3 vPoint);
	void	Check_Dist_From_Player(_float fTimeDelta);

public:
	static CGull* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END