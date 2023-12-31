#include "..\Public\ParticleSystemPool.h"
#include "Component_Manager.h"
#include "String_Manager.h"

IMPLEMENT_SINGLETON(CParticleSystemPool)

HRESULT CParticleSystemPool::Reserve_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* szParticleTag, const _tchar* szParticleDirectoryPath, _uint iNumReserveParticles)
{
	CComponent_Manager* pComponent_Manager = CComponent_Manager::GetInstance();
	Safe_AddRef(pComponent_Manager);

	_tchar szPrototypeTag[MAX_PATH] = TEXT("Prototype_GameObject_");
	lstrcat(szPrototypeTag, szParticleTag); // Particle_Rock

	// 프로토타입생성
	if (FAILED(pComponent_Manager->Add_Prototype(0, szPrototypeTag
		, CParticleSystem::Create(pDevice, pContext, szParticleDirectoryPath, 0))))
	{
		Safe_Release(pComponent_Manager);
		__debugbreak();
		return E_FAIL;
	}

	// 파티클들을 담기위한 큐
	CParticleSystemQueue* pParticleQueue = CParticleSystemQueue::Create();
	
	// Reserve하고싶은 개수만큼 포문을 돌면서 클론해서 큐에 넣는다.
	for (_uint i = 0; i < iNumReserveParticles; ++i)
	{
		CParticleSystem* pParticle = static_cast<CParticleSystem*>(pComponent_Manager->Clone_Component(0, szPrototypeTag, nullptr));
		pParticle->Set_ParticleTag(szParticleTag);
		pParticleQueue->Push_Back(pParticle);
	}

	// Particle_Rock
	m_ParticlePool.emplace(szParticleTag, pParticleQueue);

	Safe_Release(pComponent_Manager);

	return S_OK;
}

void CParticleSystemPool::Play_Particle(const _tchar* szParticleTag, _float3 vWorldPosition)
{
	CComponent_Manager* pComponent_Manager = CComponent_Manager::GetInstance();
	Safe_AddRef(pComponent_Manager);
	// Particle_Rock
	CParticleSystemQueue* pParticleQueue = Find_ParticleQueue(szParticleTag);
	if (nullptr == pParticleQueue)
	{
		Safe_Release(pComponent_Manager);
		return;
	}
	
	_tchar szComponentTag[MAX_PATH] = TEXT("GameObject_");
	CString_Manager* pString_Manager = CString_Manager::GetInstance();
	Safe_AddRef(pString_Manager);

	lstrcat(szComponentTag, szParticleTag);
	lstrcat(szComponentTag, Generate_HashtagW().c_str());

	// 현재 실행중인 레이어에 할당.
	CParticleSystem* pParticle = pParticleQueue->Pop_Front();
	if (nullptr == pParticle)
	{
		_tchar szPrototypeTag[MAX_PATH] = TEXT("Prototype_GameObject_");
		lstrcat(szPrototypeTag, szParticleTag);
		pParticle = static_cast<CParticleSystem*>(pComponent_Manager->Clone_Component(0, szPrototypeTag, nullptr));
		pParticle->Set_ParticleTag(szParticleTag);
	}

	pParticle->Play(vWorldPosition);
	pParticle->Set_ObjEvent(CGameObject::OBJ_NONE);
	pComponent_Manager->Add_Component(pParticle, 0, TEXT("Layer_Particle"), pString_Manager->Make_WChar(szComponentTag));
	pComponent_Manager->Set_CurrentScene(TEXT("Scene_Main"), true);

	Safe_Release(pString_Manager);
	Safe_Release(pComponent_Manager);
}

void CParticleSystemPool::Play_Particle(const _tchar* szParticleTag, _float3 vPosition, _float3 vDir)
{
	CComponent_Manager* pComponent_Manager = CComponent_Manager::GetInstance();
	Safe_AddRef(pComponent_Manager);
	// Particle_Rock
	CParticleSystemQueue* pParticleQueue = Find_ParticleQueue(szParticleTag);
	if (nullptr == pParticleQueue)
	{
		Safe_Release(pComponent_Manager);
		return;
	}

	_tchar szComponentTag[MAX_PATH] = TEXT("GameObject_");
	CString_Manager* pString_Manager = CString_Manager::GetInstance();
	Safe_AddRef(pString_Manager);

	lstrcat(szComponentTag, szParticleTag);
	lstrcat(szComponentTag, Generate_HashtagW().c_str());

	// 현재 실행중인 레이어에 할당.
	CParticleSystem* pParticle = pParticleQueue->Pop_Front();
	if (nullptr == pParticle)
	{
		_tchar szPrototypeTag[MAX_PATH] = TEXT("Prototype_GameObject_");
		lstrcat(szPrototypeTag, szParticleTag);
		pParticle = static_cast<CParticleSystem*>(pComponent_Manager->Clone_Component(0, szPrototypeTag, nullptr));
		pParticle->Set_ParticleTag(szParticleTag);
	}
	pParticle->Get_ShapeModuleRef().Set_ShapeLook(vPosition, vDir + vPosition);
	pParticle->Get_ShapeModuleRef().ShapeMatrix = pParticle->Get_ShapeModuleRef().ShapeMatrixInit * pParticle->Get_ShapeModuleRef().ShapeMatrix;

	pParticle->Play(vPosition);
	pParticle->Set_ObjEvent(CGameObject::OBJ_NONE);
	pComponent_Manager->Add_Component(pParticle, 0, TEXT("Layer_Particle"), pString_Manager->Make_WChar(szComponentTag));
	pComponent_Manager->Set_CurrentScene(TEXT("Scene_Main"), true);

	Safe_Release(pString_Manager);
	Safe_Release(pComponent_Manager);
}


void CParticleSystemPool::Play_Particle(const _tchar* szParticleTag, _float4x4 PositionMatrix,_float4x4 ObjectMatrix)
{
	CComponent_Manager* pComponent_Manager = CComponent_Manager::GetInstance();
	Safe_AddRef(pComponent_Manager);
	// Particle_Rock
	CParticleSystemQueue* pParticleQueue = Find_ParticleQueue(szParticleTag);
	if (nullptr == pParticleQueue)
	{
		Safe_Release(pComponent_Manager);
		return;
	}

	_tchar szComponentTag[MAX_PATH] = TEXT("GameObject_");
	CString_Manager* pString_Manager = CString_Manager::GetInstance();
	Safe_AddRef(pString_Manager);

	lstrcat(szComponentTag, szParticleTag);
	lstrcat(szComponentTag, Generate_HashtagW().c_str());

	// 현재 실행중인 레이어에 할당.
	CParticleSystem* pParticle = pParticleQueue->Pop_Front();
	if (nullptr == pParticle)
	{
		_tchar szPrototypeTag[MAX_PATH] = TEXT("Prototype_GameObject_");
		lstrcat(szPrototypeTag, szParticleTag);
		pParticle = static_cast<CParticleSystem*>(pComponent_Manager->Clone_Component(0, szPrototypeTag, nullptr));
		pParticle->Set_ParticleTag(szParticleTag);
	}
	ObjectMatrix.m[3][0] = 0;
	ObjectMatrix.m[3][1] = 0;
	ObjectMatrix.m[3][2] = 0;

	pParticle->Get_ShapeModuleRef().ShapeMatrix = pParticle->Get_ShapeModuleRef().ShapeMatrixInit * ObjectMatrix;
	pParticle->Play(PositionMatrix.Translation());
	pParticle->Set_ObjEvent(CGameObject::OBJ_NONE);
	pComponent_Manager->Add_Component(pParticle, 0, TEXT("Layer_Particle"), pString_Manager->Make_WChar(szComponentTag));
	pComponent_Manager->Set_CurrentScene(TEXT("Scene_Main"), true);

	Safe_Release(pString_Manager);
	Safe_Release(pComponent_Manager);
}

//Trace용도임.
void CParticleSystemPool::Play_Particle(const _tchar* szParticleTag, _float4x4 OffsetMatrix, const _float4x4* pBindBoneMatrix, _float4x4 PivotMatrix, const _float4x4* pWorldMatrix)
{
	CComponent_Manager* pComponent_Manager = CComponent_Manager::GetInstance();
	Safe_AddRef(pComponent_Manager);
	// Particle_Rock
	CParticleSystemQueue* pParticleQueue = Find_ParticleQueue(szParticleTag);
	if (nullptr == pParticleQueue)
	{
		Safe_Release(pComponent_Manager);
		return;
	}

	_tchar szComponentTag[MAX_PATH] = TEXT("GameObject_");
	CString_Manager* pString_Manager = CString_Manager::GetInstance();
	Safe_AddRef(pString_Manager);

	lstrcat(szComponentTag, szParticleTag);
	lstrcat(szComponentTag, Generate_HashtagW(false, 25).c_str());

	// 현재 실행중인 레이어에 할당.
	CParticleSystem* pParticle = pParticleQueue->Pop_Front();
	if (nullptr == pParticle)
	{
		_tchar szPrototypeTag[MAX_PATH] = TEXT("Prototype_GameObject_");
		lstrcat(szPrototypeTag, szParticleTag);
		pParticle = static_cast<CParticleSystem*>(pComponent_Manager->Clone_Component(0, szPrototypeTag, nullptr));
		pParticle->Set_ParticleTag(szParticleTag);
	}

	_float4x4 PositionMatrix = OffsetMatrix * (*pBindBoneMatrix) * PivotMatrix * (*pWorldMatrix);
	_float4x4 WorldMatrix = *pWorldMatrix;

	WorldMatrix.m[3][0] = 0;
	WorldMatrix.m[3][1] = 0;
	WorldMatrix.m[3][2] = 0;

	pParticle->Get_ShapeModuleRef().ShapeMatrix = pParticle->Get_ShapeModuleRef().ShapeMatrixInit * WorldMatrix;
	pParticle->Play(PositionMatrix.Translation());
	pParticle->Set_ObjEvent(CGameObject::OBJ_NONE);
	pParticle->Set_TraceOption(OffsetMatrix, pBindBoneMatrix, PivotMatrix, pWorldMatrix);
	pComponent_Manager->Add_Component(pParticle, 0, TEXT("Layer_Particle"), pString_Manager->Make_WChar(szComponentTag));
	pComponent_Manager->Set_CurrentScene(TEXT("Scene_Main"), true);

	Safe_Release(pString_Manager);
	Safe_Release(pComponent_Manager);
}

void CParticleSystemPool::Return_Particle(CParticleSystem* pParticle)
{
	// 죽은 파티클을 다시 큐에 담는다
	Safe_AddRef(pParticle);
	Find_ParticleQueue((pParticle->Get_ParticleTag()).c_str())->Push_Back(pParticle);
}

CParticleSystemQueue* CParticleSystemPool::Find_ParticleQueue(const _tchar* szParticleTag)
{
	auto iter = find_if(m_ParticlePool.begin(), m_ParticlePool.end(), CTag_Finder(szParticleTag));

	if (m_ParticlePool.end() == iter)
		return nullptr;

	return iter->second;
}

void CParticleSystemPool::Free()
{
	for (auto& pair : m_ParticlePool)
	{
		Safe_Release(pair.second);
	}

	m_ParticlePool.clear();
}
