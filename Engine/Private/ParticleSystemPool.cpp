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

	// ������Ÿ�Ի���
	if (FAILED(pComponent_Manager->Add_Prototype(0, szPrototypeTag
		, CParticleSystem::Create(pDevice, pContext, szParticleDirectoryPath, 0))))
	{
		Safe_Release(pComponent_Manager);
		__debugbreak();
		return E_FAIL;
	}

	// ��ƼŬ���� ������� ť
	CParticleSystemQueue* pParticleQueue = CParticleSystemQueue::Create();
	
	// Reserve�ϰ���� ������ŭ ������ ���鼭 Ŭ���ؼ� ť�� �ִ´�.
	for (_uint i = 0; i < iNumReserveParticles; ++i)
	{
		CParticleSystem* pParticle = dynamic_cast<CParticleSystem*>(pComponent_Manager->Clone_Component(0, szPrototypeTag, nullptr));
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

	// ���� �������� ���̾ �Ҵ�.
	CParticleSystem* pParticle = pParticleQueue->Pop_Front();
	if (nullptr == pParticle)
	{
		_tchar szPrototypeTag[MAX_PATH] = TEXT("Prototype_GameObject_");
		lstrcat(szPrototypeTag, szParticleTag);
		pParticle = dynamic_cast<CParticleSystem*>(pComponent_Manager->Clone_Component(0, szPrototypeTag, nullptr));
		pParticle->Set_ParticleTag(szParticleTag);
	}

	pParticle->Play(vWorldPosition);
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

	// ���� �������� ���̾ �Ҵ�.
	CParticleSystem* pParticle = pParticleQueue->Pop_Front();
	if (nullptr == pParticle)
	{
		_tchar szPrototypeTag[MAX_PATH] = TEXT("Prototype_GameObject_");
		lstrcat(szPrototypeTag, szParticleTag);
		pParticle = dynamic_cast<CParticleSystem*>(pComponent_Manager->Clone_Component(0, szPrototypeTag, nullptr));
		pParticle->Set_ParticleTag(szParticleTag);
	}

	pParticle->Play(PositionMatrix.Translation());

	ObjectMatrix.m[3][0] = 0;
	ObjectMatrix.m[3][1] = 0;
	ObjectMatrix.m[3][2] = 0;

	pParticle->Get_ShapeModuleRef().ShapeMatrix *= ObjectMatrix;
	pParticle->Set_ObjEvent(CGameObject::OBJ_NONE);
	pComponent_Manager->Add_Component(pParticle, 0, TEXT("Layer_Particle"), pString_Manager->Make_WChar(szComponentTag));
	pComponent_Manager->Set_CurrentScene(TEXT("Scene_Main"), true);

	Safe_Release(pString_Manager);
	Safe_Release(pComponent_Manager);
}

//Trace�뵵��.
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
	lstrcat(szComponentTag, Generate_HashtagW().c_str());

	// ���� �������� ���̾ �Ҵ�.
	CParticleSystem* pParticle = pParticleQueue->Pop_Front();
	if (nullptr == pParticle)
	{
		_tchar szPrototypeTag[MAX_PATH] = TEXT("Prototype_GameObject_");
		lstrcat(szPrototypeTag, szParticleTag);
		pParticle = dynamic_cast<CParticleSystem*>(pComponent_Manager->Clone_Component(0, szPrototypeTag, nullptr));
		pParticle->Set_ParticleTag(szParticleTag);
	}

	_float4x4 PositionMatrix = OffsetMatrix * (*pBindBoneMatrix) * PivotMatrix * (*pWorldMatrix);


	pParticle->Play(PositionMatrix.Translation());

	_float4x4 WorldMatrix = *pWorldMatrix;

	WorldMatrix.m[3][0] = 0;
	WorldMatrix.m[3][1] = 0;
	WorldMatrix.m[3][2] = 0;

	pParticle->Get_ShapeModuleRef().ShapeMatrix *= WorldMatrix;
	pParticle->Set_ObjEvent(CGameObject::OBJ_NONE);
	pParticle->Set_TraceOption(OffsetMatrix, pBindBoneMatrix, PivotMatrix, pWorldMatrix);
	pComponent_Manager->Add_Component(pParticle, 0, TEXT("Layer_Particle"), pString_Manager->Make_WChar(szComponentTag));
	pComponent_Manager->Set_CurrentScene(TEXT("Scene_Main"), true);

	Safe_Release(pString_Manager);
	Safe_Release(pComponent_Manager);
}

void CParticleSystemPool::Return_Particle(CParticleSystem* pParticle)
{
	// ���� ��ƼŬ�� �ٽ� ť�� ��´�
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
