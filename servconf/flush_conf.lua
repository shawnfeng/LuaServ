local serv_set = {
{serv="serviceA",  flush=5},
{serv="serviceB",  flush=10},
{serv="test",  flush=20},
}


function get_flush(serv)
   for _, m in ipairs(serv_set) do
      local fd = string.find(serv, m.serv)
      if fd then
         return m.flush
      end
   end

   return nil

end