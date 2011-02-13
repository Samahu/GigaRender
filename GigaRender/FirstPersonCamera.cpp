#include "dxstdafx.h"
#include "FirstPersonCamera.h"

//--------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------
FirstPersonCamera::FirstPersonCamera() :
    m_nActiveButtonMask( 0x07 )
{
    m_bRotateWithoutButtonDown = false;
}

//--------------------------------------------------------------------------------------
// Update the view matrix based on user input & elapsed time
//--------------------------------------------------------------------------------------
VOID FirstPersonCamera::FrameMove( FLOAT fElapsedTime )
{
	D3DXVECTOR3 vOldEye = m_vEye;
	float oldPitch = m_fCameraPitchAngle;
	float oldYaw = m_fCameraYawAngle;

    if( DXUTGetGlobalTimer()->IsStopped() )
        fElapsedTime = 1.0f / DXUTGetFPS();

    if( IsKeyDown(m_aKeys[CAM_RESET]) )
        Reset();

    // Get keyboard/mouse/gamepad input
    GetInput( m_bEnablePositionMovement, (m_nActiveButtonMask & m_nCurrentButtonMask) || m_bRotateWithoutButtonDown, true, m_bResetCursorAfterMove );

    // Get amount of velocity based on the keyboard input and drag (if any)
    UpdateVelocity( fElapsedTime );

    // Simple euler method to calculate position delta
	D3DXVECTOR3 vPosDelta =  IsKeyDown(m_aKeys[CAMERA_KEY_SHIFT_SPEED]) ? 50.0f * m_vVelocity * fElapsedTime : m_vVelocity * fElapsedTime;

    // If rotating the camera 
    if( (m_nActiveButtonMask & m_nCurrentButtonMask) || 
        m_bRotateWithoutButtonDown || 
        m_vGamePadRightThumb.x != 0 || 
        m_vGamePadRightThumb.z != 0 )
    {
        // Update the pitch & yaw angle based on mouse movement
        float fYawDelta   = m_vRotVelocity.x;
        float fPitchDelta = m_vRotVelocity.y;

        // Invert pitch if requested
        if( m_bInvertPitch )
            fPitchDelta = -fPitchDelta;

        m_fCameraPitchAngle += fPitchDelta;
        m_fCameraYawAngle   += fYawDelta;

        // Limit pitch to straight up or straight down
        m_fCameraPitchAngle = __max( -D3DX_PI/2.0f,  m_fCameraPitchAngle );
        m_fCameraPitchAngle = __min( +D3DX_PI/2.0f,  m_fCameraPitchAngle );
    }

    // Make a rotation matrix based on the camera's yaw & pitch
    D3DXMATRIX mCameraRot;
    D3DXMatrixRotationYawPitchRoll( &mCameraRot, m_fCameraYawAngle, m_fCameraPitchAngle, 0 );

    // Transform vectors based on camera's rotation matrix
    D3DXVECTOR3 vWorldUp, vWorldAhead;
    D3DXVECTOR3 vLocalUp    = D3DXVECTOR3(0,1,0);
    D3DXVECTOR3 vLocalAhead = D3DXVECTOR3(0,0,1);
    D3DXVec3TransformCoord( &vWorldUp, &vLocalUp, &mCameraRot );
    D3DXVec3TransformCoord( &vWorldAhead, &vLocalAhead, &mCameraRot );

    // Transform the position delta by the camera's rotation 
    D3DXVECTOR3 vPosDeltaWorld;
    if( !m_bEnableYAxisMovement )
    {
        // If restricting Y movement, do not include pitch
        // when transforming position delta vector.
        D3DXMatrixRotationYawPitchRoll( &mCameraRot, m_fCameraYawAngle, 0.0f, 0.0f );
    }
    D3DXVec3TransformCoord( &vPosDeltaWorld, &vPosDelta, &mCameraRot );

    // Move the eye position 
    m_vEye += vPosDeltaWorld;
    if( m_bClipToBoundary )
        ConstrainToBoundary( &m_vEye );

    // Update the lookAt position based on the eye position 
    m_vLookAt = m_vEye + vWorldAhead;

    // Update the view matrix
    D3DXMatrixLookAtLH( &m_mView, &m_vEye, &m_vLookAt, &vWorldUp );

    D3DXMatrixInverse( &m_mCameraWorld, NULL, &m_mView );

	CAMERA_EVENT_LIST eventList;
	if (vOldEye != m_vEye)
		eventList.push_back(CAMERA_POSITION_CHANGED);
	if (oldPitch != m_fCameraPitchAngle || oldYaw != m_fCameraYawAngle)
		eventList.push_back(CAMERA_ORIENTATION_CHANGED);
	if (eventList.size() > 0)
		notifyEvents(eventList);
}

//--------------------------------------------------------------------------------------
// Enable or disable each of the mouse buttons for rotation drag.
//--------------------------------------------------------------------------------------
void FirstPersonCamera::SetRotateButtons( bool bLeft, bool bMiddle, bool bRight, bool bRotateWithoutButtonDown )
{
    m_nActiveButtonMask = ( bLeft ? MOUSE_LEFT_BUTTON : 0 ) |
                          ( bMiddle ? MOUSE_MIDDLE_BUTTON : 0 ) |
                          ( bRight ? MOUSE_RIGHT_BUTTON : 0 );
    m_bRotateWithoutButtonDown = bRotateWithoutButtonDown;
}