#pragma once

/* =============================================== */
//	[CSelector_Degree]
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

class CSelector_Degree final : public CSelector
{
public:
	enum DEGREES { LEFT_FRONT, LEFT_BACK, LEFT_45, LEFT_90, LEFT_135, RIGHT_FRONT, RIGHT_BACK, RIGHT_45, RIGHT_90, RIGHT_135, DEGREE_END };

private:
	explicit CSelector_Degree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSelector_Degree(const CSelector_Degree& rhs);
	virtual ~CSelector_Degree() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(const _float& fTimeDelta);

public:
	HRESULT Assemble_Behavior(DEGREES eType, CBehavior* pBehavior);
	/* 각 각도에 실행 시킬 비헤비어를 넣으면 됨 */

public:
	static CSelector_Degree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSelector_Degree* Clone(void* pArg) override;
	virtual void Free() override;
};

END