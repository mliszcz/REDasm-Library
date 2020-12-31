#include "blockcontainer.h"
#include "../../support/error.h"
#include "../../support/utils.h"
#include <optional>

BlockContainer::BlockContainer()
{
    m_oninsert = [](const RDBlock&) { };
    m_onremove = [](const RDBlock&) { };
}

void BlockContainer::whenInsert(const BlockContainer::Callback& cb) { m_oninsert = cb; }
void BlockContainer::whenRemove(const BlockContainer::Callback& cb) { m_onremove = cb; }
void BlockContainer::explored(rd_address start, rd_address end) { this->mark(start, end, BlockType_Unknown, BlockFlags_Explored); }
void BlockContainer::unknown(rd_address start, rd_address end) { this->mark(start, end, BlockType_Unknown, BlockFlags_None); }
void BlockContainer::data(rd_address start, rd_address end) { this->mark(start, end, BlockType_Data, BlockFlags_Explored); }
void BlockContainer::code(rd_address start, rd_address end) { this->mark(start, end, BlockType_Code, BlockFlags_Explored); }
void BlockContainer::exploredSize(rd_address start, size_t size) { this->markSize(start, size, BlockType_Unknown, BlockFlags_Explored); }
void BlockContainer::unknownSize(rd_address start, size_t size) { this->markSize(start, size, BlockType_Unknown, BlockFlags_None); }
void BlockContainer::dataSize(rd_address start, size_t size) { this->markSize(start, size, BlockType_Data, BlockFlags_Explored); }
void BlockContainer::codeSize(rd_address start, size_t size) { this->markSize(start, size, BlockType_Code, BlockFlags_Explored); }

size_t BlockContainer::size(const RDBlock* b)
{
    if(b->start >= b->end) return 0;
    return b->end - b->start;
}

bool BlockContainer::contains(const RDBlock* b, rd_address address) { return (address >= b->start) && (address < b->end); }
bool BlockContainer::empty(const RDBlock* b) { return b->start >= b->end; }

void BlockContainer::mark(rd_address start, rd_address end, rd_type type, rd_flag flags)
{
    if(start > end) REDasmError("Trying to insert an empty block [" + Utils::hex(start) + ", " + Utils::hex(end) + "]");

    auto begit = this->get(start);
    auto endit = this->get(end - 1);

    std::optional<RDBlock> begbl, endbl;

    if(begit != m_container.end())
    {
        begbl = *begit;
        begbl->type = BlockType_Unknown; // Demote to "Unknown"
        begbl->end = start;

        if(BlockContainer::empty(std::addressof(*begbl)))
            begbl = std::nullopt;
    }

    if(endit != m_container.end())
    {
        endbl = *endit;
        endbl->type = BlockType_Unknown; // Demote to "Unknown"
        endbl->start = end;

        if(BlockContainer::empty(std::addressof(*endbl)))
            endbl = std::nullopt;
    }

    if((begit != m_container.end()) && (endit != m_container.end())) this->doRemove(begit, std::next(endit));
    else if(begit != m_container.end()) this->doRemove(begit);
    else if(endit != m_container.end()) this->doRemove(endit);

    if(begbl)
    {
        if((begbl->type == type) && (type == BlockType_Unknown))
            start = begbl->start;
        else
            this->doInsert(*begbl);
    }

    if(endbl)
    {
        if((endbl->type == type) && (type == BlockType_Unknown))
            end = endbl->end;
        else
            this->doInsert(*endbl);
    }

    this->doInsert({{start}, end, type, flags});
}

void BlockContainer::markSize(rd_address start, size_t size, rd_type type, rd_flag flags) { this->mark(start, start + size, type, flags); }

void BlockContainer::doInsert(const RDBlock& b)
{
    m_container.insert(b);
    m_oninsert(b);
}

BlockContainer::ContainerType::const_iterator BlockContainer::get(rd_address address) const
{
    auto it = m_container.lower_bound(address);

    while(it != m_container.end()) {
        if(BlockContainer::contains(std::addressof(*it), address)) break;
        if(it == m_container.begin()) break;
        it--;
    }

    return it;
}
