#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/NotificationServiceInterface.h"

#	include "Kernel/Node.h"

#	include "Core/Viewport.h"

namespace Menge
{
	class Camera3D
        : public Node
		, public RenderCameraInterface		
	{
	public:
		Camera3D();

	protected:
		bool _activate() override;
		void _deactivate() override;
		
	public:
		void setCameraPosition( const mt::vec3f & _pos );
		void setCameraDir( const mt::vec3f & _dir );
		void setCameraUp( const mt::vec3f & _up );
		void setCameraRightSign( float _rightSign );

	public:
		void setCameraFOV( float _fov );
		void setCameraAspect( float _aspect );
		void setCameraNear( float _near );
		void setCameraFar( float _far );
		

	public:
		void setRenderport( const Viewport & _renderport );
		
	public:
		const Viewport & getCameraRenderport() const override;

	public:				
		const mt::mat4f & getCameraViewMatrix() const override;
		const mt::mat4f & getCameraViewMatrixInv() const override;
		const mt::mat4f & getCameraProjectionMatrix() const override;
		const mt::mat4f & getCameraProjectionMatrixInv() const override;

	public:
		const mt::box2f & getCameraBBoxWM() const override;

	public:
		bool isOrthogonalProjection() const override;

	protected:
		void _invalidateWorldMatrix() override;
		
	protected:
		void invalidateViewMatrix_();
		void invalidateProjectionMatrix_();
		
	protected:
		void updateMatrix_() const;
		void updateProjectionMatrix_() const;
		void updateBBoxWM_() const;

	protected:
		void notifyChangeWindowResolution( bool _fullscreen, const Resolution & _resolution );
		
	protected:
        mt::vec3f m_cameraPosition;
		mt::vec3f m_cameraDir;
		mt::vec3f m_cameraUp;
		float m_cameraRightSign;

		float m_cameraFov;
		float m_cameraAspect;
		float m_cameraNear;
		float m_cameraFar;		

		Viewport m_renderport;

		ObserverInterface * m_notifyChangeWindowResolution;

		mutable mt::mat4f m_viewMatrix;
		mutable mt::mat4f m_viewMatrixInv;

		mutable mt::mat4f m_projectionMatrix;
		mutable mt::mat4f m_projectionMatrixInv;

		mutable mt::box2f m_bboxWM;
		
		mutable bool m_invalidateMatrix;
		mutable bool m_invalidateProjectionMatrix;
		mutable bool m_invalidateBB;
	};
	//////////////////////////////////////////////////////////////////////////
	inline void Camera3D::invalidateViewMatrix_()
	{
		m_invalidateMatrix = true;
		m_invalidateBB = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void Camera3D::invalidateProjectionMatrix_()
	{
		m_invalidateProjectionMatrix = true;
		m_invalidateBB = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const Viewport & Camera3D::getCameraRenderport() const
	{
		return m_renderport;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & Camera3D::getCameraProjectionMatrix() const
	{
		if( m_invalidateProjectionMatrix == true )
		{
			this->updateProjectionMatrix_();
		}

		return m_projectionMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & Camera3D::getCameraProjectionMatrixInv() const
	{
		if( m_invalidateProjectionMatrix == true )
		{
			this->updateProjectionMatrix_();
		}

		return m_projectionMatrixInv;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & Camera3D::getCameraViewMatrix() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_viewMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & Camera3D::getCameraViewMatrixInv() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_viewMatrixInv;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::box2f & Camera3D::getCameraBBoxWM() const
	{
		if( m_invalidateBB == true )
		{
			this->updateBBoxWM_();
		}

		return m_bboxWM;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Camera3D::isOrthogonalProjection() const
	{
		return false;
	}
}