#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "throttle.h"
#include "config.h"

#include "../common/cheat.h"

#include "input.h"
#include "dface.h"

#include "sdl.h"
#include "sdl-video.h"
#include "unix-netplay.h"

#ifdef WIN32
#include <windows.h>
#endif

/**
 * Read a custom pallete from a file and load it into the core.
 */
int
LoadCPalette(const std::string &file)
{
    uint8 tmpp[192];
    FILE *fp;

    if(!(fp = FCEUD_UTF8fopen(file.c_str(), "rb"))) {
        printf(" Error loading custom palette from file: %s\n", file.c_str());
        return 0;
    }
    size_t result = fread(tmpp, 1, 192, fp);
    if(result != 192) {
		printf(" Error reading custom palette from file: %s\n", file.c_str());
		return 0;
	}
    FCEUI_SetPaletteArray(tmpp);
    fclose(fp);
    return 1;
}

/**
 * Creates the subdirectories used for saving snapshots, movies, game
 * saves, etc.  Hopefully obsolete with new configuration system.
 */
static void
CreateDirs(const std::string &dir)
{
    const char *subs[7]={"fcs","snaps","gameinfo","sav","cheats","movies"};
    std::string subdir;
    int x;

#if defined(WIN32) || defined(NEED_MINGW_HACKS)
    mkdir(dir.c_str());
    chmod(dir.c_str(), 755);
    for(x = 0; x < 6; x++) {
        subdir = dir + PSS + subs[x];
        mkdir(subdir.c_str());
    }
#else
    mkdir( dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    chmod( dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);

    for(x = 0; x < 6; x++) {
        subdir = dir + PSS + subs[x];
        fprintf(stderr,"CreateDirs: %s\n", subdir.c_str());
        mkdir(subdir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
        chmod(subdir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    }
#endif
}

/**
 * Attempts to locate FCEU's application directory.  This will
 * hopefully become obsolete once the new configuration system is in
 * place.
 */
static void
GetBaseDirectory(std::string &dir)
{
    char *home = getenv("HOME");

#ifdef __QNXNTO__
    	fprintf(stderr,"GetBaseDirectory: Playbook override to shared/misc/fceux\n");
    	//dir = std::string("/accounts/1000/shared/misc/fceux") + "/.fceux";
    	dir = std::string("/accounts/1000/shared/misc/fceux");
    	return;
#endif

    if(home) {
        dir = std::string(home) + "/.fceux";

#ifdef WIN32
        home = new char[MAX_PATH + 1];
        GetModuleFileName(NULL, home, MAX_PATH + 1);

        char *lastBS = strrchr(home,'\\');
        if(lastBS) {
            *lastBS = 0;
        }

        dir = std::string(home);
        delete[] home;
#else
       dir = "";
#endif
    }
}


Config *
InitConfig()
{
    std::string dir, prefix;
    Config *config;

    GetBaseDirectory(dir);

    FCEUI_SetBaseDirectory(dir.c_str());
    CreateDirs(dir);

    config = new Config(dir);

    // sound options
    config->addOption('s', "sound", "SDL.Sound", 1);
    config->addOption("volume", "SDL.Sound.Volume", 150);
    config->addOption("trianglevol", "SDL.Sound.TriangleVolume", 256);
    config->addOption("square1vol", "SDL.Sound.Square1Volume", 256);
    config->addOption("square2vol", "SDL.Sound.Square2Volume", 256);
    config->addOption("noisevol", "SDL.Sound.NoiseVolume", 256);
    config->addOption("pcmvol", "SDL.Sound.PCMVolume", 256);
    config->addOption("soundrate", "SDL.Sound.Rate", 22500);
    config->addOption("soundq", "SDL.Sound.Quality", 1);
    config->addOption("soundrecord", "SDL.Sound.RecordFile", "");
    config->addOption("soundbufsize", "SDL.Sound.BufSize", 128);
	config->addOption("lowpass", "SDL.Sound.LowPass", 0);
    
    config->addOption('g', "gamegenie", "SDL.GameGenie", 0);
    config->addOption("pal", "SDL.PAL", 0);
    config->addOption("frameskip", "SDL.Frameskip", 0);
    config->addOption("clipsides", "SDL.ClipSides", 0);
    config->addOption("nospritelim", "SDL.DisableSpriteLimit", 1);

    // color control
    config->addOption('p', "palette", "SDL.Palette", "");
    config->addOption("tint", "SDL.Tint", 56);
    config->addOption("hue", "SDL.Hue", 72);
    config->addOption("ntsccolor", "SDL.NTSCpalette", 0);

    // scanline settings
    config->addOption("slstart", "SDL.ScanLineStart", 0);
    config->addOption("slend", "SDL.ScanLineEnd", 239);

    // video controls
    config->addOption('x', "xres", "SDL.XResolution", 512);
    config->addOption('y', "yres", "SDL.YResolution", 448);
    config->addOption('f', "fullscreen", "SDL.Fullscreen", 0);
    config->addOption('b', "bpp", "SDL.BitsPerPixel", 32);
    config->addOption("doublebuf", "SDL.DoubleBuffering", 0);
    config->addOption("autoscale", "SDL.AutoScale", 1);
    config->addOption("keepratio", "SDL.KeepRatio", 1);
    config->addOption("xscale", "SDL.XScale", 1.0);
    config->addOption("yscale", "SDL.YScale", 1.0);
    config->addOption("xstretch", "SDL.XStretch", 0);
    config->addOption("ystretch", "SDL.YStretch", 0);
    config->addOption("noframe", "SDL.NoFrame", 0);
    config->addOption("special", "SDL.SpecialFilter", 0);

    // OpenGL options
    config->addOption("opengl", "SDL.OpenGL", 0);
    config->addOption("openglip", "SDL.OpenGLip", 0);
    config->addOption("SDL.SpecialFilter", 0);
    config->addOption("SDL.SpecialFX", 0);

    // network play options - netplay is broken
    config->addOption("server", "SDL.NetworkIsServer", 0);
    config->addOption('n', "net", "SDL.NetworkIP", "");
    config->addOption('u', "user", "SDL.NetworkUsername", "");
    config->addOption('w', "pass", "SDL.NetworkPassword", "");
    config->addOption('k', "netkey", "SDL.NetworkGameKey", "");
    config->addOption("port", "SDL.NetworkPort", 4046);
    config->addOption("players", "SDL.NetworkPlayers", 1);
     
    // input configuration options
    config->addOption("input1", "SDL.Input.0", "GamePad.0");
    config->addOption("input2", "SDL.Input.1", "GamePad.1");
    config->addOption("input3", "SDL.Input.2", "Gamepad.2");
    config->addOption("input4", "SDL.Input.3", "Gamepad.3");

    // allow for input configuration
    config->addOption('i', "inputcfg", "SDL.InputCfg", InputCfg);
    
    // display input
    config->addOption("inputdisplay", "SDL.InputDisplay", 0);
    
    // pause movie playback at frame x
    config->addOption("pauseframe", "SDL.PauseFrame", 0);
    config->addOption("moviemsg", "SDL.MovieMsg", 1);
    
    // overwrite the config file?
    config->addOption("no-config", "SDL.NoConfig", 0);
    
    // video playback
    config->addOption("playmov", "SDL.Movie", "");
    config->addOption("subtitles", "SDL.SubtitleDisplay", 1);
	
	config->addOption("fourscore", "SDL.FourScore", 0);
    
    #ifdef _S9XLUA_H
    // load lua script
    config->addOption("loadlua", "SDL.LuaScript", "");
    #endif
    
    #ifdef CREATE_AVI
    config->addOption("videolog",  "SDL.VideoLog",  "");
    config->addOption("mute", "SDL.MuteCapture", 0);
    #endif
    
    
    
    #ifdef _GTK
    char* home_dir = getenv("HOME");
    // prefixed with _ because they are internal (not cli options)
    config->addOption("_lastopenfile", "SDL.LastOpenFile", home_dir);
    config->addOption("_laststatefrom", "SDL.LastLoadStateFrom", home_dir);
    config->addOption("_lastopennsf", "SDL.LastOpenNSF", home_dir);
    config->addOption("_lastsavestateas", "SDL.LastSaveStateAs", home_dir);
	// option to disable gui (broken??)
	config->addOption("nogui", "SDL.NoGUI", 0);
    #endif
    
    // fcm -> fm2 conversion
    config->addOption("fcmconvert", "SDL.FCMConvert", "");
    
    // fm2 -> srt conversion
    config->addOption("ripsubs", "SDL.RipSubs", "");
	
	// enable new PPU core
	config->addOption("newppu", "SDL.NewPPU", 0);
	

    // GamePad 0 - 3
    for(unsigned int i = 0; i < GAMEPAD_NUM_DEVICES; i++) {
        char buf[64];
        snprintf(buf, 20, "SDL.Input.GamePad.%d.", i);
        prefix = buf;

        config->addOption(prefix + "DeviceType", DefaultGamePadDevice[i]);
        config->addOption(prefix + "DeviceNum",  0);
        for(unsigned int j = 0; j < GAMEPAD_NUM_BUTTONS; j++) {
            config->addOption(prefix + GamePadNames[j], DefaultGamePad[i][j]);
        }
    }
    
    // PowerPad 0 - 1
    for(unsigned int i = 0; i < POWERPAD_NUM_DEVICES; i++) {
        char buf[64];
        snprintf(buf, 20, "SDL.Input.PowerPad.%d.", i);
        prefix = buf;

        config->addOption(prefix + "DeviceType", DefaultPowerPadDevice[i]);
        config->addOption(prefix + "DeviceNum",  0);
        for(unsigned int j = 0; j < POWERPAD_NUM_BUTTONS; j++) {
            config->addOption(prefix +PowerPadNames[j], DefaultPowerPad[i][j]);
        }
    }

    // QuizKing
    prefix = "SDL.Input.QuizKing.";
    config->addOption(prefix + "DeviceType", DefaultQuizKingDevice);
    config->addOption(prefix + "DeviceNum", 0);
    for(unsigned int j = 0; j < QUIZKING_NUM_BUTTONS; j++) {
        config->addOption(prefix + QuizKingNames[j], DefaultQuizKing[j]);
    }

    // HyperShot
    prefix = "SDL.Input.HyperShot.";
    config->addOption(prefix + "DeviceType", DefaultHyperShotDevice);
    config->addOption(prefix + "DeviceNum", 0);
    for(unsigned int j = 0; j < HYPERSHOT_NUM_BUTTONS; j++) {
        config->addOption(prefix + HyperShotNames[j], DefaultHyperShot[j]);
    }

    // Mahjong
    prefix = "SDL.Input.Mahjong.";
    config->addOption(prefix + "DeviceType", DefaultMahjongDevice);
    config->addOption(prefix + "DeviceNum", 0);
    for(unsigned int j = 0; j < MAHJONG_NUM_BUTTONS; j++) {
        config->addOption(prefix + MahjongNames[j], DefaultMahjong[j]);
    }

    // TopRider
    prefix = "SDL.Input.TopRider.";
    config->addOption(prefix + "DeviceType", DefaultTopRiderDevice);
    config->addOption(prefix + "DeviceNum", 0);
    for(unsigned int j = 0; j < TOPRIDER_NUM_BUTTONS; j++) {
        config->addOption(prefix + TopRiderNames[j], DefaultTopRider[j]);
    }

    // FTrainer
    prefix = "SDL.Input.FTrainer.";
    config->addOption(prefix + "DeviceType", DefaultFTrainerDevice);
    config->addOption(prefix + "DeviceNum", 0);
    for(unsigned int j = 0; j < FTRAINER_NUM_BUTTONS; j++) {
        config->addOption(prefix + FTrainerNames[j], DefaultFTrainer[j]);
    }

    // FamilyKeyBoard
    prefix = "SDL.Input.FamilyKeyBoard.";
    config->addOption(prefix + "DeviceType", DefaultFamilyKeyBoardDevice);
    config->addOption(prefix + "DeviceNum", 0);
    for(unsigned int j = 0; j < FAMILYKEYBOARD_NUM_BUTTONS; j++) {
        config->addOption(prefix + FamilyKeyBoardNames[j],
                          DefaultFamilyKeyBoard[j]);
    }

    // for FAMICOM microphone in pad 2 pad 1 didn't have it
    // Takeshi no Chousenjou uses it for example.
    prefix = "SDL.Input.FamicomPad2.";
    config->addOption("rp2mic", prefix + "EnableMic", 0);
    

    const int Hotkeys[HK_MAX] = {
		SDLK_F1, // cheat menu
		SDLK_F2, // bind state
		SDLK_F3, // load lua
		SDLK_F4, // toggleBG
		SDLK_F5, // save state
		SDLK_F6, // fds select
		SDLK_F7, // load state
		SDLK_F8, // fds eject
		SDLK_F6, // VS insert coin
		SDLK_F8, // VS toggle dipswitch
		SDLK_PERIOD, // toggle frame display
		SDLK_F10, // toggle subtitle
		SDLK_F11, // reset
		SDLK_F12, // screenshot
		SDLK_PAUSE, // pause
		SDLK_MINUS, // speed++
		SDLK_EQUALS, // speed--
		SDLK_BACKSLASH, //frame advnace
		SDLK_TAB, // turbo
		SDLK_COMMA, // toggle input display
		SDLK_q, // toggle movie RW
		SDLK_QUOTE, // toggle mute capture
		//SDLK_ESCAPE, // quit
		SDLK_DELETE, // frame advance lag skip
		SDLK_SLASH, // lag counter display
		SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5,
		SDLK_6, SDLK_7, SDLK_8, SDLK_9, SDLK_EXCLAIM}; 
	
	prefix = "SDL.Hotkeys.";	
    for(int i=0; i < HK_MAX; i++)
		config->addOption(prefix + HotkeyStrings[i], Hotkeys[i]);
		
  /*
    config->addOption(prefix + "Pause", SDLK_PAUSE);
    config->addOption(prefix + "DecreaseSpeed", SDLK_MINUS);
    config->addOption(prefix + "IncreaseSpeed", SDLK_EQUALS);
    config->addOption(prefix + "FrameAdvance", SDLK_BACKSLASH);
    config->addOption(prefix + "FastForward", SDLK_TAB);
    config->addOption(prefix + "InputDisplay", SDLK_i);
    config->addOption(prefix + "MovieToggleReadWrite", SDLK_q);
    #ifdef CREATE_AVI
    config->addOption(prefix + "MuteCapture", SDLK_DELETE);
    #endif
    config->addOption(prefix + "Quit", SDLK_ESCAPE);
    //config->addOption(prefix + "Power", 0);
    
    
    
    
	config->addOption(prefix + "SelectState0", SDLK_0);
	config->addOption(prefix + "SelectState1", SDLK_1);
	config->addOption(prefix + "SelectState2", SDLK_2);
	config->addOption(prefix + "SelectState3", SDLK_3);
	config->addOption(prefix + "SelectState4", SDLK_4);
	config->addOption(prefix + "SelectState5", SDLK_5);
	config->addOption(prefix + "SelectState6", SDLK_6);
	config->addOption(prefix + "SelectState7", SDLK_7);
	config->addOption(prefix + "SelectState8", SDLK_8);
	config->addOption(prefix + "SelectState9", SDLK_9);
	config->addOption(prefix + "RomNavigator", SDLK_EXCLAIM);
	
    */

    // All mouse devices
    config->addOption("SDL.OekaKids.0.DeviceType", "Mouse");
    config->addOption("SDL.OekaKids.0.DeviceNum", 0);

    config->addOption("SDL.Arkanoid.0.DeviceType", "Mouse");
    config->addOption("SDL.Arkanoid.0.DeviceNum", 0);

    config->addOption("SDL.Shadow.0.DeviceType", "Mouse");
    config->addOption("SDL.Shadow.0.DeviceNum", 0);

    config->addOption("SDL.Zapper.0.DeviceType", "Mouse");
    config->addOption("SDL.Zapper.0.DeviceNum", 0);

    return config;
}

void
UpdateEMUCore(Config *config)
{
    int ntsccol, ntsctint, ntschue, flag, start, end;
    std::string cpalette;

    config->getOption("SDL.NTSCpalette", &ntsccol);
    config->getOption("SDL.Tint", &ntsctint);
    config->getOption("SDL.Hue", &ntschue);
    FCEUI_SetNTSCTH(ntsccol, ntsctint, ntschue);

    config->getOption("SDL.Palette", &cpalette);
    if(cpalette.size()) {
        LoadCPalette(cpalette);
    }

    config->getOption("SDL.PAL", &flag);
    FCEUI_SetVidSystem(flag ? 1 : 0);

    config->getOption("SDL.GameGenie", &flag);
    FCEUI_SetGameGenie(flag ? 1 : 0);

    config->getOption("SDL.Sound.LowPass", &flag);
    FCEUI_SetLowPass(flag ? 1 : 0);

    config->getOption("SDL.DisableSpriteLimit", &flag);
    FCEUI_DisableSpriteLimitation(flag ? 1 : 0);

    //Not used anymore.
    //config->getOption("SDL.SnapName", &flag);
    //FCEUI_SetSnapName(flag ? true : false);

    config->getOption("SDL.ScanLineStart", &start);
    config->getOption("SDL.ScanLineEnd", &end);

#if DOING_SCANLINE_CHECKS
    for(int i = 0; i < 2; x++) {
        if(srendlinev[x]<0 || srendlinev[x]>239) srendlinev[x]=0;
        if(erendlinev[x]<srendlinev[x] || erendlinev[x]>239) erendlinev[x]=239;
    }
#endif

    FCEUI_SetRenderedLines(start + 8, end - 8, start, end);
}
