#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CMesh;
class CShader;
class CRenderer;
class CRigidBody;
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
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShader = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CModel_Instance* m_pModel = { nullptr };

private:
	HRESULT Add_Components(MAPOJBECTINSDESC* pMapObjectInsDesc);
	HRESULT SetUp_ShaderResources();

public:
	static CMapObject_Ins* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END