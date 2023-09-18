
#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTrail;
END

BEGIN(Client)

class CDefault_MagicTraill_Effect final : public CGameObject
{
public:
	typedef struct InitDesc {
		_float3 vInitPosition;
	}INITDESC;
	
private:
	CDefault_MagicTraill_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDefault_MagicTraill_Effect(const CDefault_MagicTraill_Effect& rhs);
	virtual ~CDefault_MagicTraill_Effect() = default;

public:
	void Set_Position(_float3 vPos);
	//상위에서 틱마다 불러주는 함수로 시작과 끝지점을 넣으면 알아서 행동개시
	void Action_Magic(TRAILACTIONTYPE eType,_float3 vStartPos,_float3 vEndPos,_float fTimeAcc, _float fTimeDelta);

public:
	virtual HRESULT Initialize_Prototype(const _tchar* wszFilePath,_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private: /* For. Component */
	CTrail* m_pTrail = { nullptr };

private:
	CTransform* m_pTrailTransform = { nullptr };
	
private:
	_uint m_iLevel = { 0 };
	_tchar	m_wszName[MAX_PATH] = {};

private:
	void	Stright_Move(_float3 vStartPos, _float3 vEndPos, _float fTimeAcc, _float fTimeDelta);
	void	Spin_Move(_float3 vStartPos, _float3 vEndPos, _float fTimeAcc, _float fTimeDelta);

	HRESULT Add_Components();

public:
	static CDefault_MagicTraill_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* wszFilePath, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};

END

