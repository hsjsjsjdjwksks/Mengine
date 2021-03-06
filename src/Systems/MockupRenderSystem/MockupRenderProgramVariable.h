#pragma once

#include "Interface/RenderProgramVariableInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MockupRenderProgramVariable
        : public RenderProgramVariableInterface
        , public Factorable
    {
    public:
        MockupRenderProgramVariable();
        ~MockupRenderProgramVariable() override;

    public:
        bool initialize( uint32_t _vertexCount, uint32_t _pixelCount );
        void finalize();

    public:
        void setVertexVariableFloats( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;
        void setVertexVariableIntegers( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count ) override;
        void setVertexVariableBooleans( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count ) override;

    public:
        void setPixelVariableFloats( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;
        void setPixelVariableIntegers( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count ) override;
        void setPixelVariableBooleans( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count ) override;

    public:
        void updatePixelVariableFloats( uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;
        void updatePixelVariableIntegers( uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count ) override;
        void updatePixelVariableBooleans( uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count ) override;

    public:
        enum EProgramVariableType
        {
            EPVT_FLOAT,
            EPVT_INTEGER,
            EPVT_BOOLEAN
        };

        struct ProgramVariableDesc
        {
            EProgramVariableType type;
            uint32_t offset;
            uint32_t size;
            uint32_t count;
        };

    protected:
        typedef Vector<float> VectorDataFloats;
        typedef Vector<int32_t> VectorDataIntegers;
        typedef Vector<int32_t> VectorDataBooleans;

        VectorDataFloats m_dataFloats;
        VectorDataIntegers m_dataIntegers;
        VectorDataBooleans m_dataBooleans;

        VectorDataFloats m_pixelFloats;
        VectorDataIntegers m_pixelIntegers;
        VectorDataBooleans m_pixelBooleans;

        typedef Vector<ProgramVariableDesc> VectorVariables;
        VectorVariables m_vertexVariables;
        VectorVariables m_pixelVariables;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MockupRenderProgramVariable> MockupRenderProgramVariablePtr;
    //////////////////////////////////////////////////////////////////////////
}