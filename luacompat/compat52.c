#include "luacompat52.h"
#include <assert.h>

#if (LUA_VERSION_NUM == 501)

LUALIB_API void 
luaL_init(lua_State *L) {
	lua_pushglobaltable(L);
	lua_setglobal(L, "_ENV");
	lua_pushglobaltable(L);
	lua_rawseti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
	int mt = lua_pushthread(L);
	if (mt == 0) {
		luaL_error(L, "luaL_init must be call in mainthread");
	}
	lua_rawseti(L, LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
}

LUA_API int
lua_absindex(lua_State *L, int idx) {
	if (idx > 0 || idx <= LUA_REGISTRYINDEX)
		return idx;
	else {
		return lua_gettop(L) + idx + 1;
	}
}

LUA_API int 
lua_compare(lua_State *L, int idx1, int idx2, int op) {
	switch(op) {
	case LUA_OPEQ:
		return lua_equal(L,idx1,idx2);
	case LUA_OPLT:
		return lua_lessthan(L, idx1, idx2);
	case LUA_OPLE:
		return !lua_lessthan(L, idx2, idx1);
	}
	assert(0);
	return 0;
}

LUA_API void
lua_copy(lua_State *L, int fromidx, int toidx) {
	lua_pushvalue(L, fromidx);
	lua_replace(L, toidx);
}

LUA_API void
lua_getuservalue(lua_State *L, int idx) {
	lua_getfenv(L, idx);
}

LUA_API void
lua_setuservalue(lua_State *L, int idx) {
	lua_setfenv(L, idx);
}

LUA_API void 
lua_len(lua_State *L, int idx) {
	int hasmeta = luaL_callmeta (L, idx, "__len");
	if (hasmeta == 0) {
		size_t sz = lua_objlen(L, idx);
		lua_pushnumber(L, sz);
	}
}

LUA_API void
lua_rawgetp(lua_State *L, int idx, const void *p) {
	idx = lua_absindex(L, idx);
	lua_pushlightuserdata(L, (void *)p);
	lua_rawget(L, idx);
}

LUA_API void
lua_rawsetp(lua_State *L, int idx, const void *p) {
	idx = lua_absindex(L, idx);
	lua_pushlightuserdata(L, (void *)p);
	lua_insert(L, -2);
	lua_rawset(L, idx);
}

LUA_API lua_Number 
lua_tonumberx(lua_State *L, int index, int *isnum) {
	lua_Number n = lua_tonumber(L, index);
	if (isnum) {
		if (n != 0) {
			*isnum = 1;
		} else {
			switch (lua_type(L, index)) {
			case LUA_TNUMBER:
				*isnum = 1;
				break;
			case LUA_TSTRING: {
				size_t sz = 0;
				const char * number = lua_tolstring(L, index, &sz);
				if ((sz == 1 && number[0] == '0') || 
					(sz == 3 && number[0] == '0' && (number[1] == 'x' || number[1] == 'X') && number[2] == '0')) {
					*isnum = 1;
				} else {
					*isnum = 0;
				}
				break;
			}
			default:
				*isnum = 0;
				break;
			}
		}
	} 
	return n;
}

LUA_API lua_Unsigned
lua_tounsignedx(lua_State *L, int idx, int *isnum) {
	lua_Number n = lua_tonumberx(L, idx, isnum);
	return (lua_Unsigned)n;
}

LUA_API lua_Integer
lua_tointegerx(lua_State *L, int idx, int *isnum) {
	lua_Number n = lua_tonumberx(L, idx, isnum);
	return (lua_Integer)n;
}

LUA_API const lua_Number *
lua_version(lua_State *L) {
	static const lua_Number version = LUA_VERSION_NUM;
	return &version;
}

LUALIB_API void 
luaL_checkversion_ (lua_State *L, lua_Number ver) {
	const lua_Number *v = lua_version(L);
	if (*v != ver)
		luaL_error(L, "version mismatch: app. needs %f, Lua core provides %f", ver, *v);
  /* check conversions number -> integer types */
  lua_pushnumber(L, -(lua_Number)0x1234);
  if (lua_tointeger(L, -1) != -0x1234 ||
      lua_tounsigned(L, -1) != (lua_Unsigned)-0x1234)
    luaL_error(L, "bad conversion number->int;"
                  " must recompile Lua with proper settings");
  lua_pop(L, 1);
}

LUALIB_API int 
luaL_len (lua_State *L, int idx) {
  int l;
  int isnum;
  lua_len(L, idx);
  l = (int)lua_tointegerx(L, -1, &isnum);
  if (!isnum)
    luaL_error(L, "object length is not a number");
  lua_pop(L, 1);  /* remove object */
  return l;
}

LUALIB_API int 
luaL_getsubtable (lua_State *L, int idx, const char *fname) {
  lua_getfield(L, idx, fname);
  if (lua_istable(L, -1)) return 1;  /* table already there */
  else {
    lua_pop(L, 1);  /* remove previous result */
    idx = lua_absindex(L, idx);
    lua_newtable(L);
    lua_pushvalue(L, -1);  /* copy to be left at top */
    lua_setfield(L, idx, fname);  /* assign new table to field */
    return 0;  /* false, because did not find table there */
  }
}

LUALIB_API lua_Unsigned 
luaL_checkunsigned (lua_State *L, int narg) {
  int isnum;
  lua_Unsigned d = lua_tounsignedx(L, narg, &isnum);
  if (!isnum) {
	return luaL_error(L, "arg %d is not a unsigned", narg);
  }
  return d;
}

LUALIB_API lua_Unsigned 
luaL_optunsigned (lua_State *L, int narg, lua_Unsigned def) {
  return luaL_opt(L, luaL_checkunsigned, narg, def);
}

LUALIB_API void 
luaL_pushresultsize (luaL_Buffer *B, size_t sz) {
  luaL_addsize(B, sz);
  luaL_pushresult(B);
}

LUALIB_API void 
luaL_requiref (lua_State *L, const char *modname,
                               lua_CFunction openf, int glb) {
  lua_pushcfunction(L, openf);
  lua_pushstring(L, modname);  /* argument to open function */
  lua_call(L, 1, 1);  /* open module */
  luaL_getsubtable(L, LUA_REGISTRYINDEX, "_LOADED");
  lua_pushvalue(L, -2);  /* make copy of module (call result) */
  lua_setfield(L, -2, modname);  /* _LOADED[modname] = module */
  lua_pop(L, 1);  /* remove _LOADED table */
  if (glb) {
    lua_pushvalue(L, -1);  /* copy of 'mod' */
    lua_setglobal(L, modname);  /* _G[modname] = module */
  }
}

LUALIB_API void 
luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
  luaL_checkversion(L);
  luaL_checkstack(L, nup, "too many upvalues");
  for (; l->name != NULL; l++) {  /* fill the table with given functions */
    int i;
    for (i = 0; i < nup; i++)  /* copy upvalues to the top */
      lua_pushvalue(L, -nup);
    lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
    lua_setfield(L, -(nup + 2), l->name);
  }
  lua_pop(L, nup);  /* remove upvalues */
}

LUALIB_API void 
luaL_setmetatable (lua_State *L, const char *tname) {
  luaL_getmetatable(L, tname);
  lua_setmetatable(L, -2);
}

LUALIB_API const char *
luaL_tolstring (lua_State *L, int idx, size_t *len) {
  if (!luaL_callmeta(L, idx, "__tostring")) {  /* no metafield? */
    switch (lua_type(L, idx)) {
      case LUA_TNUMBER:
      case LUA_TSTRING:
        lua_pushvalue(L, idx);
        break;
      case LUA_TBOOLEAN:
        lua_pushstring(L, (lua_toboolean(L, idx) ? "true" : "false"));
        break;
      case LUA_TNIL:
        lua_pushliteral(L, "nil");
        break;
      default:
        lua_pushfstring(L, "%s: %p", luaL_typename(L, idx),
                                            lua_topointer(L, idx));
        break;
    }
  }
  return lua_tolstring(L, -1, len);
}

/*
** {======================================================
** Traceback
** =======================================================
*/


#define LEVELS1	12	/* size of the first part of the stack */
#define LEVELS2	10	/* size of the second part of the stack */



/*
** search for 'objidx' in table at index -1.
** return 1 + string at top if find a good name.
*/
static int findfield (lua_State *L, int objidx, int level) {
  if (level == 0 || !lua_istable(L, -1))
    return 0;  /* not found */
  lua_pushnil(L);  /* start 'next' loop */
  while (lua_next(L, -2)) {  /* for each pair in table */
    if (lua_type(L, -2) == LUA_TSTRING) {  /* ignore non-string keys */
      if (lua_rawequal(L, objidx, -1)) {  /* found object? */
        lua_pop(L, 1);  /* remove value (but keep name) */
        return 1;
      }
      else if (findfield(L, objidx, level - 1)) {  /* try recursively */
        lua_remove(L, -2);  /* remove table (but keep name) */
        lua_pushliteral(L, ".");
        lua_insert(L, -2);  /* place '.' between the two names */
        lua_concat(L, 3);
        return 1;
      }
    }
    lua_pop(L, 1);  /* remove value */
  }
  return 0;  /* not found */
}


static int pushglobalfuncname (lua_State *L, lua_Debug *ar) {
  int top = lua_gettop(L);
  lua_getinfo(L, "f", ar);  /* push function */
  lua_pushglobaltable(L);
  if (findfield(L, top + 1, 2)) {
    lua_copy(L, -1, top + 1);  /* move name to proper place */
    lua_pop(L, 2);  /* remove pushed values */
    return 1;
  }
  else {
    lua_settop(L, top);  /* remove function and global table */
    return 0;
  }
}

static void pushfuncname (lua_State *L, lua_Debug *ar) {
  if (*ar->namewhat != '\0')  /* is there a name? */
    lua_pushfstring(L, "function " LUA_QS, ar->name);
  else if (*ar->what == 'm')  /* main? */
      lua_pushfstring(L, "main chunk");
  else if (*ar->what == 'C') {
    if (pushglobalfuncname(L, ar)) {
      lua_pushfstring(L, "function " LUA_QS, lua_tostring(L, -1));
      lua_remove(L, -2);  /* remove name */
    }
    else
      lua_pushliteral(L, "?");
  }
  else
    lua_pushfstring(L, "function <%s:%d>", ar->short_src, ar->linedefined);
}


static int countlevels (lua_State *L) {
  lua_Debug ar;
  int li = 1, le = 1;
  /* find an upper bound */
  while (lua_getstack(L, le, &ar)) { li = le; le *= 2; }
  /* do a binary search */
  while (li < le) {
    int m = (li + le)/2;
    if (lua_getstack(L, m, &ar)) li = m + 1;
    else le = m;
  }
  return le - 1;
}


LUALIB_API void luaL_traceback (lua_State *L, lua_State *L1,
                                const char *msg, int level) {
  lua_Debug ar;
  int top = lua_gettop(L);
  int numlevels = countlevels(L1);
  int mark = (numlevels > LEVELS1 + LEVELS2) ? LEVELS1 : 0;
  if (msg) lua_pushfstring(L, "%s\n", msg);
  lua_pushliteral(L, "stack traceback:");
  while (lua_getstack(L1, level++, &ar)) {
    if (level == mark) {  /* too many levels? */
      lua_pushliteral(L, "\n\t...");  /* add a '...' */
      level = numlevels - LEVELS2;  /* and skip to last ones */
    }
    else {
      lua_getinfo(L1, "Slnt", &ar);
      lua_pushfstring(L, "\n\t%s:", ar.short_src);
      if (ar.currentline > 0)
        lua_pushfstring(L, "%d:", ar.currentline);
      lua_pushliteral(L, " in ");
      pushfuncname(L, &ar);
      lua_concat(L, lua_gettop(L) - top);
    }
  }
  lua_concat(L, lua_gettop(L) - top);
}

/* }====================================================== */

#else

LUALIB_API void 
luaL_init(lua_State *L) {
}

#endif
