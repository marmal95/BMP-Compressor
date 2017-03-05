This application uses SDL2 library, therefore to be able to compile it from source code you have to include this library properly into your project. On linux it is simple, just install `libsdl2-dev` using package manager.
On windows (e.g. when using Visual Studio 2015 or QtCreator), you have to compile SDL2 library by yourself from source code to make it work. <br /> <br />
SDL2: https://www.libsdl.org/download-2.0.php <br />
Guide for VS 2015: http://headerphile.com/sdl2/sdl2-part-0-setting-up-visual-studio-for-sdl2/

1. Usage:

	* `[-input] <...files> -output <pattern> [options]` creates encoded/decoded output from input files <br />
		*Remarks*: String `<pattern>` is generated output file name (full path), which can contain these parameters:

		- `%input.path%`    [unix] path of file (if not null, ends with '/')
		- `%input.name%`    name of input file (without extension)
		- `%input.ext%`     extension of input file (without dot)
		- `%input.id%`      position in input files set (0...N-1)
	
		*Remarks*: '.rgb12' is automatically added to output file name, when `<pattern>` doesn't contain extension.

	* `[-input] <...files> [(-s | --show)]`              Preview image files (.bmp | .rgb12) <br />
		*Remarks*: Can drag and drop image files on application to execute this command.

	* `(-a | --authors)`                                 Informations about authors
	* `(-v | --version)`                                 Application version
	* `(-h | --help)`                                    This message

	* **Options**:

		- (-s | --show)            show output file afterwards
		- (-gs | --grayscale)      convert image to grayscale (even if it is already in grayscale!)
		- (--huffman | --lz77)     use another compression algorithm (default = 12 bits per pixel)

2. Project directory tree structure

	* /bin  - all compiled executables
	* /build - compiled binary build files
	* /include - c++ header files
	* /src - c++ source files
	* /test - main.cpp and other files used for tests
	* /doc - documentation (in polish)

3. Makefile <br />
	Edit makefile to change flags for compilation debug and release versions
	* `make` compiles application binary `bin/app`
	* `make test` fresh compile test application binary `bin/test`
	* `make clean` removes all files from `bin/` and `build/`
