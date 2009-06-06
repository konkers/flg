print("test")
poof(0xf,2,10)
led(0x10,0xff,0x00,0x00)
if switch(0x20,0) then
	print("0")
end
if switch(0x20,1) then
	print("1")
end
if switch(0x20,2) then
	print("2")
end
if switch(0x20,3) then
	print("3")
end

