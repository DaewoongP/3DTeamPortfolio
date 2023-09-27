#pragma once

/* =============================================== */
// 
//	정 : 전대인
//	부 :
//
/* =============================================== */

#include "GameObject.h"
#include "VIBuffer_Rect_Color_Instance.h"
#include "VIBuffer_Color_Index_Instance.h"
#include "Modules.h"

BEGIN(Engine)
class CRenderer;
class CTexture;
class CShader;
class CModel;
class CTransform;
class CMotionBlurInstance;
class CVIBuffer_Rect_Particle_Instance;
END

BEGIN(Engine)

class ENGINE_DLL CParticleSystem : public CGameObject
{
public:
	// ALIVE : 업데이트 되고 있는 파티클
	// DELAY : 대기시간이 채워지면 ALIVE로 넘어감
	// WAIT : 파티클 풀 장소(루프on일 경우 파티클이 죽었을 때 여기로 온다.)
	// DEAD : 죽은 파티클 부활 할 수 없음.
	enum STATE { ALIVE, DELAY, WAIT, DEAD, STATE_END };

public:
	wstring Get_ParticleTag() { return m_szParticleTag; }
	void Set_ParticleTag(wstring szParticleTag) { m_szParticleTag = szParticleTag; }

protected:
	explicit CParticleSystem(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	explicit CParticleSystem(const CParticleSystem& _rhs);
	virtual ~CParticleSystem() = default;

public:
	_uint Get_DeadCount() { return m_Particles[DEAD].size(); }
	_bool Is_AllDead() { return (m_Particles[ALIVE].empty() && m_Particles[DELAY].empty()); }
	_bool IsEnable() { return m_MainModuleDesc.isEnable; }
	_bool Is_AliveEmpty() { return m_Particles[ALIVE].empty(); }
	void Add_Notify(PT_NOTIFY PTNotify);

	list<PARTICLE>::iterator Begin(STATE eState) { return m_Particles[eState].begin(); }
	list<PARTICLE>::iterator End(STATE eState) { return m_Particles[eState].end(); }		
	void PlayEvent(const PARTICLE_IT& Particle_iter);

public:
	virtual HRESULT Initialize_Prototype(const _tchar* _pDirectoryPath, _uint iLevel);
	virtual HRESULT Initialize(void* _pArg) override;
	void Tick(_float _fTimeDelta);
	void Late_Tick(_float _fTimeDelta) override;
	HRESULT Render();

public: 
	// ex) MAIN_MODULE& MainModule = m_pParticleSystem->Get_MainModuleRef();
	MAIN_MODULE&					Get_MainModuleRef() { return m_MainModuleDesc; }
	EMISSION_MODULE&				Get_EmissionModuleRef() { return m_EmissionModuleDesc; }
	SHAPE_MODULE&					Get_ShapeModuleRef() { return m_ShapeModuleDesc; }
	RENDERER_MODULE&				Get_RendererModuleRef() { return m_RendererModuleDesc; }
	VELOCITY_OVER_LIFETIME& Get_VelocityOverLifetimeModuleRer() { return m_VelocityOverLifeTimeModuleDesc; }
	COLOR_OVER_LIFETIME&			Get_ColorOverLifetimeModuleRef() { return m_ColorOverLifeTimeModuleDesc; }
	SIZE_OVER_LIFETIME&				Get_SizeOverLifetimeModuleRef() { return m_SizeOverLifeTimeModuleDesc; }
	ROTATION_OVER_LIFETIME_MODULE&	Get_RotationOverLifetimeModuleRef() { return m_RotationOverLifetimeModuleDesc; }

	
public:
	void Play(_float3 vPosition = _float3());
	void Stop();
	virtual void Restart();
	// Stop vs Disable
	// Stop : 새로운 파티클의 생성을 막는다.(Alive와 Delay에 남은 파티클들이 여전히 연산을 진행하고있음.)
	// Disable : 파티클 객체를 비활성화 시킨다.(Tick과 Late Tick을 강제로 멈춤)
public:
	void Enable();
	void Enable(_float3 vPos);
	void Disable();

protected:
	HRESULT Save(const _tchar* _pDirectoyPath);
	HRESULT Load(const _tchar* _pDirectoyPath);

public:
	void Reset_Particle(PARTICLE_IT& _particle_iter);
	void Reset_AllParticles();
	void ResetStartPosition(PARTICLE_IT& _particle_iter);

protected:
	// WAIT->DELAY로 이동.
	PARTICLE_IT Wating_One_Particle();

	// 빌보드
	_float4x4 LookAt(_float3 vPos, _float3 _vTarget, _bool _isDeleteY = false);

protected:
	// 시작할 때 자동재생 할건지 결정함.
	void Play_On_Awake();

	// 파티클 시뮬레이션 속도계산
	void Simulation_Speed(_float& fTimeDelta);

	// 인스턴스 수에 따라 resize, reserve함수로 메모리 할당.
	void Resize_Container(_uint iNumInstance);
	
protected:
	void Action_By_Age();
	void Action_By_Duration();
	void Action_By_StopOption();

protected:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

protected:
	MAIN_MODULE						m_MainModuleDesc;
	EMISSION_MODULE					m_EmissionModuleDesc;
	SHAPE_MODULE					m_ShapeModuleDesc;
	VELOCITY_OVER_LIFETIME			m_VelocityOverLifeTimeModuleDesc;
	COLOR_OVER_LIFETIME				m_ColorOverLifeTimeModuleDesc;
	SIZE_OVER_LIFETIME				m_SizeOverLifeTimeModuleDesc;
	ROTATION_OVER_LIFETIME_MODULE	m_RotationOverLifetimeModuleDesc;
	TEXTURE_SHEET_ANIMATION			m_TextureSheetAnimationModuleDesc;
	NOISE_MODULE					m_NoiseModuleDesc;
	RENDERER_MODULE					m_RendererModuleDesc;

protected: 
	//CVIBuffer_Geometry* m_pShapeBuffer = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CTexture* m_pMainTexture = { nullptr }; // 출력에 사용 될 텍스처
	CTexture* m_pClipTexture = { nullptr }; // 알파테스트에 사용될 텍스처
	CTexture* m_pGradientTexture = { nullptr }; // 알파테스트에 사용될 텍스처
	CTexture* m_pNormalTexture = { nullptr }; // 텍스처시트에 사용될 텍스처
	CVIBuffer_Rect_Particle_Instance* m_pBuffer = { nullptr };
	CShader* m_pShader = { nullptr };
	CModel* m_pModel = { nullptr };
	//CMotionBlurInstance* m_pMotionBlurInstance = { nullptr };

protected:
	list<PARTICLE> m_Particles[STATE_END];
	vector<VTXPARTICLEINSTANCE>  m_ParticleMatrices;
	function<void()> m_StopAction;
	_uint m_iLevel = { 0 };
	_bool m_isStop = { false };
	wstring m_szParticleTag = { TEXT("") };

public:
	static CParticleSystem* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pDirectoryPath, _uint iLevel = 0);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

	friend struct EMISSION_MODULE;
};

END

//enum MODULE {
//	MAIN, EMITTER, SHAPE, VELOCITY_OVER_LIFETIME,
//	LIMIT_VELOCITY_OVER_LIFETIME, INHERIT_VELOCITY, FORCE_OVER_LIFETIME,
//	COLOR_OVER_LIFETIME, COLOR_BY_SPEED, SIZE_OVER_LIFETIME,
//	SIZE_BY_SPEED, ROTATION_OVER_ROTATION, ROTATION_BY_SPEED,
//	EXTERNAL_FORCES, NOISE, COLLISION, TRIGGERS,
//	SUB_EMITTERS, TEXTURE_SHEET_ANIMATION, LIGHTS,
//	TRAILS, CUSTOM_DATA, RENDERER, MODULE_END
//};