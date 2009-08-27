	
do
   local t = {}

   t.inc = 4;
   t.frame_delay = 2000
   t.behavior = nil
   t.states = {0,
	       256 + 0 * 32,
	       256 + 1 * 32,
	       256 + 2 * 32,
	       256 + 3 * 32,
	       256 + 4 * 32,
	       256 + 5 * 32,
	       256 + 6 * 32,
	       256 + 7 * 32,
	       256 + 8 * 32,
	       256 + 9 * 32,
	       384 + 9 * 32}

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
      
      red, green, blue = self:rgb_val(self.states[1])
      for i, name in pairs(upper_arial_leds) do--
	 set_led(leds[name], red, green, blue)
      end

      red, green, blue= self:rgb_val(self.states[2])
      set_led(leds["a10"], red, green, blue)
      
      red, green, blue= self:rgb_val(self.states[3])
      set_led(leds["a9"], red, green, blue)

      red, green, blue= self:rgb_val(self.states[4])
      set_led(leds["a8"], red, green, blue)
      
      red, green, blue= self:rgb_val(self.states[5])
      set_led(leds["a7"], red, green, blue)
      
      red, green, blue= self:rgb_val(self.states[6])
      set_led(leds["a6"], red, green, blue)
      
      red, green, blue= self:rgb_val(self.states[7])
      set_led(leds["a5"], red, green, blue)
      
      red, green, blue= self:rgb_val(self.states[8])
      set_led(leds["a4"], red, green, blue)
      
      red, green, blue= self:rgb_val(self.states[9])
      set_led(leds["a3"], red, green, blue)
      
      red, green, blue= self:rgb_val(self.states[10])
      set_led(leds["a2"], red, green, blue)
      
      red, green, blue= self:rgb_val(self.states[11])
      set_led(leds["a1"], red, green, blue)
      


      red, green, blue = self:rgb_val(self.states[12])
      for i, name in pairs(lower_arial_leds) do
	 set_led(leds[name], red, green, blue)
      end

--      for name, index in pairs(leds) do
--	 set_led(index, red, green, blue)
--      end
--      print(self.n, red, green, blue)

      for i, val in pairs(self.states) do
	 self.states[i] = val + self.inc
      end
   end
	
   return t
end
