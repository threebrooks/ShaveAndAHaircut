#!/usr/bin/python

import sys
from phue import Bridge

b = Bridge('192.168.86.26')

b.connect()

if (sys.argv[1] == "on"):
  b.set_light(4, 'on', True)
  b.set_light(4, 'bri', 254)
else:
  b.set_light(4, 'on', False)

