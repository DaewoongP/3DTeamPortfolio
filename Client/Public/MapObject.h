#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CMesh;
class CModel_LOD;
class CShader;
class CRenderer;
class CRigidBody;
END

BEGIN(Client)

class CMapObject final : public CGameObject
{
public:
	typedef struct tagMapObjectDesc
	{
		_float4x4 WorldMatrix; // ���� ���
		_uint iTagLen = { 0 }; // ���ڿ� ����
		_tchar wszTag[MAX_PATH] = TEXT(""); // ������Ʈ ����(�� ������Ʈ �̸�)
	}MAPOBJECTDESC;

private:
	explicit CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMapObject(const CMapObject& rhs);
	virtual ~CMapObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth() override;

private:
	CShader*		m_pShader = { nullptr };
	CShader*		m_pShadowShader = { nullptr };
	CRigidBody*		m_pRigidBody = { nullptr };
	CRenderer*		m_pRenderer = { nullptr };
	CModel_LOD*		m_pModel = { nullptr };

private:
	_bool			m_isCull = { true }; // ������ ���� ���� � Cull ����� �������� ���ϴ� ����
	// ����ü �ø��� ���� Bounding Box�� ���� �ϱ����� �ּ�, �ִ� ����
	_float3			m_vMinPoint, m_vMaxPoint, m_vCenterPoint;
	_float			m_fRadius = { 0.f };

private:
	HRESULT Add_Components(MAPOBJECTDESC* pMapObjectDesc);
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();
	void Check_MinMaxPoint(_float3 vPoint);

public:
	static CMapObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
