#include <lua.h>
#include <lauxlib.h>
#include "zstd.h"

// from
// https://github.com/facebook/zstd  v1.5.7
// https://github.com/rangercyh/lua-zstd
// https://github.com/hao3039032/zstd-lua/blob/main/lzstd.cpp
static int lua_zstd_version_number(lua_State *L) {
    lua_settop(L, 0);

    lua_pushinteger(L, ZSTD_versionNumber());
    return 1;
}

static int lua_zstd_version(lua_State *L) {
    lua_settop(L, 0);

    lua_pushstring(L, ZSTD_versionString());
    return 1;
}

static int lua_zstd_compress(lua_State *L) {
    lua_settop(L, 2);
    size_t data_len;
    const char * data = luaL_checklstring(L, 1, &data_len);
    int level = luaL_optinteger(L, 2, ZSTD_defaultCLevel());

    lua_settop(L, 0);

    if (level < ZSTD_minCLevel() || level > ZSTD_maxCLevel()) return luaL_error(L, "zstd clevel not available, min level %d, max level %d", ZSTD_minCLevel(), ZSTD_maxCLevel());

    size_t dst_size = ZSTD_compressBound(data_len);
    if (ZSTD_isError(dst_size)) {
        return luaL_error(L, "zstd error: %s", ZSTD_getErrorName(dst_size));
    }

    luaL_Buffer lbuf;
    char *buf = luaL_buffinitsize(L, &lbuf, dst_size);
    size_t result = ZSTD_compress(buf, dst_size, data, data_len, level);
    if (ZSTD_isError(result)) {
        return luaL_error(L, "zstd error: %s", ZSTD_getErrorName(result));
    }

    luaL_pushresultsize(&lbuf, result);
    return 1;
}

static int lua_zstd_decompress(lua_State *L) {
    lua_settop(L, 1);
    size_t data_len;
    const char * data = luaL_checklstring(L, 1, &data_len);

    lua_settop(L, 0);

    size_t dst_size = ZSTD_getFrameContentSize(data, data_len);
    if (dst_size == ZSTD_CONTENTSIZE_UNKNOWN) {
        return luaL_error(L, "zstd decompress size cannot be determined, please use streaming mode decompress it");
    } else if (dst_size == ZSTD_CONTENTSIZE_ERROR) {
        return luaL_error(L, "zstd decompress size determined error");
    }

    luaL_Buffer lbuf;
    char *buf = luaL_buffinitsize(L, &lbuf, dst_size);
    size_t result = ZSTD_decompress(buf, dst_size, data, data_len);
    if (ZSTD_isError(result)) {
        return luaL_error(L, "zstd error: %s", ZSTD_getErrorName(result));
    }

    luaL_pushresultsize(&lbuf, result);
    return 1;
}


LUAMOD_API int
luaopen_zstd(lua_State *L) {
    luaL_checkversion(L);
    luaL_Reg l[] = {
        {"version_number", lua_zstd_version_number},
        {"version", lua_zstd_version},
        {"compress", lua_zstd_compress},
        {"decompress", lua_zstd_decompress},
        { NULL, NULL },
    };
    luaL_newlib(L, l);
    return 1;
}
