SGPEngine
==============

SGPEngine is a lightweight cross-platform modern open source game development toolkit. It features low and high level engine systems, OpenGL 3.3 and OpenGL ES 2.0 support, complete asset pipeline and many fully featured editors. 

This Engine aims to provide simple yet powerful environment for creating games and other graphical applications. A wide range of features are available, ranging from a math and utility library, to OpenGL/ES render systems all the way to asset processing, fully featured editor and sound supported.

For game developers, SGPEngine aims to provide a complete set of tools and APIs needed to make a game from start to finish. It provides a highly intuitive and customizable editor that is easy to use by artists, designers and programmers alike, supporting development stages from asset management, scene building, scripting to game publishing. It now includes Worldmap Editor, Model Editor, 3DMax9.0 export plugin and one Win32 OpenGLES Emulation.

Features (currently available)
--------

- Editor 
 - Asset management
     - 3DMax9 plugin import
     - Asset modifications immediately reflected in editor
     - Custom binary format
 - Level creation 
 	 - Simple drag and drop interface
	 - Traditional set of tools
	 - Objects modifications immediately reflected in editor
 - Game publishing 
	 - One click build process
 - Customizable frontend
	 - Dockable layout and floating windows with MFC
	 - Custom skin & localization support with MFC
- Core
 - Design
	 - Optimized fast 3D math and container template libraries
	 - Written in pure C++ and totally object oriented
	 - Clean layered design
	 - Modular & plugin based
	 - Minimal third-party dependencies
	 - Multiplatform ready
 - Renderer
	 - OpenGL 3.3 and OpenGL ES 2.0 render systems
     - Character animation system with skeletal animation
	 - SPARK Particle system
	 - Unicode text rendering
	 - Brand new and Flexible material system + Shader system
 - Asset pipeline 
	 - Easy to use
	 - Multi-threaded and asynchronous resource loading
	 - Support format: TGA DDS mf1(custom mesh) bf1(custom bone) map(custom world map) TTF fonts
 - Other 
	 - Virtual input
	 - Debug drawing
	 - VLD memory check
	 - Platform independent. Runs on Windows, OSX, iOS, and Android

Features (coming in soon)
--------
- DirectX 12/Vulkan render interface API support
- Physics system integration
- GUI system and script integration
- CPU & GPU profiler


Usage
-----
Aside from SGPEngine source code, you DON'T need any config and by default NON'T need any third party dependencies. 

You can use one of the following compilers/IDEs to develop applications with SGP Engine or recompile the engine. However, other compilers/IDEs may work as well, we simply didn't test them.

 - gcc 4.x
 - Visual Studio 2008(9.0)-2010(10.0)
 - Code::Blocks (& gcc or visual studio toolkit)

If you ever want to (re)compile the engine yourself (which means you don't want to use the precompiled version) you need the following:

 - Windows:
	- Needed: PlatformSDK (which usually comes with all IDEs, download it separately for MSVC Express 2008-2010)
	- Optional: OPENGL headers and libraries
 - iOS:
	- Needed: OpenGL headers and libraries
 - MacOSX:
	- Needed: OpenGL headers and libraries
 - Android:
	- Needed: OpenGL headers and libraries

Author
-----
SGPEngine was created and is developed by phoenixzz.

Contact me at e-mail(phoenixzz@sina.com) or add me on [LinkedIn](http://www.linkedin.com/profile/preview?locale=en_US&trk=prof-0-sb-preview-primary-button). 

Thanks
------
JUCE <https://www.juce.com/> 

SPARK Particle Engine <https://sourceforge.net/projects/sparkengine/>
