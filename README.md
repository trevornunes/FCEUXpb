FCEUXpb the FCEUX port for Blackberry Playbook  

FCEUX is a Nintendo Entertainment System and 'Famicom' emulator of the classic 8bit console of the 1980s.  This project depends on SDL and TouchControlOverlay libraries found on the github.com/blackberry pages. 

http://www.fceux.com

Thanks:  FCEUX developers! 
         Jeremy Nicholl for detailed and quick responses to questions and his TCO library, and for fixing the audio init glitch.
         crackberry.com forum members for feedback and 'offthahorseceo' for the NES pad graphics and layout
         
Default game startup:

1. Create a directory called z:/misc/nes/roms 

2. Put your .nes files ( .zip not supported yet ) in this directory.
   
3. You can tap the top left portion of the screen and it will autoload the next rom in the directory.  

4. Swipe down from top of screen to see the TCO controls layout
   - You can move each 'key' around as you like, the game will pause during this time, swipe down again to go back.

         


History
===========================================
v1.0.0.9 05/01/2012
- 3rdparty directory added with inc,lib sub dirs to avoid dependancies with external projects
  this make the code much easier to work with for other software designers.
  
- new controls thanks to 'offthahorseceo' on crackberry forums thanks!
- bluetooth confirmed working on MS 6000 keyboard

  s select
  d B
  f A
  arrow keys up,down,left,right

- overlay display toggle control ( tap top right )
  shift + O via BT keyboard


v1.0.0.8 03/01/2012
- another sdl-controls.xml with buttons moved 50 odd pixels up.
- misc/fceux is now root directory, note: only fceux.cfg is visible via share.
  you need to ssh into the device to fiddle around with .sav files etc.
- rom loader fixes, mutex locking and no delays on rom switching.


v1.0.0.6 01/01/2012

- use shared/misc/fceux as the configuration dir store
- new sdl-controls.xml overlay

v1.0.0.5 11/31/2011
- stuff

v1.0.0.4  11/29/2011
- can't remember :-)

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
 
 
 
