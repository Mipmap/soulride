/*
    Copyright 2000, 2001, 2002, 2003 Slingshot Game Technology, Inc.

    This file is part of The Soul Ride Engine, see http://soulride.com

    The Soul Ride Engine is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2 of
    the License, or (at your option) any later version.

    The Soul Ride Engine is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

// macosxmain.cpp      - Bjorn Leffler 31/8/2003
// after linuxmain.cpp by thatcher 1/26/2001 Copyright Thatcher Ulrich 

// Main program for MacOSX version of Soul Ride.

#include <SDL.h>
#include <SDL_Main.h>

#ifdef MACOSX
#include <OpenGL/gl.h>
#include <iostream>
#include <sys/stat.h>
#include "sound.hpp"
#else // not MACOSX (X-windows interface)
#include <X11/X.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <GL/gl.h>
#endif // end not MACOSX

#include <unistd.h>
#include <string>
#include <new>

#include "main.hpp"
#include "macosxmain.hpp"
#include "gameloop.hpp"
#include "error.hpp"
#include "input.hpp"
#include "timer.hpp"
#include "render.hpp"

// This is used for the GUI part of MacOSX 
// (option window before starting opengl)
#ifdef MACOSX_CARBON
#include <Carbon/Carbon.h>
#include "config.hpp" // used to set default mountain

#define kSRApplicationSignature		'BjSR'
#define kSRStart			'STRT'
#define kSRDisplayError		        'ERR1'
#define kSRDefaultMountainPopUp		1	
#define kSRScreenResolutionPopUp	2
#define kSRScreenDepthPopUp		3
#define kSRFullScreenCheckbox		4

#define MACOSX_WINDOW_PROBLEM -1

int argC;
char** argV;

static WindowRef window;
static WindowRef windowErrorDisplay;

// definded using interface builder, file main.nib
static int screenWidth[]  = {0, 320, 640, 800, 1024, 1152, 1280, 1600};
static int screenHeight[] = {0, 240, 480, 600,  768,  864, 1024, 1200};
static int screenDepth[]  = {0, 8, 16, 24};

// This should be automatic later
static char *mountains[]  = {"", "testing", "Jay_Peak", "Stratton", "Breckenridge"};

static int defaultMountain = 1;
static int screenResolution = 2;
static int depth = 2;
static int fullscreen = 1;


OSStatus SoulRideOptionWindowEventHandler (EventHandlerCallRef myHandler, 
                                                  EventRef event,
                                                  void *userData)
{
    OSStatus 	result = eventNotHandledErr;
    HICommand	command;

    int main2result;

    GetEventParameter (event, kEventParamDirectObject, typeHICommand, NULL, 
                       sizeof (HICommand), NULL, &command);
    switch (command.commandID)
      {
      case kSRStart:
	getWindowOptions();
	result = noErr;
	HideWindow(window);
	main2result = main2(argC, argV);
	if (main2result == MACOSX_WINDOW_PROBLEM){
	  SDL_Quit();
	  ShowWindow(windowErrorDisplay);
	}
	else
	  exit(0);
	break;
      case kSRDisplayError:
	HideWindow(windowErrorDisplay);
	ShowWindow(window);
	std::cout << "kSRDisplayError\n" << std::endl;
	break;
      case 'quit':
	exit(0);
	break;
	//default:
	//printf("Command = %d\n", command.commandID);
      }
    return result;
}

int main(int argc, char** argv)
{
  argC = argc;
  argV = argv;


    IBNibRef 		nibRef;
    //WindowRef 		window;
    
    OSStatus		err;

    EventTypeSpec	mainSpec = {kEventClassCommand, kEventCommandProcess};

    // Create a Nib reference passing the name of the nib file (without the .nib extension)
    // CreateNibReference only searches into the application bundle.
    err = CreateNibReference(CFSTR("main"), &nibRef);
    require_noerr( err, CantGetNibRef );
    
    // Once the nib reference is created, set the menu bar. "MainMenu" is the name of the menu bar
    // object. This name is set in InterfaceBuilder when the nib is created.
    err = SetMenuBarFromNib(nibRef, CFSTR("MenuBar"));
    require_noerr( err, CantSetMenuBar );
    
    // Then create a window. "MainWindow" is the name of the window object. This name is set in 
    // InterfaceBuilder when the nib is created.
    err = CreateWindowFromNib(nibRef, CFSTR("MainWindow"), &window);
    require_noerr( err, CantCreateWindow );

    // Create display error window
    err = CreateWindowFromNib(nibRef, CFSTR("ErrorDisplay"), &windowErrorDisplay);
    require_noerr( err, CantCreateWindow );

    // We don't need the nib reference anymore.
    DisposeNibReference(nibRef);
        
    // Install event handlers
    err =  InstallWindowEventHandler (window, NewEventHandlerUPP (SoulRideOptionWindowEventHandler),
                                      1, &mainSpec, 
                                      (void *) window, NULL);
    err =  InstallWindowEventHandler (windowErrorDisplay, NewEventHandlerUPP (SoulRideOptionWindowEventHandler),
                                      1, &mainSpec, 
                                      (void *) window, NULL);
    
    // The window was created hidden so show it.
    ShowWindow( window );
    
    // Call the event loop
    RunApplicationEventLoop();

CantCreateWindow:
CantSetMenuBar:
CantGetNibRef:
	return err;
}

void getWindowOptions ()
{

    ControlHandle 	defaultMountainControlHandle;
    ControlHandle	screenResolutionTimeControlHandle;
    ControlHandle	colourDepthControlHandle;
    ControlHandle	fullscreenControlHandle;

    ControlID		defaultMountainControlID = { kSRApplicationSignature, kSRDefaultMountainPopUp };
    ControlID		screenResolutionTimeControlID = { kSRApplicationSignature, kSRScreenResolutionPopUp };
    ControlID		screenDepthControlID = { kSRApplicationSignature, kSRScreenDepthPopUp };
    ControlID		fullscreenControlID = { kSRApplicationSignature, kSRFullScreenCheckbox };

    GetControlByID (window, &defaultMountainControlID, &defaultMountainControlHandle);
    GetControlByID (window, &screenResolutionTimeControlID, &screenResolutionTimeControlHandle);
    GetControlByID (window, &screenDepthControlID, &colourDepthControlHandle);
    GetControlByID (window, &fullscreenControlID, &fullscreenControlHandle);
    
    
    defaultMountain = 	GetControl32BitValue (defaultMountainControlHandle);
    screenResolution = 	GetControl32BitValue (screenResolutionTimeControlHandle);
    depth = 		GetControl32BitValue (colourDepthControlHandle);
    fullscreen = 	GetControl32BitValue (fullscreenControlHandle);
     
    printf("DefaultMountain = %s\nResolution = %dx%d, %d bpp, fullscreen: ",
           mountains[defaultMountain],
           screenWidth[screenResolution],
           screenHeight[screenResolution],
           screenDepth[depth]);

    if (fullscreen)
      printf("yes\n");
    else 
      printf("no\n");

    Config::Open();
    Config::Set("DefaultMountain", mountains[defaultMountain]);
    Config::SetInt("OGLDriverIndex", 0);
    Render::SetWindowFullscreen(fullscreen);
    Render::SetWindowWidth(screenWidth[screenResolution]);
    Render::SetWindowHeight(screenHeight[screenResolution]);
    Render::SetWindowDepth(screenDepth[depth]);
        
}
#endif // MACOSX_CARBON


void	ProcessEvent(SDL_Event* event);


static float	MouseX = 0;
static float	MouseY = 0;
static float	MouseSpeedX = 0;
static float	MouseSpeedY = 0;
static int	LastMouseSampleTicks = 0;


static void	MeasureMouseSpeed()
// This function measures the mouse speed input, filters it, and passes
// the results to the Input:: module.
{
	int	ticks = Timer::GetTicks();
	int	dt = ticks - LastMouseSampleTicks;
	float	NewMouseX, NewMouseY;
	int	dummy;
	Input::GetMouseStatus(&NewMouseX, &NewMouseY, &dummy);
	if (dt > 0 &&
	    (dt > 50 || NewMouseX != MouseX || NewMouseY != MouseY))
	{
		float	c0 = exp(-(dt/1000.0)/0.20);
		float	msx = (NewMouseX - MouseX) * 1000.0 / float(dt);
		float	msy = (NewMouseY - MouseY) * 1000.0 / float(dt);
		MouseSpeedX = MouseSpeedX * c0 + msx * (1 - c0);
		MouseSpeedY = MouseSpeedY * c0 + msy * (1 - c0);
		Input::NotifyMouseSpeed(MouseSpeedX, MouseSpeedY);
		MouseX = NewMouseX;
		MouseY = NewMouseY;
		LastMouseSampleTicks = ticks;
	}
}

// MacOSX uses a different main to start for its GUI
// main2(...) is called at the end of the other main(...)

#ifndef MACOSX_CARBON
int	main(int argc, char** argv)
#else
int	main2(int argc, char** argv)
#endif // MACOSX_CARBON

// Main program entry point for Soul Ride.
{
	bool	CaughtError = false;
	char	ErrorMessage[1000];

	// Print the program title and version.
	printf("Soul Ride %s\n(c) Copyright 2003 Slingshot Game Technology\nhttp://soulride.com\n", Main::GetVersionString());

#ifdef MACOSX
	printf ("Mac OSX port by Bjorn Leffler \n");

	// if started from the Finder, change directory to Resources
        std::string path(argv[0]);
        std::cout << "Path = " << path << std::endl;
        int position = path.rfind("SoulRide.app/Contents/MacOS", path.length());
	//std::cout << "position = " << position << std::endl;
        if (position > 0){
	  //std::cout << "length of string  = " << path.length() << std::endl;
	  //std::cout << "position of MacOS = " << position << std::endl;
	  path.replace(position, 38, "SoulRide.app/Contents/Resources");
	  //std::cout << "Path to Resources = " << path << std::endl;
	  chdir(path.c_str());
	}
	else{
	  std::cerr << "Couldn't change to Resources directory," << std::endl;
	  std::cerr << "maybe I won't find the data directory." << std::endl;
	  std::cerr << "<hint> launch using ./SoulRide.app/Contents/MacOS/Soul_Ride" << std::endl;

	}
	
	// On MacOSX PlayerData files are saved in the users home directory:
	// $HOME/Documents/SoulRide/Playerdata
	// This is for SoulRide.app to be installed in a system-wide location
	// We now make sure these directories exist

	std::string dirname("");
	dirname += "/Documents/SoulRide/PlayerData";
	chdir(dirname.c_str());
	if (chdir(dirname.c_str()) != 0) {
	  std::string dirname("");
	  dirname += getenv("HOME");
	  dirname += "/Documents";
	  mkdir(dirname.c_str(), 0755);
	  dirname += "/SoulRide";
	  mkdir(dirname.c_str(), 0755);
	  dirname += "/PlayerData";
	  mkdir(dirname.c_str(), 0755);
	}

#endif



	// Init SDL.
	int	result;
	result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_CDROM | SDL_INIT_AUDIO);
	if (result == -1) {
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
	}

	// Enable Unicode translation.
        SDL_EnableUNICODE(1);

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	// Don't show the OS mouse cursor.  We draw our own when necessary.
	// Bjorn: annoying when debugging
	// SDL_ShowCursor(SDL_DISABLE);

	try {
		// Concatenate all the args into one big string.  It'll be re-parsed into Lua
		// assignments by Main::Open().
		int	cl_size = 0;
		int	i;
		for (i = 1; i < argc; i++) {
			cl_size += strlen(argv[i]) + 1;
		}
		char*	command_line = new char[cl_size];
		command_line[0] = 0;
		for (i = 1; i < argc; i++) {
			strcat(command_line, argv[i]);
			strcat(command_line, " ");
		}

		Main::Open(command_line);

		delete [] command_line;

		// Start up 3D and load the game data.
		GameLoop::Open();

		while (!Main::GetQuit()) {
			SDL_Event	event;
			if (SDL_PollEvent(&event)) {
				ProcessEvent(&event);

			// If the app is not paused and the game loop is open, then run an iteration of the game loop.
			} else if (Main::GetPaused() == false && GameLoop::GetIsOpen() == true) {
				// Compute mouse speed and notify Input::
				MeasureMouseSpeed();

				GameLoop::Update();
			} else {
				// Wait for an event.
				SDL_WaitEvent(NULL);
			}
		}
	}
	catch (std::bad_alloc) {
		printf("Out Of Memory.\n");
		exit(1);
	}
#ifdef MACOSX_CARBON
	// problem initializing window (maybe too large resolution) 
	catch (Render::RenderError& e) {
		printf("Caught RenderError: %s\n", e.GetMessage());
		return MACOSX_WINDOW_PROBLEM;
	}
#endif // MACOSX_CARBON
	catch (Error& e) {
//		CaughtError = true;
//		strcpy(ErrorMessage, e.GetMessage());
		printf("%s\n", e.GetMessage());
	}
//	catch (...) {
//		MessageBox(hWndMain, "Unknown exception caught.", "Soul Ride", MB_OK);
//	}

	// Close subsystems.
	try {
		// Clean up resources, post a quit message.
		if (GameLoop::GetIsOpen()) {
			GameLoop::Close();
		}

		Main::Close();
	}
	catch (Error& e) {
		if (!CaughtError) {
			CaughtError = true;
			strcpy(ErrorMessage, "While closing:\n");
			strcat(ErrorMessage, e.GetMessage());
		}
	}
//	catch (...) {
//		MessageBox(hWndMain, "Unknown exception caught in Main::Close().", "Soul Ride", MB_OK);
//	}

	// Show any error.
	if (CaughtError) {
		printf("%s", ErrorMessage);
	}

	SDL_Quit();

	return 0;
}


static void	ProcessKeyEvent(SDL_keysym* keysym, bool Down);


void	ProcessEvent(SDL_Event* event)
// Process the given SDL event.
{
	switch (event->type) {
	default:
		break;

	case SDL_KEYDOWN:
	case SDL_KEYUP:
	{
		SDL_KeyboardEvent*	key = &event->key;

		// key->keysym.scancode;
		// key->type == SDL_KEYDOWN or SDL_KEYUP
		// key->keysym.unicode == unicode char
		// key->keysym.mod == modifiers
		// key->keysym.sym == key symbol

		ProcessKeyEvent(&(key->keysym), key->type == SDL_KEYDOWN);

		if (key->type == SDL_KEYDOWN && key->keysym.unicode < 0x80 && key->keysym.unicode > 0) {
			// ASCII-ish key.  Pass it to the Input module.
			Input::NotifyAlphaKeyClick(key->keysym.unicode);
		}

		break;
	}

	case SDL_MOUSEMOTION:
	{
		SDL_MouseMotionEvent*	m = (SDL_MouseMotionEvent*) event;
		int	buttons = 0;
		if (SDL_BUTTON_LMASK & m->state) buttons |= 1;
		if (SDL_BUTTON_RMASK & m->state) buttons |= 2;
		if (SDL_BUTTON_MMASK & m->state) buttons |= 4;
		Input::NotifyMouseStatus(m->x, m->y, m->state);
		break;
	}

	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
	{
		SDL_MouseButtonEvent*	m = (SDL_MouseButtonEvent*) event;
		int	button = 0;
		if (event->type == SDL_MOUSEBUTTONDOWN) {
			if (m->button == SDL_BUTTON_LEFT) button = 1;
			else if (m->button == SDL_BUTTON_RIGHT) button = 2;
			else if (m->button == SDL_BUTTON_MIDDLE) button = 4;
		}
		Input::NotifyMouseStatus(m->x, m->y, button);
		break;
	}

	case SDL_QUIT:
		Main::SetQuit(true);
		break;
	}
}


static void	ProcessKeyEvent(SDL_keysym* keysym, bool Down)
// Looks at the key symbol and sends a notification to the Input module if
// the key matches one of the input buttons.
// Down should be true when the key goes down or when it auto-repeats.  Down
// should be false for key-up events.
{
	if (Main::GetQuit()) return;

	SDLKey	key = keysym->sym;

	// Control keys.
	if (key == SDLK_LCTRL || key == SDLK_RCTRL) {
		Input::NotifyControlKey(Down);
	}
	
	Input::ButtonID	id = Input::BUTTONCOUNT;

	switch (key) {
	default:
		break;
		
	case SDLK_LCTRL:
	case SDLK_RCTRL:
		id = Input::BUTTON0; break;
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		id = Input::BUTTON1; break;
	case SDLK_SPACE:
		id = Input::BUTTON2; break;
	case SDLK_RALT:
	case SDLK_LALT:
		id = Input::BUTTON3; break;
	// player 2
	case SDLK_LEFT:
	case SDLK_KP4:
		id = Input::LEFT1; break;
	case SDLK_RIGHT:
	case SDLK_KP6:
		id = Input::RIGHT1; break;
	case SDLK_UP:
	case SDLK_KP8:
		id = Input::UP1; break;
	case SDLK_DOWN:
	case SDLK_KP2:
		id = Input::DOWN1; break;
	// player 2
	case SDLK_s:
		id = Input::LEFT2; break;
	case SDLK_f:
		id = Input::RIGHT2; break;
	case SDLK_e:
		id = Input::UP2; break;
	case SDLK_d:
		id = Input::DOWN2; break;
	// player 3
	case SDLK_j:
		id = Input::LEFT3; break;
	case SDLK_l:
		id = Input::RIGHT3; break;
	case SDLK_i:
		id = Input::UP3; break;
	case SDLK_k:
		id = Input::DOWN3; break;
	// player 4
	case SDLK_v:
		id = Input::LEFT4; break;
	case SDLK_n:
		id = Input::RIGHT4; break;
	case SDLK_g:
		id = Input::UP4; break;
	case SDLK_b:
		id = Input::DOWN4; break;

	case SDLK_RETURN:
	case SDLK_KP_ENTER:
		id = Input::ENTER; break;
	case SDLK_ESCAPE: id = Input::ESCAPE; break;
	case SDLK_F1: id = Input::F1; break;
	case SDLK_F2: id = Input::F2; break;
	case SDLK_F3: id = Input::F3; break;
	case SDLK_F4: id = Input::F4; break;
	case SDLK_F5: id = Input::F5; break;
	case SDLK_F6: id = Input::F6; break;
	case SDLK_F7: id = Input::F7; break;
	case SDLK_F8: id = Input::F8; break;
	case SDLK_F9: id = Input::F9; break;
	case SDLK_F10: id = Input::F10; break;
	case SDLK_F11: id = Input::F11; break;
	case SDLK_F12: id = Input::F12; break;
	}

	if (id < Input::BUTTONCOUNT) {
		Input::NotifyKeyEvent(id, Down, Down);
	}

	// Translate basic editing keys into special character codes.
	if (Down) {
		int	c = 0;
		switch (key) {
		case SDLK_INSERT:
		case SDLK_KP0:
			c = 128; break;
				
		case SDLK_END:
		case SDLK_KP1:
			c = 129; break;
		case SDLK_DOWN:
		case SDLK_KP2:
			c = 130; break;
		case SDLK_PAGEDOWN:
		case SDLK_KP3:
			c = 131; break;	// pg dn

		case SDLK_LEFT:
		case SDLK_KP4:
			c = 132; break;
		case SDLK_RIGHT:
		case SDLK_KP6:
			c = 134; break;

		case SDLK_HOME:
		case SDLK_KP7:
			c = 135; break;
		case SDLK_UP:
		case SDLK_KP8:
			c = 136; break;
		case SDLK_PAGEUP:
		case SDLK_KP9:
			c = 137; break;	// pg up
				   
		case SDLK_DELETE:
		case SDLK_KP_PERIOD:
			c = 138; break;

		case SDLK_F1: c = 141; break;
		case SDLK_F2: c = 142; break;
		case SDLK_F3: c = 143; break;
		case SDLK_F4: c = 144; break;
		case SDLK_F5: c = 145; break;
		case SDLK_F6: c = 146; break;
		case SDLK_F7: c = 147; break;
		case SDLK_F8: c = 148; break;
		case SDLK_F9: c = 149; break;
		case SDLK_F10: c = 150; break;
		case SDLK_F11: c = 151; break;
		case SDLK_F12: c = 152; break;
		}

		if (c) Input::NotifyAlphaKeyClick(c);
	}
}


namespace Main {
;
//


void	CenterMouse()
// Center the mouse cursor.
{
	int	x = Render::GetWindowWidth() >> 1;
	int	y = Render::GetWindowHeight() >> 1;
	SDL_WarpMouse(x, y);

	// Update some mouse-speed measuring state to prevent it from
	// considering this synthetic move a real mouse motion.
	MouseX = x;
	MouseY = y;
}


}; // end namespace Main
