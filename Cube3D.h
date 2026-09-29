#pragma once
#include <d3d11.h>
#include "Camera.h"
namespace Cube
{
	XMFLOAT3 GetPosition();
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Finalize(void);
	void Update(void);
	void Draw(void);
}
