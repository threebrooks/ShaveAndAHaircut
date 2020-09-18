#import asyncio
import ShaveAndAHaircut as SAAH
import sys
#from kasa import SmartPlug
#from phue import Bridge

#ikealicht = SmartPlug("192.168.86.155")
#biglight = Bridge('192.168.86.26')
#biglight.connect()

async def Ikealicht(onoff):
  await ikealicht.update()
  if (onoff):
    await ikealicht.turn_on()
  else:
    await ikealicht.turn_off()

async def Biglight(onoff):
  if (onoff):
    biglight.set_light(4, 'on', True)
    biglight.set_light(4, 'bri', 254)
  else:
    biglight.set_light(4, 'on', False)
 
saah = SAAH.ShaveAndAHaircut()
saah.start_listening()
onoff = True
while(saah.wait_for_event()):
  onoff = not onoff
  #asyncio.run(Ikealicht(onoff))
  #asyncio.run(Biglight(onoff))
  print("Onoff: "+str(onoff))
saah.stop_listening()
saah.shutdown()
