#pragma once

#include "Component.h"
#include "MainModule.h"

BEGIN(Engine)

class ENGINE_DLL CParticleSystem final : public CComponent
{
public:
	friend class CParticleModule;

	enum MODULE { MAIN, EMMISION, SHAPE, VELOCITY_OVER_LIFETIME,
		LIMIT_VELOCITY_OVER_LIFETIME, INHERIT_VELOCITY, FORCE_OVER_LIFETIME,
		COLOR_OVER_LIFETIME, COLOR_BY_SPEED, SIZE_OVER_LIFETIME,
		SIZE_BY_SPEED, ROTATION_OVER_ROTATION, ROTATION_BY_SPEED,
		EXTERNAL_FORCES, NOISE, COLLISION, TRIGGERS,
		SUB_EMITTERS, TEXTURE_SHEET_ANIMATION, LIGHTS,
		TRAILS, CUSTOM_DATA, RENDERER, MODULE_END};

private:
	explicit CParticleSystem(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	explicit CParticleSystem(const CParticleSystem& _rhs);
	virtual ~CParticleSystem() = default;

public:
	CParticleModule* Get_Module(MODULE _eModule) const {
		return m_Modules[_eModule];
	}
	
public:


public:
	virtual HRESULT Initialize_Prototype(const _tchar * _pDirectoyPath);
	virtual HRESULT Initialize(void* _pArg) override;
	void Tick(_float fTimeDelta, vector<CVIBuffer_Color_Instance::COLORINSTANCE>& _ColInsts);
	HRESULT Render();

public:
	// NULL이 아닌 모든 모듈을 데이터 파일로 쓴다.
	void Save(const _tchar* _pDirectoyPath);

	// 파일이 존재하면 로드한다.(없는 모듈은 저장을 안해서 파일이 없을 수 있음.)
	void Load(const _tchar* _pDirectoyPath);

private:
	CParticleModule* ModuleFactory(MODULE _eModule);

private:
	vector<CParticleModule*> m_Modules;
	
public:
	static CParticleSystem* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const _tchar * _pTextureFilePath);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END