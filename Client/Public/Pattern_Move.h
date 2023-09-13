#pragma once

/* =============================================== */
//	[CPattern_Move]
//	: 움직임에 관련된 클래스
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "Sequence.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CPattern_Move final : public CSequence
{
private:
	explicit CPattern_Move(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPattern_Move(const CPattern_Move& rhs);
	virtual ~CPattern_Move() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(const _float& fTimeDelta) override;

public:
	HRESULT Assemble_Childs(CTransform* pTransform);

public:
	static CPattern_Move* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPattern_Move* Clone(void* pArg) override;
	virtual void Free() override;
};

END