## QT-ClothesCAD

This repository contains two folders: 1. release 2. yzh3551.

In the 'release' folder, you will find the executable files for the release version. However, upon double-clicking to run the program, you might encounter a system error stating that "Qt5Svg.dll" could not be found. This error indicates that the environment required to run the executable is missing. To resolve this, you have two options:

Use the command line to provide the environment variable to the executable. The program requires DLL dynamic link libraries, so you should add the program's installation directory (e.g., X:\Qt5.0.2\bin) to your system's Path environment variable.

Locate the folder on your computer where Qt5Svg.dll is saved and copy the executable file into that folder. You should then be able to run the program.

I have provided my version of Qt5Svg.dll. If the application fails to start with the error 0xc000007b, you can try the following:

Click on the Start menu in the bottom left corner of your computer screen.
Enter "cmd.exe" in the search bar and press Enter to open the Command Prompt with administrative privileges.
In the Command Prompt, type "sfc /scannow" and press Enter. After the scan completes, the issue should be resolved.
The 'yzh3551' folder contains the source code for the program. You can also run the program directly from Qt5 by opening the "yzh3551.pro" file located in this folder. If you encounter any issues with compiling the program, you might need to set the build output path to match the directory structure on your computer. Alternatively, you can copy my program to the location on your computer where the build outputs are generated.

Please note that if you are using Visual Studio, the file extension for project files is .sln, whereas .pro is the file extension used by QT Creator. The 'icons' folder contains various icons used to enhance the appearance of the application, and these icons can be downloaded from the internet. The required files for the project include .h, .cpp, .ui, and .qss files, where .qss files are used for interface rendering.

You may need to rebuild all files before compiling and running the program. If your computer environment and QT environment are significantly different from mine, I have included screenshots of the entire required process in the report.
