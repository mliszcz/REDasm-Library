#include "graphtests.h"
#include "doctest.h"

void GraphTests::testCavia(const RDDocument* doc)
{
    static TestGraphs g = {
        { 0x401000, 0xC9051DFC }, { 0x401029, 0x6711D964 },
        { 0x401052, 0xC9051DFC }, { 0x401058, 0xC9051DFC },
        { 0x40105E, 0xC9051DFC }, { 0x401064, 0xC9051DFC },
    };

    GraphTests::testGraphs(g, doc);
}

void GraphTests::testCM01(const RDDocument* doc)
{
    static TestGraphs g = {
        { 0x00401000, 0x29521D06 }, { 0x00401128, 0x4070F071 }, { 0x00401253, 0x60F31F9D },
        { 0x0040130A, 0x0F710C9D }, { 0x0040134D, 0xC9051DFC }, { 0x00401362, 0xC9051DFC },
        { 0x0040137E, 0x98916B68 }, { 0x004013C2, 0x49904259 }, { 0x004013D2, 0xC9051DFC },
        { 0x004013D8, 0x09E26C53 }, { 0x0040140A, 0xC9051DFC }, { 0x00401416, 0xC9051DFC },
        { 0x00401428, 0xC9051DFC }, { 0x00401434, 0xC9051DFC }, { 0x0040143A, 0xC9051DFC },
        { 0x00401440, 0xC9051DFC }, { 0x0040144C, 0xC9051DFC }, { 0x0040145E, 0xC9051DFC },
        { 0x0040146A, 0xC9051DFC }, { 0x00401482, 0xC9051DFC }, { 0x00401488, 0xC9051DFC },
        { 0x00401494, 0xC9051DFC }, { 0x0040149A, 0xC9051DFC }, { 0x004014A0, 0xC9051DFC },
        { 0x004014A6, 0xC9051DFC }, { 0x004014B2, 0xC9051DFC }, { 0x004014BE, 0xC9051DFC },
        { 0x004014D0, 0xC9051DFC }, { 0x004014D6, 0xC9051DFC }, { 0x00401506, 0xC9051DFC },
        { 0x00401512, 0xC9051DFC },
    };

    GraphTests::testGraphs(g, doc);
}

void GraphTests::testJmpTable(const RDDocument* doc)
{
    static TestGraphs g = {
        { 0x1000, 0x396D947F }, { 0x1030, 0xC9051DFC }, { 0x1040, 0xC9051DFC }, { 0x1050, 0xC9051DFC },
        { 0x1060, 0xC9051DFC }, { 0x1070, 0xC9051DFC }, { 0x1080, 0xC9051DFC }, { 0x1090, 0xC9051DFC },
        { 0x10A0, 0xC9051DFC }, { 0x10B0, 0xC9051DFC }, { 0x10C0, 0xC9051DFC }, { 0x10D0, 0xC9051DFC },
        { 0x10E0, 0xC9051DFC }, { 0x10F0, 0xC9051DFC }, { 0x1100, 0xC9051DFC }, { 0x1110, 0xC9051DFC },
        { 0x1120, 0xC9051DFC }, { 0x1150, 0x3806793C }, { 0x11C0, 0x355E503A }, { 0x1210, 0x6F6F4B95 },
        { 0x1362, 0xAC76E99C }, { 0x152F, 0xC3B7AE84 }, { 0x1578, 0xC9051DFC }, { 0x158D, 0xC9051DFC },
        { 0x15A0, 0xC9051DFC }, { 0x15BC, 0xC9051DFC }, { 0x15D8, 0xC9051DFC }, { 0x15F4, 0xC9051DFC },
        { 0x161C, 0xC9051DFC }, { 0x1638, 0xC9051DFC }, { 0x1686, 0xC9051DFC }, { 0x16CA, 0xC9051DFC },
        { 0x16FC, 0xC9051DFC }, { 0x1720, 0xC9051DFC }, { 0x173C, 0xC9051DFC }, { 0x176A, 0xC9051DFC },
        { 0x1776, 0x47E4E9A8 }, { 0x17AC, 0xC9051DFC }, { 0x17BA, 0xC9051DFC }, { 0x17E4, 0xC9051DFC },
        { 0x17F2, 0x85BCB8CA }, { 0x188C, 0xC9051DFC }, { 0x189E, 0xC9051DFC }, { 0x18A9, 0xC9051DFC },
        { 0x18D7, 0xC9051DFC }, { 0x18FD, 0xC9051DFC }, { 0x190B, 0xC9051DFC }, { 0x1944, 0x72886678 },
        { 0x1990, 0xB2456C80 }, { 0x19C2, 0xC9051DFC }, { 0x19CE, 0x14E94F55 }, { 0x1CB8, 0xC9051DFC },
        { 0x1CEA, 0xC9051DFC }, { 0x1CFA, 0xC9051DFC }, { 0x1D3A, 0xC9051DFC }, { 0x1D58, 0xCA598BC3 },
        { 0x1E44, 0x53E8943C }, { 0x1E8C, 0xC9051DFC }, { 0x1ECC, 0xC4BF16B7 }, { 0x1EFF, 0xC9051DFC },
        { 0x1F32, 0xC9051DFC }, { 0x1F40, 0xC9051DFC }, { 0x1F93, 0xC9051DFC }, { 0x1FBA, 0xC9051DFC },
        { 0x1FDC, 0xC9051DFC }, { 0x1FFF, 0xE50502DB }, { 0x202A, 0xC9051DFC }, { 0x2054, 0xC9051DFC },
        { 0x2086, 0x428DB6E7 }, { 0x20CE, 0xC9051DFC }, { 0x2100, 0xC9051DFC }, { 0x210F, 0x837C1373 },
        { 0x2174, 0xC9051DFC }, { 0x2182, 0x5F5FC3A1 }, { 0x21C4, 0xC9051DFC }, { 0x2228, 0xC9051DFC },
        { 0x2251, 0xC9051DFC }, { 0x2282, 0xC9051DFC }, { 0x229C, 0xD88A7BAA }, { 0x22C8, 0xC9051DFC },
        { 0x22DC, 0xC9051DFC }, { 0x22EA, 0xD2E32AEE }, { 0x234D, 0xC9051DFC }, { 0x235B, 0xC9051DFC },
        { 0x2388, 0xC9051DFC }, { 0x23A2, 0xC9051DFC }, { 0x23ED, 0xC9051DFC }, { 0x240F, 0xC9051DFC },
        { 0x247A, 0xC9051DFC }, { 0x248B, 0xC9051DFC }, { 0x2499, 0xC9051DFC }, { 0x24C6, 0xC9051DFC },
        { 0x24D8, 0xC9051DFC }, { 0x2505, 0xE9DC1F60 }, { 0x2570, 0xCBE0A0A3 }, { 0x25E0, 0xC9051DFC },
        { 0x25E8, 0xC9051DFC },
    };

   GraphTests::testGraphs(g, doc);
}

void GraphTests::testSwitchCase(const RDDocument* doc)
{
    static TestGraphs g = {
        { 0x1000, 0x396D947F }, { 0x1030, 0xC9051DFC }, { 0x1040, 0xC9051DFC },
        { 0x1050, 0xC9051DFC }, { 0x1060, 0xC9051DFC }, { 0x1070, 0xC9051DFC },
        { 0x10A0, 0x8BCB152A }, { 0x1110, 0x876EEDD4 }, { 0x1160, 0x74F2AA35 },
        { 0x1169, 0x67D6621D }, { 0x1270, 0xCB53DDBC }, { 0x12E0, 0xC9051DFC },
        { 0x12E8, 0xC9051DFC },
    };

   GraphTests::testGraphs(g, doc);
}

void GraphTests::testGraphs(const GraphTests::TestGraphs& tg, const RDDocument* doc)
{
    for(const auto& [address, hash] : tg)
    {
        RDGraph* g = nullptr;
        REQUIRE(RDDocument_GetFunctionGraph(doc, address, &g));
        REQUIRE_EQ(RDGraph_Hash(g), hash);
    }
}
