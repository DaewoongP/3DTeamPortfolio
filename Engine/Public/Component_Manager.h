#pragma once

/* =============================================== */
// 
//	정 : 박대웅
//	부 :
//
//
// 헤더추가는 편하게 스윽 처리하십쇼
/* =============================================== */

#include "Bone.h"
#include "Mesh.h"
#include "Model.h"
#include "Phase.h"
#include "Trail.h"
#include "Shader.h"
#include "Status.h"
#include "Attack.h"
#include "Ignite.h"
#include "Health.h"
#include "Defence.h"
#include "Texture.h"
#include "Collider.h"
#include "Renderer.h"
#include "Behavior.h"
#include "Selector.h"
#include "Sequence.h"
#include "CoolTime.h"
#include "Animation.h"
#include "Transform.h"
#include "RigidBody.h"
#include "Decorator.h"
#include "Model_LOD.h"
#include "MeshParts.h"
#include "MeshEffect.h"
#include "Navigation.h"
#include "MeshEffect.h"
#include "BlackBoard.h"
#include "CustomModel.h"
#include "RandomChoose.h"
#include "Bounding_OBB.h"
#include "RootBehavior.h"
#include "Bounding_AABB.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Line.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Cell.h"
#include "Mesh_Instance.h"
#include "Model_Instance.h"
#include "MotionBlurInst.h"
#include "VIBuffer_Cloth.h"
#include "ParticleSystem.h"
#include "Bounding_Sphere.h"
#include "Shape_GeoSphere.h"
#include "Texture_Filpbook.h"
#include "VIBuffer_Terrain.h"
#include "Debug_Render_Box.h"
#include "VIBuffer_Triangle.h"
#include "VIBuffer_Geometry.h"
#include "VIBuffer_Instance.h"
#include "Debug_Render_Ring.h"
#include "VIBuffer_UVSphere.h"
#include "VIBuffer_GeoSphere.h"
#include "VIBuffer_Rect_Trail.h"
#include "CharacterController.h"
#include "Debug_Render_Sphere.h"
#include "Debug_Render_Capsule.h"
#include "VIBuffer_Rect_Dynamic.h"
#include "VIBuffer_Rect_Instance.h"
#include "VIBuffer_Point_Instance.h"
#include "VIBuffer_Rect_Color_Instance.h"
#include "VIBuffer_Point_Color_Instance.h"
#include "VIBuffer_Rect_Particle_Instance.h"
#include "VIBuffer_Rect_Color_Index_Instance.h"
//---------------------- 눈치 챙겨!!
#include "ConvexMesh.h"
#include "TriangleMesh.h"
#include "UI_Button.h"
#include "UI_Progress1.h"

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)

private:
	explicit CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	const _tchar* Get_CurrentSceneTag() const { return m_szCurrentSceneTag; }
	// 씬변경이 루프내에서 처리되지 않게 해주세요
	// 프레임 하락에 많은 영향을 끼칩니다.
	void Set_CurrentScene(const _tchar* pSceneTag, _bool isSimulation);

public:
	void	Tick(_float fTimeDelta);
	void	Late_Tick(_float fTimeDelta);

public:
	HRESULT				Reserve_Containers(_uint iNumLevels);
	// 프로토타입 생성
	HRESULT				Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype, _bool isFailedSkip = false);
	// Tick에 컴포넌트가 돌아가게끔 clone 처리
	HRESULT				Add_Component(_uint iPrototypeLevelIndex, _uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, const _tchar* pComponentTag, void* pArg);
	HRESULT				Add_Component(class CComponent* pComponent, _uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg);
	void				Clear_LevelResources(_uint iLevelIndex);
	HRESULT				Delete_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag);

	class CComponent* Find_Component_In_Layer(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag);
	unordered_map<const _tchar*, class CComponent*>* Find_Components_In_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
	class CComponent* Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
	HRESULT				Clear_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

private:
	typedef unordered_map<const _tchar*, class CLayer*>		LAYERS;
	typedef unordered_map<const _tchar*, class CComponent*>	PROTOTYPES;

private:
	PROTOTYPES*			m_pPrototypes = { nullptr };
	LAYERS*				m_pLayers = { nullptr };
	_uint				m_iNumLevels = { 0 };

private:
	_tchar				m_szCurrentSceneTag[MAX_PATH] = TEXT("");
	_bool				m_isSimulation = { false };
	_bool				m_isChanged = { false };
	LAYERS*				m_pCurrentLayers = { nullptr };

private:
	void Update_CurrentScene();

public:
	virtual void Free() override;
};

END