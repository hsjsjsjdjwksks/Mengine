#include "Win32MouseEvent.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32MouseEvent::Win32MouseEvent()
        : m_hWnd( NULL )
        , m_uTimer( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32MouseEvent::~Win32MouseEvent()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32MouseEvent::initialize( HWND _hWnd )
    {
        m_hWnd = _hWnd;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32MouseEvent::verify()
    {
        if( m_hWnd == NULL )
        {
            return;
        }

        if( m_uTimer == 0 )
        {
            return;
        }

        POINT pt;
        ::GetCursorPos( &pt );

        RECT rect;
        ::GetClientRect( m_hWnd, &rect );

        POINT pleft;
        pleft.x = rect.left;
        pleft.y = rect.top;
        ClientToScreen( m_hWnd, &pleft );

        POINT pright;
        pright.x = rect.right;
        pright.y = rect.bottom;
        ClientToScreen( m_hWnd, &pright );

        RECT rclient;
        rclient.left = pleft.x;
        rclient.top = pleft.y;
        rclient.right = pright.x;
        rclient.bottom = pright.y;

        if( ::PtInRect( &rclient, pt ) == FALSE )
        {
            ::KillTimer( m_hWnd, m_uTimer );
            m_uTimer = 0;

            ::SendMessage( m_hWnd, UWM_MOUSE_LEAVE, 0, 0 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32MouseEvent::update()
    {
        if( m_hWnd == NULL )
        {
            return;
        }

        if( m_uTimer != 0 )
        {
            return;
        }

        UINT uElapse = 20;

        m_uTimer = ::SetTimer( m_hWnd, MENGINE_UTIMER_MOUSE_EVENT, uElapse, NULL );

        if( m_uTimer == 0 )
        {
            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32MouseEvent::stop()
    {
        if( m_hWnd == NULL )
        {
            return;
        }

        if( m_uTimer != 0 )
        {
            ::KillTimer( m_hWnd, m_uTimer );
            m_uTimer = 0;
        }

        m_hWnd = NULL;
    }
}