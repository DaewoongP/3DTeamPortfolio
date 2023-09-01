#pragma once

#include "Bone.h"
#include "Mesh.h"
#include "Model.h"
#include "Shader.h"
#include "Texture.h"
#include "Collider.h"
#include "Renderer.h"
#include "Animation.h"
#include "Transform.h"
#include "RigidBody.h"
#include "Navigation.h"
#include "Bounding_OBB.h"
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
	HRESULT				Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent*	Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg);
	void				Clear_LevelResources(_uint iLevelIndex);
	HRESULT				Delete_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);

private:
	_uint	m_iNumLevels = { 0 };

private:
	typedef unordered_map<const _tchar*, class CComponent*>	PROTOTYPES;
	PROTOTYPES* m_pPrototypes = { nullptr };
	
private:
	class CComponent*	Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);

public:
	virtual void Free() override;
};

END