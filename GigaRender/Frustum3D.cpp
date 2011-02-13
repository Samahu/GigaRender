#include "dxstdafx.h"
#include "Frustum3D.h"
#include "BaseCamera.h"

void Frustum3D::updateCullInfo(CULLINFO* pCullInfo, const D3DXMATRIX* pMatView, const D3DXMATRIX* pMatProj)
{
    D3DXMATRIX mat;

    D3DXMatrixMultiply( &mat, pMatView, pMatProj );
    D3DXMatrixInverse( &mat, NULL, &mat );

    pCullInfo->vecFrustum[0] = D3DXVECTOR3(-1.0f, -1.0f,  0.0f); // xyz
    pCullInfo->vecFrustum[1] = D3DXVECTOR3( 1.0f, -1.0f,  0.0f); // Xyz
    pCullInfo->vecFrustum[2] = D3DXVECTOR3(-1.0f,  1.0f,  0.0f); // xYz
    pCullInfo->vecFrustum[3] = D3DXVECTOR3( 1.0f,  1.0f,  0.0f); // XYz
    pCullInfo->vecFrustum[4] = D3DXVECTOR3(-1.0f, -1.0f,  1.0f); // xyZ
    pCullInfo->vecFrustum[5] = D3DXVECTOR3( 1.0f, -1.0f,  1.0f); // XyZ
    pCullInfo->vecFrustum[6] = D3DXVECTOR3(-1.0f,  1.0f,  1.0f); // xYZ
    pCullInfo->vecFrustum[7] = D3DXVECTOR3( 1.0f,  1.0f,  1.0f); // XYZ

    for( INT i = 0; i < 8; i++ )
        D3DXVec3TransformCoord( &pCullInfo->vecFrustum[i], &pCullInfo->vecFrustum[i], &mat );

    D3DXPlaneFromPoints( &pCullInfo->planeFrustum[0], &pCullInfo->vecFrustum[0], 
        &pCullInfo->vecFrustum[1], &pCullInfo->vecFrustum[2] ); // Near
    D3DXPlaneFromPoints( &pCullInfo->planeFrustum[1], &pCullInfo->vecFrustum[6], 
        &pCullInfo->vecFrustum[7], &pCullInfo->vecFrustum[5] ); // Far
    D3DXPlaneFromPoints( &pCullInfo->planeFrustum[2], &pCullInfo->vecFrustum[2], 
        &pCullInfo->vecFrustum[6], &pCullInfo->vecFrustum[4] ); // Left
    D3DXPlaneFromPoints( &pCullInfo->planeFrustum[3], &pCullInfo->vecFrustum[7], 
        &pCullInfo->vecFrustum[3], &pCullInfo->vecFrustum[5] ); // Right
    D3DXPlaneFromPoints( &pCullInfo->planeFrustum[4], &pCullInfo->vecFrustum[2], 
        &pCullInfo->vecFrustum[3], &pCullInfo->vecFrustum[6] ); // Top
    D3DXPlaneFromPoints( &pCullInfo->planeFrustum[5], &pCullInfo->vecFrustum[1], 
        &pCullInfo->vecFrustum[0], &pCullInfo->vecFrustum[4] ); // Bottom
}

Frustum3D::Frustum3D()
{
	ZeroMemory(m_pPlaneVB, sizeof(m_pPlaneVB));

	//D3DUtil_InitMaterial(m_mtrlWhite, 1.0f, 1.0f, 1.0f, 0.5f); // white
	ZeroMemory(&m_mtrlWhite, sizeof(D3DMATERIAL9));
    m_mtrlWhite.Diffuse.r = m_mtrlWhite.Ambient.r = 1.0f;
    m_mtrlWhite.Diffuse.g = m_mtrlWhite.Ambient.g = 1.0f;
    m_mtrlWhite.Diffuse.b = m_mtrlWhite.Ambient.b = 1.0f;
    m_mtrlWhite.Diffuse.a = m_mtrlWhite.Ambient.a = 0.5f;
}

void Frustum3D::Initialize(IDirect3DDevice9 &device)
{
	HRESULT hr;

	// Create VBs for frustum planes
    for( int iPlane = 0; iPlane < 6; iPlane++ )
        V(device.CreateVertexBuffer(
			4 * sizeof(PlaneVertex),
			D3DUSAGE_WRITEONLY,
			PlaneVertex::FVF,
			D3DPOOL_MANAGED,
			&m_pPlaneVB[iPlane],
			NULL));
}

void Frustum3D::Destroy()
{
	for( int iPlane = 0; iPlane < 6; iPlane++ )
        SAFE_RELEASE( m_pPlaneVB[iPlane] );
}

void Frustum3D::Update(BaseCamera &camera)
{
	static CULLINFO m_cullinfo;
	updateCullInfo(&m_cullinfo, camera.GetViewMatrix(), camera.GetProjMatrix());

	HRESULT hr;

    for( int iPlane = 0; iPlane < 6; iPlane++ )
    {
        PlaneVertex* v;
        V(m_pPlaneVB[iPlane]->Lock( 0, 0, (void**)&v, 0 ));
        switch( iPlane )
        {
        case 0: // near
            v[0].p = m_cullinfo.vecFrustum[0];
            v[1].p = m_cullinfo.vecFrustum[1];
            v[2].p = m_cullinfo.vecFrustum[2];
            v[3].p = m_cullinfo.vecFrustum[3];
            v[0].color = v[1].color = v[2].color = v[3].color = 0x80505050;
            break;
        case 1: // far
            v[0].p = m_cullinfo.vecFrustum[4];
            v[1].p = m_cullinfo.vecFrustum[6];
            v[2].p = m_cullinfo.vecFrustum[5];
            v[3].p = m_cullinfo.vecFrustum[7];
            v[0].color = v[1].color = v[2].color = v[3].color = 0x80404040;
            break;
        case 2: // left
            v[0].p = m_cullinfo.vecFrustum[0];
            v[1].p = m_cullinfo.vecFrustum[2];
            v[2].p = m_cullinfo.vecFrustum[4];
            v[3].p = m_cullinfo.vecFrustum[6];
            v[0].color = v[1].color = v[2].color = v[3].color = 0x80404040;
            break;
        case 3:
            v[0].p = m_cullinfo.vecFrustum[1];
            v[1].p = m_cullinfo.vecFrustum[3];
            v[2].p = m_cullinfo.vecFrustum[5];
            v[3].p = m_cullinfo.vecFrustum[7];
            v[0].color = v[1].color = v[2].color = v[3].color = 0x80606060;
            break;
        case 4:
            v[0].p = m_cullinfo.vecFrustum[2];
            v[1].p = m_cullinfo.vecFrustum[3];
            v[2].p = m_cullinfo.vecFrustum[6];
            v[3].p = m_cullinfo.vecFrustum[7];
            v[0].color = v[1].color = v[2].color = v[3].color = 0x80505050;
            break;
        case 5:
            v[0].p = m_cullinfo.vecFrustum[0];
            v[1].p = m_cullinfo.vecFrustum[1];
            v[2].p = m_cullinfo.vecFrustum[4];
            v[3].p = m_cullinfo.vecFrustum[5];
            v[0].color = v[1].color = v[2].color = v[3].color = 0x80505050;
            break;
        }
        m_pPlaneVB[iPlane]->Unlock();
    }
}

void Frustum3D::Render(IDirect3DDevice9 &device)
{
	// Render frustum planes
    device.SetMaterial(&m_mtrlWhite);
    D3DXMATRIX mat;
    D3DXMatrixIdentity(&mat );
    device.SetTransform(D3DTS_WORLD, &mat);
    device.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	device.SetFVF(PlaneVertex::FVF);
    device.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device.SetTexture(0, NULL);
    for (int iPlane = 0; iPlane < 6; ++iPlane)
    {
        device.SetStreamSource(0, m_pPlaneVB[iPlane], 0, sizeof(PlaneVertex));
        device.DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
    }
    device.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    device.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}