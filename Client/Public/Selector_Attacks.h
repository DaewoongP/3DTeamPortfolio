#pragma once

/* =============================================== */
//	[CSelector_Attacks]
//	: 근접 공격을 랜덤 실행하고 프로테고 방어시 그로기 애니메이션 까지 
//	: 진행하는 클래스 
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "Selector.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CSelector_Attacks final : public CSelector
{
private:
	explicit CSelector_Attacks(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSelector_Attacks(const CSelector_Attacks& rhs);
	virtual ~CSelector_Attacks() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(const _float& fTimeDelta);

public:
	/* 각 각도에 실행 시킬 비헤비어를 넣으면 됨 */
	HRESULT Assemble_Childs(CBehavior* pBehavior);

public:
	static CSelector_Attacks* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSelector_Attacks* Clone(void* pArg) override;
	virtual void Free() override;
};

END