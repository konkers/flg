An Open Pixel Control (OPC) server for driving Soma, for use when Soma is
installed (with LEDs only) at Pier 14.  OPC is a standardized, simple protocol
for controlling arrays of RGB LEDs.  The server accepts RGB light sequences
from OPC clients, converts those sequences to the Soma-specific protocol, and
sends them down the RS-485 bus to the LED boards.

OPC client programs that drive the light sequences can be written in any
programming language, so long as they can make TCP connections to the OPC
server.  For more information about OPC, see <http://openpixelcontrol.org/>

To run the server, try "make run"

                                -- Michael C. Toren <mct@toren.net>
                                   Sat, 28 Sep 2013 02:44:18 -0700
