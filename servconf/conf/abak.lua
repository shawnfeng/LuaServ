FT_1 = {}


local FT = FT_1
FT.test = function(s)
      print("in 1: "..s)
      FT_2.test("call 1")
      justtest.just_fuck(1.2)
      return "Hello World"
end

FT.testvar = "Hello"
FT.testvar = {}

print("Hello World")
lib = libint64
local int64 = lib.new
print(type(int64))
a = int64(1)
b = 1+int64(2)
print(a+b*b+a)

print("====================================================")
print("------------------afs")
print(type(a))
print(a)
print(b)
print(a+b+b)
print("------------------afs2")
print(a-b)
print(b-a)
print(b-1)
print("------------------afs3")
c=b+b
print(c)
c=int64(b+b)
print("-da---------------", c)
print(b+b)
print("-----------------")
print(-b)
c=a/b+c
print(getmetatable(a))
print(getmetatable(b))
print(getmetatable(c))
print(getmetatable(a+b))
print(c)
--print(a/b)

print(string.format("--------------%s", tostring(b)))



b = 1 + int64(1)
--b = int64 "\16" + int64("9",10)
--print(lib.tostring(a,10), lib.tostring(b,2))
print("+", a+b)
--print("-", lib.tostring(a-b,10))
print("*", a*b)
print("/", a/b)
print("%", a%b)
print("^", a^b)
print("==", a == b)
print(">", a > b)
print("#", #a)


--[[
local cjson = require "cjson"
require "lxp"


local cjson2 = cjson.new()
local cjson_safe = require "cjson.safe"

json_text = '[ true, { "foo": "bar" } ]'
print(json_text)
value = cjson.decode(json_text)
print(value)

jj = 'fuck beauty'
print(jj)
]]--

