#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CMesh;
class CShader;
class CRenderer;
class CModel_Instance;
END

BEGIN(Client)

class CMapObject_Ins final : public CGameObject
{
public:
	typedef struct tagMapObjectInsDesc
	{
		_uint iInstanceCnt = { 0 };			// 인스턴스 개수
		_float4x4* pMatTransform;			// 각각 상태 행렬들의 주소
		_float4x4 WorldMatrix;				// 상태 행렬
		_uint iTagLen = { 0 };				// 문자열 길이
		_tchar wszTag[MAX_PATH] = TEXT(""); // 오브젝트 종류(모델 컴포넌트 이름)
	}MAPOJBECTINSDESC;

private:
	explicit CMapObject_Ins(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMapObject_Ins(const CMapObject_Ins& rhs);
	virtual ~CMapObject_Ins() = default;

public:
	void	Set_Pos(_float3 vPos) { m_pTransform->Set_Position(vPos); }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix) override;

private:
	CShader*			m_pShader = { nullptr };
	CShader*			m_pShadowShader = { nullptr };
	CRenderer*			m_pRenderer = { nullptr };
	CModel_Instance*	m_pModel = { nullptr };
	vector<CComponent*>	m_RigidBodys;

private:
	_float4				m_vEmissive;
	MAPOJBECTINSDESC	m_ObjectDesc;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);

public:
	static CMapObject_Ins* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END