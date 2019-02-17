#include "rtti_msvc.h"
#include "../../../plugins/format.h"
#include "../../../disassembler/disassemblerapi.h"

#define RTTI_MSVC_CLASS_DESCRIPTOR_PREFIX ".?AV"
#define RTTI_MSVC_FIXUP (sizeof(u32) * 2)
#define RTTI_MSVC_TYPE_DESCRIPTOR(format, typedescriptorname) FormatPlugin::relpointer<RTTITypeDescriptor>(typedescriptorname, -RTTI_MSVC_FIXUP)

namespace REDasm {
namespace RTTI {

void RTTIMsvc::search(DisassemblerAPI *disassembler)
{
    RTTIVTableMap rttivtables;
    RTTICompleteObjectMap rttiobjects;
    RTTITypeDescriptorMap rttitypes;
    DataSegmentList segments;

    RTTIMsvc::searchDataSegments(disassembler, segments);
    RTTIMsvc::searchTypeDescriptors(disassembler, rttitypes, segments);
    RTTIMsvc::searchCompleteObjects(disassembler, rttiobjects, rttitypes, segments);
    RTTIMsvc::searchVTables(disassembler, rttivtables, rttiobjects, segments);

    auto lock = x_lock_safe_ptr(disassembler->document());
    FormatPlugin* format = disassembler->format();

    for(auto& rttivtableitem : rttivtables)
    {
        const RTTICompleteObjectLocator* rttiobject = rttivtableitem.first;
        auto it = rttiobjects.find(rttiobject);

        if(it == rttiobjects.end())
            continue;

        std::string objectname = RTTIMsvc::objectName(disassembler, rttiobject);
        std::string vtablename = RTTIMsvc::vtableName(disassembler, rttiobject);
        const u32* pobjectdata = rttivtableitem.second;
        address_t address = format->addressof(pobjectdata), rttiobjectaddress = format->addressof(rttiobject);

        REDasm::status("Reading " + objectname + "'s VTable");

        lock->info(address, vtablename);
        lock->lock(address, objectname + "::ptr_rtti_object", SymbolTypes::Data | SymbolTypes::Pointer);
        lock->lock(rttiobjectaddress, objectname + "::rtti_object");
        disassembler->pushReference(rttiobjectaddress, address);
        pobjectdata++; // Skip RTTICompleteObjectLocator

        const Segment* segment = lock->segment(static_cast<u32>(*pobjectdata));

        for(u64 i = 0; segment && segment->is(SegmentTypes::Code); i++) // Walk vtable
        {
            address = format->addressof(pobjectdata);
            disassembler->disassemble(*pobjectdata);

            lock->lock(address, objectname + "::vftable_" + std::to_string(i), SymbolTypes::Data | SymbolTypes::Pointer);
            lock->function(*pobjectdata, objectname + "::sub_" + REDasm::hex(static_cast<u32>(*pobjectdata)));

            disassembler->pushReference(*pobjectdata, address);

            pobjectdata++;
            segment = lock->segment(*pobjectdata);
        }
    }
}

std::string RTTIMsvc::objectName(DisassemblerAPI* disassembler, const RTTICompleteObjectLocator *rttiobject)
{
    const RTTITypeDescriptor* rttitype = disassembler->format()->addrpointer<RTTITypeDescriptor>(rttiobject->pTypeDescriptor);
    std::string rttitypename = reinterpret_cast<const char*>(&rttitype->name);
    return Demangler::demangled("?" + rttitypename.substr(4) + "6A@Z");
}

std::string RTTIMsvc::vtableName(DisassemblerAPI *disassembler, const RTTICompleteObjectLocator *rttiobject)
{
    const RTTITypeDescriptor* rttitype = disassembler->format()->addrpointer<RTTITypeDescriptor>(rttiobject->pTypeDescriptor);
    std::string rttitypename = reinterpret_cast<const char*>(&rttitype->name);
    return Demangler::demangled("??_7" + rttitypename.substr(4) + "6B@Z");
}

void RTTIMsvc::searchDataSegments(DisassemblerAPI *disassembler, DataSegmentList &segments)
{
    const ListingDocument& document = disassembler->document();

    for(size_t i = 0; i < document->segmentsCount(); i++)
    {
        const Segment* segment = document->segmentAt(i);

        if(segment->empty() || segment->is(SegmentTypes::Bss) || segment->is(SegmentTypes::Code) || (segment->name.find("data") == std::string::npos))
            continue;

        REDasm::status("Checking segment '" + segment->name + "'");
        segments.push_front(segment);
    }
}

void RTTIMsvc::searchTypeDescriptors(DisassemblerAPI *disassembler, RTTITypeDescriptorMap &rttitypes, const DataSegmentList &segments)
{
    const ListingDocument& document = disassembler->document();
    const FormatPlugin* format = disassembler->format();

    for(const Segment* segment : segments)
    {
        BufferView view = format->viewSegment(segment);

        if(view.eob())
            continue;

        auto res = view.find<char>(RTTI_MSVC_CLASS_DESCRIPTOR_PREFIX);

        while(res.hasNext())
        {
            REDasm::statusAddress("Searching RTTITypeDescriptors in " + REDasm::quoted(segment->name), format->address(res.position));
            const RTTITypeDescriptor* rttitype = RTTI_MSVC_TYPE_DESCRIPTOR(format, res.result);

            if(document->segment(rttitype->pVFTable))
                rttitypes.emplace(segment->address + res.position - RTTI_MSVC_FIXUP, rttitype);

            res = res.next();
        }
    }
}

void RTTIMsvc::searchCompleteObjects(DisassemblerAPI *disassembler, RTTICompleteObjectMap& rttiobjects, const RTTITypeDescriptorMap &rttitypes, const DataSegmentList &segments)
{
    const FormatPlugin* format = disassembler->format();
    RTTICompleteObjectLocatorSearch searchobj = { 0, 0, 0, 0 };

    for(const auto& item : rttitypes)
    {
        searchobj.pTypeDescriptor = item.first;

        for(const Segment* segment : segments)
        {
            BufferView view = format->viewSegment(segment);
            auto res = view.find<RTTICompleteObjectLocatorSearch>(&searchobj);

            if(!res.hasNext())
                continue;

            REDasm::statusProgress("Searching RTTICompleteObjectLocators in " + REDasm::quoted(segment->name), format->address(res.position));
            rttiobjects.emplace(reinterpret_cast<const RTTICompleteObjectLocator*>(res.result), segment->address + res.position);
            break;
        }
    }
}

void RTTIMsvc::searchVTables(DisassemblerAPI *disassembler, RTTIMsvc::RTTIVTableMap &vtables, const RTTIMsvc::RTTICompleteObjectMap &rttiobjects, const RTTIMsvc::DataSegmentList &segments)
{
    const FormatPlugin* format = disassembler->format();

    for(const auto& item : rttiobjects)
    {
        const RTTICompleteObjectLocator* rttiobject = item.first;
        REDasm::status("Searching VTables for " + REDasm::quoted(RTTIMsvc::objectName(disassembler, rttiobject)));

        for(const Segment* segment : segments)
        {
            BufferView view = format->viewSegment(segment);
            auto res = view.find(reinterpret_cast<const u32*>(&item.second));
            bool found = false;

            while(res.hasNext())
            {
                const RTTICompleteObjectLocator* foundrttiobject = format->addrpointer<RTTICompleteObjectLocator>(*res.result);

                if(rttiobject == foundrttiobject)
                {
                    found = true;
                    break;
                }

                res = res.next();
            }

            if(!found)
                continue;

            vtables.emplace(item.first, format->pointer<u32>(segment->offset + res.position));
            break;
        }
    }
}

} // namespace RTTI
} // namespace REDasm
