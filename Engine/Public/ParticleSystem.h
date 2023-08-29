#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CParticleSystem final : public CComponent
{
private:
	explicit CParticleSystem(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	explicit CParticleSystem(const CParticleSystem& _rhs);
	virtual ~CParticleSystem() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar * _pTextureFilePath);
	virtual HRESULT Initialize(void* _pArg) override;

private:
	class CMainModule* m_pMainModule = { nullptr };
	
public:
	static CParticleSystem* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const _tchar * _pTextureFilePath);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END