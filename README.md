FCEUXpb the FCEUX port for Blackberry Playbook  

FCEUX is a Nintendo Entertainment System and 'Famicom' emulator of the classic 8bit console of the 1980s. 
This project uses SDL, TouchControlOverlay and the native development kit to implement a basic SDL port of FCEUX.
Currently there are glitches with sprite rendering but it's playable.  Input layout needs more work for sure.

http://www.fceux.com

Thanks:  FCEUX developers! 
         Jeremy Nicholl for detailed and quick responses to questions and his TCO library, and for fixing the audio init glitch.
         
Default game startup:

1. Create a directory called z:/misc/nes/roms 

2. Put your .nes files ( .zip not supported yet ) in this directory.
   
3. You can tap the top left portion of the screen and it will autoload the next rom in the directory.  

4. Swipe down from top of screen to see the TCO controls layout
   - You can move each 'key' around as you like, the game will pause during this time, swipe down again to go back.

         


History
===========================================
v1.0.0.3  11/31/2011

- graphic overlays added
- rom failure skipping, prevents exit on failed loads



v1.0.0.0-2  11/24/2011 initial work


Todo:

1. better key layout , maybe dpad support.
2. compile LUA support back in
3. OpenGL port to GLES ( so change GL_QUAD, begin-end sequences etc )
4. USB host joystick support, should be possible on 2.x release so we can use proper pads.
5. Network support ( it's compiled in, but original source is broken I believe )
 
 
contact: trevor.nunes@gmail.com
 
 
 
