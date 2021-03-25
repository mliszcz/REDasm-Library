#pragma once

#include "surfacerenderer.h"
#include "surfacepath.h"
#include "cursor.h"

class BlockContainer;
class Renderer;

class Surface: public SurfaceRenderer
{
    private:

    public:
        Surface(Context* ctx, rd_flag flags, uintptr_t userdata);
        ~Surface();
        const CursorPtr& cursor() const;
        size_t getPath(const RDPathItem** path) const;
        const std::string* currentWord() const;
        const std::string* wordAt(int row, int col) const;
        const std::string& selectedText() const;
        rd_address firstAddress() const;
        rd_address lastAddress() const;
        rd_address currentAddress() const;
        const std::string* currentLabel(rd_address* resaddress) const;
        uintptr_t userData() const;
        rd_address getAddress(int row) const;
        void getScrollRange(rd_address* start, rd_address* end) const;
        bool contains(rd_address address) const;
        bool labelAt(int row, int col, rd_address* resaddress) const;
        bool goTo(rd_address address, bool updatehistory = true);
        bool goTo(const RDBlock* block, bool updatehistory = true);
        void setUserData(uintptr_t userdata);
        void getSize(int* rows, int* cols) const;
        void scroll(rd_address address, int ncols);
        void resize(int rows, int cols);
        void moveTo(int row, int col);
        void select(int row, int col);
        void selectAt(int row, int col);
        void linkTo(Surface* s);
        void unlink();
        void activate();
        void deactivate();
        bool active() const;

    public: // Used by Cursor class
        void notifyHistoryChanged();
        void notifyAddressChanged();

    protected:
        void updateCompleted(rd_address currentaddress) override;

    private:
        void handleEvents(const RDEventArgs* event);
        void checkColumn(int row, int& col) const;
        bool centerOnSurface(const BlockContainer* blocks, rd_address address);
        void scrollAddress(rd_address address);
        void drawCursor();
        void highlightCurrentRow();
        void highlightWords();
        void checkSelection();

    private:
        CursorPtr m_cursor;
        SurfacePath m_path;
        std::string m_selectedtext;
        uintptr_t m_userdata;
        bool m_active{false};
};
