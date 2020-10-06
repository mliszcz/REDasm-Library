#pragma once

#include <memory>
#include <rdapi/plugin/loader.h>
#include "../document/document_fwd.h"
#include "../database/database.h"
#include "../buffer/buffer.h"
#include "entry.h"

class Disassembler;

class Loader: public Entry<RDEntryLoader>
{
    public:
        Loader(const RDLoaderRequest* req, const RDEntryLoader* entry, Context* ctx);
        bool load();
        bool build();
        bool view(rd_address address, RDBufferView* view) const;
        bool view(rd_address address, size_t size, RDBufferView* view) const;
        bool view(const RDSegment& segment, RDBufferView* view) const;
        Database* database(const std::string& dbname);
        rd_flag flags() const;
        MemoryBuffer* buffer();
        SafeDocument& document();

    public:
        RDLocation offset(rd_address address) const;
        RDLocation address(rd_offset offset) const;
        RDLocation addressof(const void* ptr) const;
        RDLocation fileoffset(const void* ptr) const;
        u8* addrpointer(rd_address address) const;
        u8* pointer(rd_offset offset) const;

    public:
        static const char* test(const RDEntryLoader* entry, const RDLoaderRequest* req);

    private:
        RDLoaderBuildParams m_buildparams{ };
        std::unique_ptr<MemoryBuffer> m_buffer;
        std::unordered_map<std::string, std::unique_ptr<Database>> m_database;
        SafeDocument m_document;
        std::string m_filepath;
};
