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
#include "Plane.h"
#include "Model.h"
#include "Alarm.h"
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
#include "Animation.h"
#include "Transform.h"
#include "RigidBody.h"
#include "Decorator.h"
#include "MeshParts.h"
#include "Navigation.h"
#include "BlackBoard.h"
#include "CustomModel.h"
#include "Bounding_OBB.h"
#include "RootBehavior.h"
#include "Bounding_AABB.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Line.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Cell.h"
#include "ParticleSystem.h"
#include "Bounding_Sphere.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Triangle.h"
#include "VIBuffer_Instance.h"
#include "VIBuffer_Rect_Trail.h"
#include "CharacterController.h"
#include "VIBuffer_Rect_Dynamic.h"
#include "VIBuffer_Rect_Instance.h"
#include "VIBuffer_Point_Instance.h"
#include "VIBuffer_Rect_Color_Instance.h"
#include "VIBuffer_Point_Color_Instance.h"

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)

private:
	explicit CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT				Reserve_Containers(_uint iNumLevels);
	// 프로토타입 생성
	HRESULT				Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	// Tick에 컴포넌트가 돌아가게끔 clone 처리
	HRESULT				Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, const _tchar* pComponentTag, void* pArg);
	class CComponent*	Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg);
	void				Clear_LevelResources(_uint iLevelIndex);
	HRESULT				Delete_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag);

	class CComponent*	Find_Component_In_Layer(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag);
	class CLayer*		Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
	HRESULT				Clear_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

public:
	void	Tick(_float fTimeDelta);
	void	Late_Tick(_float fTimeDelta);

private:
	typedef unordered_map<const _tchar*, class CLayer*>		LAYERS;
	typedef unordered_map<const _tchar*, class CComponent*>	PROTOTYPES;

private:
	PROTOTYPES*			m_pPrototypes = { nullptr };
	LAYERS*				m_pLayers = { nullptr };
	_uint				m_iNumLevels = { 0 };

private:
	class CComponent*	Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);

public:
	virtual void Free() override;
};

END