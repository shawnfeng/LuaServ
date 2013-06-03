FT_1 = {}

print("Hello World !")
local FT = FT_1
FT.test = function(s)
      print("in 1: "..s)
      FT_2.test("call 1")
      justtest.just_fuck(1.2)
      return "Hello World"
end



FT.testvar = "Hello"
FT.testvar = {}

local show = function(nm, ud)
   --print(nm.." meta:", getmetatable(ud), type(ud), ud)
   print(nm.." meta:", getmetatable(_G[nm]), type(_G[nm]), _G[nm])
end


local int64 = int64.new
a = int64(1)
b = 1+int64(2)
c = a+b
show("a")
show("b")
show("c")
print(type(a+b*b+a), a+b*b+a)
d = a+b+c*b
show("d")
-- 这个测试很关键
e = a/b+c
show("e")
f = a + "123"
show("f")

FT.testint64 = function(s)
   local aa = int64(1)
   print("test int64 meta:", getmetatable(aa), type(aa), aa)
   print("test int64 meta:", getmetatable(s), type(s), s)

   justtest.int64_test(s)
   local stest = {a = "321", c = "add", b = int64(11), d = false}
   justtest.struct_test(stest)
   rv = justtest.table_return_test()
   print(type(rv))
   print(type(rv.t0))
   print(type(rv.t0.aa))
   print(type(rv.t1))

   local stest = {aad = 22, a = "333", c = "fuck", b = int64(232), d = true, e = {}, f = false}
   justtest.struct_try_test(stest)

--   local stest  = {1, 3, 3}
   ok, err = justtest.test_map(stest)
   print("rv", ok, err)

   justtest.test_string_cast()
   justtest.test_string_cast(nil)
   justtest.test_string_cast(1)
   justtest.test_string_cast(int64(1))
   justtest.test_string_cast("dddd")
   justtest.test_string_cast({})

   justtest.test_string_cast(nil)
   justtest.test_string_cast("dddd")

   justtest.test_string_cast({})
   justtest.test_string_cast(int64(3))
   justtest.test_string_cast(123)
   justtest.test_string_cast("123")
--   justtest.test_string_check(12333)
--   justtest.test_string_check("dafd")
--   justtest.test_string_check(nil)
  -- justtest.test_string_check(adasfsdfsdf)
   --justtest.test_string_check({})
   --justtest.just_fuck(1.2)
end


--[[
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
]]--

