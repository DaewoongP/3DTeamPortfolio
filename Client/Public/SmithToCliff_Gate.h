#pragma once
#include "MapObject.h"

BEGIN(Engine)
class CParticleSystem;
END

BEGIN(Client)

class CSmithToCliff_Gate final : public CMapObject
{
private:
	explicit CSmithToCliff_Gate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSmithToCliff_Gate(const CSmithToCliff_Gate& rhs);
	virtual ~CSmithToCliff_Gate() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CMeshEffect* m_pEffect = { nullptr };
	CMeshEffect* m_pEffect2 = { nullptr };

private:
	_uint m_iSound0 = { 0 }; // 사운드 채널 0

public:
	static CSmithToCliff_Gate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END