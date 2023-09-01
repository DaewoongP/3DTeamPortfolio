#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)

class CRenderer;
class CCollider;
class CTransform;

END

BEGIN(Tool)

class CCamera_Point final : public CGameObject
{
public:
	typedef struct tagCameraPointDesc
	{
		_float4 vPosition;
	}CAMERAPOINTDESC;
private:
	explicit CCamera_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCamera_Point() = default;


public:
	const _float4& Get_Position();


public:
	void Set_Position(_float4 _vPosition);


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool RayIntersects(_float4 vOrigin, _float4 vDirection, _Inout_ _float& fDist);



#ifdef _DEBUG
	//µð¹ö±ë ·»´õ »ö º¯°æ
	void Set_Collider_Color(_float4 _vColor);
#endif // _DEBUG


private:


private:
	CRenderer* m_pRenderer = { nullptr };
	CCollider* m_pCollider = { nullptr };
	
private:
	HRESULT Add_Components();


public:
	static CCamera_Point* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);

	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free();
};

END
