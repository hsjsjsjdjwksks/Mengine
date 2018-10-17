#include "OpenGLRenderProgramVariable.h"

#include "OpenGLRenderError.h"
#include "OpenGLRenderEnum.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderProgramVariable::OpenGLRenderProgramVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderProgramVariable::~OpenGLRenderProgramVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderProgramVariable::initialize( uint32_t _vertexCount, uint32_t _pixelCount )
    {
        m_vertexVariables.resize( _vertexCount );
        m_pixelVariables.resize( _pixelCount );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::finalize()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        template<class T>
        void makeVariableData( OpenGLRenderProgramVariable::Variable & _variable, Vector<T> & _container, uint32_t _type, T * _values, uint32_t _count )
        {
            _variable.type = _type;
            _variable.offset = _container.size();
            _variable.count = _count;

            _container.insert( _container.end(), _values, _values + _count );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setVertexVariableFloats( uint32_t _index, float * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataFloats, 0, _values, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setVertexVariableIntegers( uint32_t _index, int32_t * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataIntegers, 1, _values, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setVertexVariableBooleans( uint32_t _index, int32_t * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataBooleans, 2, _values, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setPixelVariableFloats( uint32_t _index, float * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataFloats, 0, _values, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setPixelVariableIntegers( uint32_t _index, int32_t * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataIntegers, 1, _values, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setPixelVariableBooleans( uint32_t _index, int32_t * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataBooleans, 2, _values, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderProgramVariable::apply( IDirect3DDevice9 * _pD3DDevice, const RenderProgramInterfacePtr & _program )
    {
        (void)_program;

        uint32_t vertexEnumerator = 0;
        for( const Variable & v : m_vertexVariables )
        {
            switch( v.type )
            {
            case 0:
                {
                    const float * buff = &m_dataFloats[v.offset];
                    DXCALL( _pD3DDevice, SetVertexShaderConstantF, (vertexEnumerator, buff, v.count) );
                }break;
            case 1:
                {
                    const int32_t * buff = &m_dataIntegers[v.offset];
                    DXCALL( _pD3DDevice, SetVertexShaderConstantI, (vertexEnumerator, buff, v.count) );
                }break;
            case 2:
                {
                    const int32_t * buff = &m_dataBooleans[v.offset];
                    DXCALL( _pD3DDevice, SetVertexShaderConstantB, (vertexEnumerator, buff, v.count) );
                }break;
            default:
                {
                    return false;
                }
            }

            ++vertexEnumerator;
        }

        uint32_t pixelEnumerator = 0;
        for( const Variable & v : m_pixelVariables )
        {
            switch( v.type )
            {
            case 0:
                {
                    const float * buff = &m_dataFloats[v.offset];
                    DXCALL( _pD3DDevice, SetPixelShaderConstantF, (pixelEnumerator, buff, v.count) );
                }break;
            case 1:
                {
                    const int32_t * buff = &m_dataIntegers[v.offset];
                    DXCALL( _pD3DDevice, SetPixelShaderConstantI, (pixelEnumerator, buff, v.count) );
                }break;
            case 2:
                {
                    const int32_t * buff = &m_dataBooleans[v.offset];
                    DXCALL( _pD3DDevice, SetPixelShaderConstantB, (pixelEnumerator, buff, v.count) );
                }break;
            default:
                {
                    return false;
                }
            }

            ++pixelEnumerator;
        }

        return true;
    }
}