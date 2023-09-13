#pragma once

#ifdef _DEBUG
namespace Engine
{
	static void cout_float3(const char* pName, XMFLOAT3 _vVector)
	{
		cout << pName << " x : " << _vVector.x << "y : " << _vVector.y << "z : " << _vVector.z << endl;
	}
}
#endif // _DEBUG
