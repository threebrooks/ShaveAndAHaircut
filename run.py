import ShaveAndAHaircut as SAAH

def listener_callback():
  print("Got an event!")

def runme():
  saah = SAAH.ShaveAndAHaircut(listener_callback)
  saah.start_listening()
  line = input()
  #while(True):
  #  time.sleep(1000000)
  saah.stop_listening()
  
  godec.blocking_shutdown()
  
runme()

