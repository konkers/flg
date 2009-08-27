	
do
   local t = {}

   t.inc = 8;
   t.n = 0;
   t.frame_delay = 1000;
   t.behavior = nil

   function t:rgb_val(n)
      local state = math.floor(n / (0x100 + self.frame_delay)) % 6
      local idx = n % (0x100 + self.frame_delay)
      
      if idx > 0xff then
	 idx = 0xff
      end

      if state == 0 then
	 return 0xff, idx, 0x00
      elseif state == 1 then
	 return 0xff - idx, 0xff, 0x00
      elseif state == 2 then
	 return 0x00, 0xff, idx
      elseif state == 3 then
	 return 0x00, 0xff - idx, 0xff
      elseif state == 4 then
	 return idx, 0x00, 0xff
      else
	 return 0xff, 0x00, 0xff - idx
      end

   end

   function t:process()
      local red
      local green
      local blue
      local offset
      
      red, green, blue = self:rgb_val(self.n)

      for led = 0, 59 do
	 set_led(led, red, green, blue)
      end
--      print(self.n, red, green, blue)
      self.n = self.n + self.inc
   end
	
   return t
end
