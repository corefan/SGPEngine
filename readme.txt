Welcome the SGP Engine SDK.  
by phoenixzz


==========================================================================
 Directory Structure Overview
==========================================================================


==========================================================================
 Requirements
==========================================================================
  You can use one of the following compilers/IDEs to develop applications
  with SGP Engine or recompile the engine. However, other compilers/IDEs may
  work as well, we simply didn't test them.

  * gcc 4.x
  * Visual Studio 2008(9.0)-2010(10.0)
  * Code::Blocks (& gcc or visual studio toolkit)

  If you ever want to (re)compile the engine yourself (which means you don't
  want to use the precompiled version) you need the following:

  * Windows:
	* Needed: PlatformSDK (which usually comes with all IDEs, download
			it separately for MSVC Express 2008-2010)
	* Optional: OPENGL headers and libraries


  * iOS:
	* Needed: OpenGL headers and libraries

  * MacOSX:
	* Needed: OpenGL headers and libraries

  * Android:



==========================================================================
 Version History of the SGP Engine SDK
==========================================================================


----------------------------------------------------------
2014/12/18
----------------------------------------------------------
* Added OpenAL Sound Lib support

+ Added the ability to play WAV / OGG sound
+ Added the ability to play sound in single thread
+ Added the ability to play sound with Multi Thread (sound file loading / deleting in Multi-Thread)

