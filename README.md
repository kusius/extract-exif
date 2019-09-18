# Extract Exif

A simple program to extract EXIF header information from jpeg images and create an output data file that can be directly imported to MatLab. Uses the single-header [easyexif](https://github.com/mayanklahiri/easyexif) C API.


## Compilation Win

Comes with a windows visual studio 2019 solution that can be used to compile and create executables.

Alternatively: 
cl -nologo  -D_CRT_SECURE_NO_WARNINGS -FC -Zi  path\to\extract-exif.cpp path\to\exif.cpp

# Compilation Linux 
compile extract-exif.cpp and exif.cpp....

#Compilation with C++ Experimental
use compiler flag CPP_EXPERIMENTAL and compile extract-exif.cpp and exif.cpp

## Usage
Windows version: extract-exif [files]
	*files*: a sring that specifies the (relative/absolute) path to a jpeg file.
The *files* string can contain tokens like "*.jpg" to process all jpg files in a directory 

Linux Version: Not tested, assumed that it's not working. Recommended alternative for linux users is to compile with the CPP_EXPERIMENTAL flag.

C++ Experimental: extract-exif [folder]
	*folder*: a string that specifies the (relative/absolute) path to a folder.
	All files in that folder will be processed. If some  of them are not jpeg files, they are ignored.
		   

(not yet implemented)
demo.cpp and exif.cpp are easyexif example code programs.

## Sample data 
The sample data folder "test-images" is taken from the easyexif repository.