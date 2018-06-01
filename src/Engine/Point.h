#pragma once

#include "Kernel/Node.h"

#include "Core/RenderVertex2D.h"

namespace Mengine
{
	class Point
		: public Node
	{
	public:
		Point();

	public:
		void setLinkedPoint( Point * _linked );
		void removeLinkedPoint();
		Point * getLinkedPoint() const;

	public:
		void setWidth( float _width );
		float getWidth() const;

	protected:
		void _destroy() override;

	protected:
		void _render( RenderServiceInterface * _renderService, const RenderContext * _state ) override;

	protected:
		void _debugRender( RenderServiceInterface * _renderService, const RenderContext * _state ) override;

	protected:
		Point * m_linked;
		bool m_owner;

		float m_width;

		RenderVertex2D m_vertices[4];
	};
}
