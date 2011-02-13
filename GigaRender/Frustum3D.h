#pragma once

class BaseCamera;

class Frustum3D
{
private:
	D3DMATERIAL9 m_mtrlWhite;
	IDirect3DVertexBuffer9 *m_pPlaneVB[6]; // VBs to visualize the view frustum

	struct PlaneVertex
	{
		D3DXVECTOR3 p;
		DWORD       color;

		static const unsigned int FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
	};

	struct CULLINFO
	{
		D3DXVECTOR3 vecFrustum[8];    // corners of the view frustum
		D3DXPLANE planeFrustum[6];    // planes of the view frustum
	};

	void updateCullInfo(CULLINFO* pCullInfo, const D3DXMATRIX* pMatView, const D3DXMATRIX* pMatProj);

public:
	Frustum3D();

	void Initialize(IDirect3DDevice9 &device);
	void Destroy();
	void Update(BaseCamera &camera);
	void Render(IDirect3DDevice9 &device);
};