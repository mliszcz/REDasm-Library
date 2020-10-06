#include "binary.h"
#include "../../object.h"
#include "../../plugin/loader.h"
#include "../../document/document.h"
#include "../builtin.h"

static const char* test(const RDLoaderRequest*) { return ""; /* Always valid */ }

static bool build(RDContext*, RDLoader* ldr, const RDLoaderBuildParams* req)
{
    auto* loader = CPTR(Loader, ldr);
    auto& doc = loader->document();
    size_t sz = loader->buffer()->size();

    doc->segment("BINARY", req->offset, req->baseaddress, sz, sz, SegmentFlags_CodeData);
    doc->entry(req->entrypoint);
    return true;
}

RDEntryLoader loaderEntry_Binary = RD_BUILTIN_ENTRY(binary_builtin, "Binary",
                                                    LoaderFlags_CustomAssembler | LoaderFlags_CustomAddressing,
                                                    &test, nullptr, &build);
