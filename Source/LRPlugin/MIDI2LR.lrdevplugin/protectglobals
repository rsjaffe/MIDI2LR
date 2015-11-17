-- This module is used for debugging and may be eliminated (along with the require statements) in production
--===================================================
--=  Niklas Frykholm 
-- basically if user tries to create global variable
-- the system will not let them!!
-- call GLOBAL_lock(_G)
--
--===================================================
function GLOBAL_lock(t)
  local mt = getmetatable(t) or {}
  mt.__newindex = lock_new_index
  setmetatable(t, mt)
end

--===================================================
-- call GLOBAL_unlock(_G)
-- to change things back to normal.
--===================================================
function GLOBAL_unlock(t)
  local mt = getmetatable(t) or {}
  mt.__newindex = unlock_new_index
  setmetatable(t, mt)
end

function lock_new_index(t, k, v)
  if (k~="_" and string.sub(k,1,2) ~= "__") then
    GLOBAL_unlock(_G)
    error("GLOBALS are locked -- " .. k ..
      " must be declared local or prefix with '__' for globals.", 2)
  else
    rawset(t, k, v)
  end
end

function unlock_new_index(t, k, v)
  rawset(t, k, v)
end
