local serv_set = {
{serv="serviceA",  id=1},
{serv="serviceA",  id=2},

{serv="serviceB",  id=2},
{serv="test",  id=1},
{serv="test%d*",  id=2},

--{serv="test",  id=1000},
--{serv="test",  id=1},
}

function get_serv_set(serv)
   print("LUA:call get_serv_set")
   idset = {}
   for _, m in ipairs(serv_set) do
      local fd = string.find(serv, m.serv)
      if fd then
         idset[#idset+1] = m.id
      end
   end

   return idset
   --return nil

end

