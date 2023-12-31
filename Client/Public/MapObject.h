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

class CMapObject : public CGameObject
{
public:
	typedef struct tagMapObjectDesc
	{
		_float4x4 WorldMatrix; // 상태 행렬
		_uint iTagLen = { 0 }; // 문자열 길이
		_tchar wszTag[MAX_PATH] = TEXT(""); // 오브젝트 종류(모델 컴포넌트 이름)
	}MAPOBJECTDESC;

protected:
	explicit CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMapObject(const CMapObject& rhs);
	virtual ~CMapObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix) override;
	
protected:
	CShader*		m_pShader = { nullptr };
	CShader*		m_pShadowShader = { nullptr };
	CRenderer*		m_pRenderer = { nullptr };
	CModel*			m_pModel = { nullptr };
	vector<CComponent*>	m_RigidBodys;

protected:
	_bool			m_isCull = { true }; // 스케일 값에 따라 어떤 Cull 방식을 선택할지 정하는 변수
	// 절두체 컬링을 위해 Bounding Box를 생성 하기위한 최소, 최대 정점
	_float3			m_vMinPoint, m_vMaxPoint, m_vCenterPoint;
	_float			m_fRadius = { 0.f };
	MAPOBJECTDESC	m_ObjectDesc;

	_float4			m_vEmissive;

protected:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);
	void Check_MinMaxPoint(_float3 vPoint);

public:
	static CMapObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
