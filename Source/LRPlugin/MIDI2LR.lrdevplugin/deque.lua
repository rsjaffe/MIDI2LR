--- Deque implementation by Pierre 'catwell' Chapuis
--- MIT licensed (see LICENSE.txt)

local push_right = function(self, x)
  assert(x ~= nil)
  self.tail = self.tail + 1
  self[self.tail] = x
end

local push_left = function(self, x)
  assert(x ~= nil)
  self[self.head] = x
  self.head = self.head - 1
end

local peek_right = function(self)
  return self[self.tail]
end

local peek_left = function(self)
  return self[self.head+1]
end

local pop_right = function(self)
  if self:is_empty() then return nil end
  local r = self[self.tail]
  self[self.tail] = nil
  self.tail = self.tail - 1
  return r
end

local pop_left = function(self)
  if self:is_empty() then return nil end
  local r = self[self.head+1]
  self.head = self.head + 1
  local r = self[self.head]
  self[self.head] = nil
  return r
end

local rotate_right = function(self, n)
  n = n or 1
  if self:is_empty() then return nil end
  for i=1,n do self:push_left(self:pop_right()) end
end

local rotate_left = function(self, n)
  n = n or 1
  if self:is_empty() then return nil end
  for i=1,n do self:push_right(self:pop_left()) end
end

local _remove_at_internal = function(self, idx)
  for i=idx, self.tail do self[i] = self[i+1] end
  self.tail = self.tail - 1
end

local remove_right = function(self, x)
  for i=self.tail,self.head+1,-1 do
    if self[i] == x then
      _remove_at_internal(self, i)
      return true
    end
  end
  return false
end

local remove_left = function(self, x)
  for i=self.head+1,self.tail do
    if self[i] == x then
      _remove_at_internal(self, i)
      return true
    end
  end
  return false
end

local length = function(self)
  return self.tail - self.head
end

local is_empty = function(self)
  return self:length() == 0
end

local contents = function(self)
  local r = {}
  for i=self.head+1,self.tail do
    r[i-self.head] = self[i]
  end
  return r
end

local iter_right = function(self)
  local i = self.tail+1
  return function()
    if i > self.head+1 then
      i = i-1
      return self[i]
    end
  end
end

local iter_left = function(self)
  local i = self.head
  return function()
    if i < self.tail then
      i = i+1
      return self[i]
    end
  end
end

local methods = {
  push_right = push_right,
  push_left = push_left,
  peek_right = peek_right,
  peek_left = peek_left,
  pop_right = pop_right,
  pop_left = pop_left,
  rotate_right = rotate_right,
  rotate_left = rotate_left,
  remove_right = remove_right,
  remove_left = remove_left,
  iter_right = iter_right,
  iter_left = iter_left,
  length = length,
  is_empty = is_empty,
  contents = contents,
}

local new = function()
  local r = {head = 0, tail = 0}
  return setmetatable(r, {__index = methods})
end

return {
  new = new,
}