FCEUX port for Blackberry Playbook  

FCEUX is a Nintendo Entertainment System and 'Famicom' emulator of the classic 8bit console of the 1980s. 
This project uses SDL, TouchControlOverlay and the native development kit to implement a basic SDL port of FCEUX.
Currently there are glitches with sprite rendering but it's playable.  Input layout needs more work for sure.

http://www.fceux.com

Thanks:  FCEUX developers! 
         Jeremy Nicholl for detailed and quick responses to questions and his TCO library, and for fixing the audio init glitch.
         
Default game startup:

1. Create a directory called z:/misc/nes/roms 
2. Put your .nes files ( .zip not supported yet ) in this directory.
   - Currently one game has to be called 'game.nes'for it to startup.
   - This will be fixed up shortly.
   
3. You can tap the top left portion of the screen and it will autoload the next rom in the directory.  
4. Swipe down from top of screen to see the TCO controls layout
   - You can move each 'key' around as you like, the game will pause during this time, swipe down again to go back.

         

Issues:

1. sound stops on every second load when you load new games via 'tapping'
2. Sometimes exits if you tap too quickly
3. game speed issues 
4. game.nes required on first load in z:misc/roms/nes dir.


Todo:

1. labels for pad 
2. better key layout , maybe dpad support.
3. compile LUA support back in
4. OpenGL port to GLES ( so change GL_QUAD, begin-end sequences etc )
5. USB host joystick support, should be possible on 2.x release so we can use proper pads.
6. LUA compile, it's useful for debugging and other users not so much for game playing.
7. Network support ( it's compiled in, but original source is broken I believe )
 
 
contact: trevor.nunes@gmail.com
 
 
 
