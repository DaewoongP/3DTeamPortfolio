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

class CMapObject final : public CGameObject
{
public:
	typedef struct tagMapObjectDesc
	{
		_float4x4 WorldMatrix; // 상태 행렬
		_uint iTagLen = { 0 }; // 문자열 길이
		_tchar wszTag[MAX_PATH] = TEXT(""); // 오브젝트 종류(모델 컴포넌트 이름)
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

private:
	CShader*		m_pShader = { nullptr };
	CRigidBody*		m_pRigidBody = { nullptr };
	CRenderer*		m_pRenderer = { nullptr };
	CModel*			m_pModel = { nullptr };

private:
	_bool			m_isCull = { true }; // 스케일 값에 따라 어떤 Cull 방식을 선택할지 정하는 변수

private:
	HRESULT Add_Components(MAPOBJECTDESC* pMapObjectDesc);
	HRESULT SetUp_ShaderResources();

public:
	static CMapObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
