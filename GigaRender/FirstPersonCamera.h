#pragma once

#include "BaseCamera.h"

//--------------------------------------------------------------------------------------
// Simple first person camera class that moves and rotates.
//       It allows yaw and pitch but not roll.  It uses WM_KEYDOWN and 
//       GetCursorPos() to respond to keyboard and mouse input and updates the 
//       view matrix based on input.  
//--------------------------------------------------------------------------------------
class FirstPersonCamera : public BaseCamera
{
public:
    FirstPersonCamera();

    // Call these from client and use Get*Matrix() to read new matrices
    virtual void FrameMove( FLOAT fElapsedTime );

    // Functions to change behavior
    void SetRotateButtons( bool bLeft, bool bMiddle, bool bRight, bool bRotateWithoutButtonDown = false );

    void SetResetCursorAfterMove( bool bResetCursorAfterMove ) { m_bResetCursorAfterMove = bResetCursorAfterMove; }

    // Functions to get state
    D3DXMATRIX*  GetWorldMatrix()            { return &m_mCameraWorld; }

    const D3DXVECTOR3* GetWorldRight() const { return (D3DXVECTOR3*)&m_mCameraWorld._11; } 
    const D3DXVECTOR3* GetWorldUp() const    { return (D3DXVECTOR3*)&m_mCameraWorld._21; }
    const D3DXVECTOR3* GetWorldAhead() const { return (D3DXVECTOR3*)&m_mCameraWorld._31; }
    const D3DXVECTOR3* GetEyePt() const      { return (D3DXVECTOR3*)&m_mCameraWorld._41; }

protected:
    D3DXMATRIX m_mCameraWorld;       // World matrix of the camera (inverse of the view matrix)

    int        m_nActiveButtonMask;  // Mask to determine which button to enable for rotation
    bool       m_bRotateWithoutButtonDown;

    bool       m_bResetCursorAfterMove;// If true, the class will reset the cursor position so that the cursor always has space to move 
};