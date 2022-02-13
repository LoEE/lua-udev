local Pretty = require'interactive'
local M = {}

--: ANSI terminal colors and other goodies
local colors = {
  red = "31m", green = "32m", yellow = "33m",
  blue = "34m", magenta = "35m", cyan = "36m",
  white = "37m",
  norm = "m"
}

local p = Pretty:new{}
M.p = p

local unq = {
  __tostring = function (self) return table.concat(self, ' ') end
}

function M.unq(...)
  return setmetatable({...}, unq)
end

local function color (c)
  if colors[c] then
    return "\27[" .. colors[c]
  else
    error ("unknown color: " .. p:format (c))
  end
end

function M.eprintf (...)
  local args = {...}
  local len = #args
  local t = {}
  for i,v in ipairs (args) do
    t[#t+1] = tostring (v)
    if i < len then
      t[#t+1] = ' '
    else
      t[#t+1] = '\n'
    end
  end
  io.stderr:write (table.concat (t))
end

local function D(c)
  return function (msg, nonl)
    return function (...)
      local args = "" 
      if select('#', ...) > 0 then
        args = (msg and " " or "") .. p:format (...)
      end
      local t = color (c) .. (msg or '') .. args .. color'norm' .. (nonl and '' or '\n')
      io.stderr:write (t)
      return ...
    end
  end
end

local function __index (self, color)
  if colors[color] then
    self[color] = D(color)
    return self[color]
  end
end

local function __call (self, ...)
  return self.blue(...)
end

function table.index (t, v)
  for i,x in ipairs (t) do
    if x == v then
      return i
    end
  end
end

do
  local matches =
  {
    ["^"] = "%^";
    ["$"] = "%$";
    ["("] = "%(";
    [")"] = "%)";
    ["%"] = "%%";
    ["."] = "%.";
    ["["] = "%[";
    ["]"] = "%]";
    ["*"] = "%*";
    ["+"] = "%+";
    ["-"] = "%-";
    ["?"] = "%?";
    ["\0"]= "%z";
  }
  function string.pattern_quote (s)
    return (s:gsub(".", matches))
  end
end

function string.strip (str, chars)
  if not str then return nil end
  if chars then
    chars = "["..string.quote_patterns(chars).."]"
  else
    chars = "[ \t\r\n]"
  end
  return string.match(str, "^"..chars.."*(.-)"..chars.."*$")
end

local ssub = string.sub
local sfind = string.find

function string.split (str, pat, n)
  -- FIXME: transform into a closure based iterator?
  pat = pat or "[ \t\r\n]+"
  n = n or #str
  local r = {}
  local s, e = sfind (str, pat, 1)
  if not s then return {str} end
  if s ~= 1 then r[#r+1] = ssub(str, 1, s - 1) end
  while true do
    if e == #str then return r end
    local ne
    s, ne = sfind (str, pat, e + 1)
    if not s or #r >= n then r[#r+1] = ssub(str, e + 1, #str) return r end
    r[#r+1] = ssub(str, e + 1, s - 1)
    e = ne
  end
end

function string.splitv (str, pat, n)
  return unpack(string.split (str, pat, n))
end

--[[ tests
do
  local split = function (str, pat, n) return yd('split', string.split (str, pat, n)) end
  split('foo/bar/baz/test','/')
  --> {'foo','bar','baz','test'}
  split('/foo/bar/baz/test','/')
  --> {'foo','bar','baz','test'}
  split('/foo/bar/baz/test/','/')
  --> {'foo','bar','baz','test'}
  split('/foo/bar//baz/test///','/')
  --> {'foo','bar','','baz','test','',''}
  split('//foo////bar/baz///test///','/+')
  --> {'foo','bar','baz','test'}
  split('foo','/+')
  --> {'foo'}
  split('','/+')
  --> {}
  split('foo','')  -- splits a zero-sized string 3 (#str) times
  --> {'','','',''}
  split('a|b|c|d','|',2)
  --> {'a','b','c|d'}
  split('|a|b|c|d|','|',2)
  --> {'a','b','c|d|')
end
--]]

function string.splitall (str, pat, n)
  -- FIXME: transform into a closure based iterator?
  pat = pat or "[ \t\r\n]+"
  n = n or #str
  local r = {}
  local s = 0
  local e = 0
  while true do
    local ne
    s, ne = sfind (str, pat, e + 1)
    if not s or #r >= n then r[#r+1] = ssub(str, e + 1, #str) return r end
    r[#r+1] = ssub(str, e + 1, s - 1)
    e = ne
  end
end

function string.splitallv (str, pat, n)
  return unpack(string.splitall (str, pat, n))
end

--[[ tests
do
  local split = function (str, pat, n) local t = string.splitall (str, pat, n) yd('splitall', pat, str == table.concat (t, pat), t) return t end
  split('foo/bar/baz/test','/')
  --> {'foo','bar','baz','test'}
  split('/foo/bar/baz/test','/')
  --> {'','foo','bar','baz','test'}
  split('/foo/bar/baz/test/','/')
  --> {'','foo','bar','baz','test',''}
  split('/foo/bar//baz/test///','/')
  --> {'','foo','bar','','baz','test','','',''}
  split('//foo////bar/baz///test///','/+')
  --> {'','foo','bar','baz','test',''}
  split('foo','/+')
  --> {'foo'}
  split('','/+')
  --> {}
  split('foo','')  -- splits a zero-sized string 3 (#str) times
  --> {'','','',''}
  split('a|b|c|d','|',2)
  --> {'a','b','c|d'}
end
--]]

do
  pcall (function () require 'luarocks.loader' end)
  local socket = require'socket'
  local names = {}
  local times = {}
  function M.rtime (name)
    names[#names + 1] = name
    times[#times + 1] = socket.gettime()
  end
  function M.rtimedump ()
    local maxl = 0
    for i,name in ipairs(names) do maxl = math.max (maxl, #name) end
    local tab = string.rep (' ', maxl + 2)
    for i = 2,#times do
      local t = times[i] - times[i-1]
      if t > 1 then
        t = string.format("%6.2f s", t)
      else
        t = string.format("%4.2f ms", t * 1000)
      end
      print (tab .. t .. '\r' .. names[i - 1])
    end
  end
end

return setmetatable(M, {__call = __call, __index = __index})

--[=[ interactive registry dumper
function repl.ns.REG(table,index)
  local ans = ""
  local dash = "--------------"
  local function dumpreg(table,index)
    local x,y
    print(dash)
    for x,y in pairs(table) do
      if type(y) == "table" then
        print(string.format("%-30s%-20s",index,tostring(x)),tostring(y),"     Enter table y or q")
        ans = io.read()
        if ans == 'y' then
          dumpreg(y,tostring(x))
        end
        if ans == 'q' then
          return
        end
      else
        print(string.format("%-30s%-20s",index,tostring(x)),tostring(y))
      end
    end
    print(dash)
  end

  print(string.format("%-30s%-25s%s","Table","Index","Type"))
  dumpreg(table or debug.getregistry(),index or "Registry")
  print("\nend of program")
end
--]=]
