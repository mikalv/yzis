_REQUIREDNAME=true    -- small hack - Lua 5.1 does not have _REQUIREDNAME

require('luaunit')
require('test_bugs1')
require('test_excommands')
require('test_lua_binding')
require('test_utils')
require('test_movements')
require('test_LHM_commands')
require('test_percent_command')
require('test_changes')
require('test_search')
require('test_regexp')
require('test_vim_pattern')
require('test_insert_mode')
require('test_mode_command_parser')
require('test_undo')

-- ret = LuaUnit:run('TestLuaBinding:test_setline') -- will execute only one test
-- ret = LuaUnit:run('TestMovements') -- will execute only one class of test
ret = LuaUnit:run() -- will execute all tests
setLuaReturnValue( ret )

