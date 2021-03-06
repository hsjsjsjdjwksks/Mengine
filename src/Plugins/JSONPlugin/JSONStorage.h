#pragma once

#include "JSONInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class JSONStorage
        : public JSONStorageInterface
        , public Factorable
    {
    public:
        JSONStorage();
        ~JSONStorage() override;

    public:
        void setJSON( const jpp::object & _json );

    public:
        MENGINE_INLINE const jpp::object & getJSON() const override;

    public:
        jpp::object modifyJSON() override;
        jpp::object copyJSON() const override;
        bool mergeJSON( const jpp::object & _json, bool _copy, bool _recurive, bool _mergeArray ) override;

    protected:
        jpp::object m_json;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<JSONStorage> JSONStoragePtr;
    //////////////////////////////////////////////////////////////////////////
    const jpp::object & JSONStorage::getJSON() const
    {
        return m_json;
    }
    //////////////////////////////////////////////////////////////////////////
}