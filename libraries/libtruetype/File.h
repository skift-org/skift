#pragma once

#include <libsystem/io/File.h>

#include <libtruetype/Header.h>
#include <libtruetype/Utils.h>

#include <libtruetype/tables/CMAP.h>
#include <libtruetype/tables/GLYF.h>
#include <libtruetype/tables/HEAD.h>
#include <libtruetype/tables/HHEA.h>
#include <libtruetype/tables/HMTX.h>
#include <libtruetype/tables/LOCA.h>
#include <libtruetype/tables/MAXP.h>

namespace truetype
{

class File
{
private:
    String _path = "none";
    Slice _data{};
    bool _valid = false;

    const Header *_header;

    const CMAP *_cmap = nullptr;
    const GLYF *_glyf = nullptr;
    const HEAD *_head = nullptr;
    const HHEA *_hhea = nullptr;
    const HMTX *_hmtx = nullptr;
    const LOCA *_loca = nullptr;
    const MAXP *_maxp = nullptr;

public:
    const CMAP *cmap() { return _cmap; }
    const GLYF *glyf() { return _glyf; }
    const HEAD *head() { return _head; }
    const HHEA *hhea() { return _hhea; }
    const HMTX *hmtx() { return _hmtx; }
    const LOCA *loca() { return _loca; }
    const MAXP *maxp() { return _maxp; }

    File(String path)
    {
        _path = path;

        auto result_or_data = file_read_all(path);

        if (!result_or_data.success())
        {
            return;
        }
        else
        {
            _data = result_or_data.take_value();
        }

        logger_info("The file is %d bytes", _data.size());

        if (_data.size() < sizeof(Header))
        {
            return;
        }

        _header = (const Header *)_data.start();

        if (_data.size() < sizeof(Header) + sizeof(Element) * _header->table_count())
        {
            return;
        }

        _cmap = get_table<CMAP>();
        _glyf = get_table<GLYF>();
        _head = get_table<HEAD>();
        _hhea = get_table<HHEA>();
        _hmtx = get_table<HMTX>();
        _loca = get_table<LOCA>();
        _maxp = get_table<MAXP>();

        _valid = true;
    }

    template <typename T>
    T *get_table()
    {
        assert(_header);

        for (size_t i = 0; i < _header->table_count(); i++)
        {
            auto &el = _header->tables[i];

            auto table = _data.slice(el.offset(), el.length());

            logger_trace("size: %d any:%d match:%d checksum:%8x==%8x", table.size(), table.any(), el.match(T::NAME), checksum(table), el.checksum());

            // FIXME: Head table checksum not matching...
            if (table.any() &&
                el.match(T::NAME) /* &&
                checksum(table) == el.checksum()*/
            )
            {
                logger_trace("table(%s) found while loading %s !", T::NAME, _path.cstring());
                return (T *)table.start();
            }
        }

        logger_warn("table(%s) not found while loading %s!", T::NAME, _path.cstring());
        return nullptr;
    }
};

} // namespace truetype
