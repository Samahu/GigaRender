//--------------------------------------------------------------------------------------
// File: TerrainPaging.cpp
//
// Empty starting point for new Direct3D applications
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "resource.h"
#include "TerrainPager.h"
#include "PatchResolution.h"
#include "Frustum3D.h"
#include "FirstPersonCamera.h"
#include "DoubleFrustumRectangle.h"

class FirstPersonCameraM : public FirstPersonCamera
{
	virtual CameraKey MapKey( UINT nKey )
	{
		// This could be upgraded to a method that's user-definable but for 
		// simplicity, we'll use a hardcoded mapping.
		switch( nKey )
		{
		case VK_NUMPAD0:  return CAMERA_KEY_CONTROL_DOWN;
		case VK_NUMPAD4:  return CAMERA_KEY_STRAFE_LEFT;
		case VK_NUMPAD6: return CAMERA_KEY_STRAFE_RIGHT;
		case VK_NUMPAD8:    return CAMERA_KEY_MOVE_FORWARD;
		case VK_NUMPAD2:  return CAMERA_KEY_MOVE_BACKWARD;
		case VK_NUMPAD9: return CAMERA_KEY_MOVE_UP;        // pgup
		case VK_NUMPAD3:  return CAMERA_KEY_MOVE_DOWN;      // pgdn
		case VK_NUMPAD7:   return CAMERA_KEY_RESET;
		case VK_ADD:	return CAMERA_KEY_SHIFT_SPEED;
		}

		return CAMERA_KEY_UNKNOWN;
	}
};

ID3DXFont* g_pFont = NULL;			// Font for drawing text
ID3DXSprite* g_pTextSprite = NULL;	// Sprite for batching draw text calls
TerrainPager *g_pTerrain;
FirstPersonCameraM g_Camera1;
FirstPersonCamera g_Camera2;
DoubleFrustumRectangle *g_pDFRWindow;
Frustum3D g_Frustum;
bool g_SwitchCameras = false;


//--------------------------------------------------------------------------------------
// Rejects any devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // Typically want to skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3DObject(); 
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
                    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

	if (!(pCaps->Caps2 & D3DCAPS2_DYNAMICTEXTURES))
		return false;


    return true;
}


//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_MANAGED resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr;

	// Initialize the font
    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                         L"Arial", &g_pFont ) );

	g_pTerrain->SetDevice(pd3dDevice);
	if (false == g_pTerrain->Init())
	{
		g_pTerrain->Destroy();
		return E_FAIL;
	}

	// setup camera
	float cam_x = 800.0f;
	float cam_z = 800.0f;
	D3DXVECTOR3 eye, lookat;

	// setup first camera.
	eye.x = cam_x, eye.y = 50.0f, eye.z = cam_z;
	lookat.x = cam_x, lookat.y = 50.0f, lookat.z = 2 * cam_z;
	g_Camera1.SetViewParams(&eye, &lookat);
	g_Camera1.SetRotateButtons(false, false, true);

	// setup second camera.
	eye.x = 0.0f, eye.y = 300.0f, eye.z = 0.0f;
	lookat.x = cam_x, lookat.y = 0.0f, lookat.z = cam_z;
	g_Camera2.SetViewParams(&eye, &lookat);
	g_Camera2.SetRotateButtons(true, false, false);

	//pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_DEFAULT resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr;

    if( g_pFont )
        V_RETURN( g_pFont->OnResetDevice() );

	// Create a sprite to help batch calls when drawing many lines of text
    V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pTextSprite ) );

	float aspectRatio = (float)pBackBufferSurfaceDesc->Width / (float)pBackBufferSurfaceDesc->Height;
	g_Camera1.SetProjParams((float)3.14 / 4, aspectRatio, 1.0f, 800.0f);
	g_Camera2.SetProjParams((float)3.14 / 4, aspectRatio, 1.0f, 8000.0f);

	g_pDFRWindow->SetCamera(&g_Camera1);

	g_Frustum.Initialize(*pd3dDevice);

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	g_Camera1.FrameMove(fElapsedTime);
	g_Camera2.FrameMove(fElapsedTime);
	g_Frustum.Update(g_Camera1);
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
	static WCHAR buffer[255];
    CDXUTTextHelper txtHelper( g_pFont, g_pTextSprite, 15 );

    // Output statistics
    txtHelper.Begin();
    txtHelper.SetInsertionPos(5, 5);
    txtHelper.SetForegroundColor(D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ));
    txtHelper.DrawTextLine(DXUTGetFrameStats(true));
    txtHelper.DrawTextLine(DXUTGetDeviceStats());
//	swprintf_s(buffer, 256, L"window I= %d, J= %d", g_CurrPatch.GetI(), g_CurrPatch.GetJ());
//	txtHelper.DrawTextLine(buffer);
    txtHelper.End();
}


//--------------------------------------------------------------------------------------
// Render the scene 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;

	BaseCamera *pCamera = (g_SwitchCameras ? &g_Camera1 : &g_Camera2);

    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
		D3DXMATRIX mat;
		D3DXMatrixIdentity(&mat);
		pd3dDevice->SetTransform(D3DTS_WORLD, &mat);
		pd3dDevice->SetTransform(D3DTS_VIEW, pCamera->GetViewMatrix());
        pd3dDevice->SetTransform(D3DTS_PROJECTION, pCamera->GetProjMatrix());

		g_pTerrain->Render();

		if (!g_SwitchCameras)	// only render the furstum if we are viewing it from the outside.
			g_Frustum.Render(*pd3dDevice);

		RenderText();

        V( pd3dDevice->EndScene() );
    }

	// Copy in-memory ready items to the video memory
	g_pTerrain->Process(fElapsedTime);
}


//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
	switch( uMsg )
    {
        case WM_KEYUP:
        {
			if (VK_TAB == (UINT)wParam)
				g_SwitchCameras = !g_SwitchCameras;

            break;
        }
	}

	g_Camera1.HandleMessages(hWnd, uMsg, wParam, lParam);
	g_Camera2.HandleMessages(hWnd, uMsg, wParam, lParam);
    return 0;
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnResetDevice callback here 
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{
	g_Frustum.Destroy();

	if( g_pFont )
        g_pFont->OnLostDevice();

	SAFE_RELEASE( g_pTextSprite );
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnCreateDevice callback here
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
	SAFE_RELEASE( g_pFont );

	if (NULL != g_pTerrain)
		g_pTerrain->Destroy();
}



//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	CreateDXUTState();
//	onexit(DeleteDXUTState);

    // Set the callback functions
    DXUTSetCallbackDeviceCreated( OnCreateDevice );
    DXUTSetCallbackDeviceReset( OnResetDevice );
    DXUTSetCallbackDeviceLost( OnLostDevice );
    DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackFrameRender( OnFrameRender );
    DXUTSetCallbackFrameMove( OnFrameMove );
   
    // TODO: Perform any application-level initialization here
	WCHAR buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	SetCurrentDirectory(L"Resources\\map0\\");
	GetCurrentDirectory(MAX_PATH, buffer);

	g_pTerrain = TerrainPager::Create("major", 272);	// 272 = 16 * 16 + 16
	g_pDFRWindow = new DoubleFrustumRectangle(*g_pTerrain);

    // Initialize DXUT and create the desired Win32 window and Direct3D device for the application
    DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"TerrainPaging" );
    DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 640, 480, IsDeviceAcceptable, ModifyDeviceSettings );

    // Start the render loop
    DXUTMainLoop();

	int exit_code = DXUTGetExitCode();
	DeleteDXUTState();

    // TODO: Perform any application-level cleanup here
	if(NULL != g_pDFRWindow)
	{
		delete g_pDFRWindow;
		g_pDFRWindow = NULL;
	}
	
	if(NULL != g_pTerrain)
	{
		delete g_pTerrain;
		g_pTerrain = NULL;
	}

    return exit_code;
}