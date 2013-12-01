# QkDaemon

### Dependencies

* Qt 5.1
* qkcore
* qkapi

### Build Instructions


1. Open the *.pro* file with QtCreator
2. Create *build configuration* for *debug* and *release* (use the default values)
3. Go to *Projects* tab (on the left pane) and disable *Shadow build* for every build configuration 
4. Select *Run Settings*. Here, on *Run Environment* you need to specify the path to *qkcore* and *qkapi* libraries using the environment variable available on your operating system for that purpose. See the following example for Linux and Windows

**Linux**:
	
	LD_LIBRARY_PATH: /opt/Qt5.1.1/5.1.1/gcc/lib:/opt/Qt5.1.1/5.1.1/gcc/lib;/home/qkthings/software/qkcore/release;/home/qkthings/software/qkapi/release

**Windows**

If you are using MinGW, the *PATH* environment variable should has at least the following paths:

	PATH = C:\Qt\Qt5.1.0\5.1.0\mingw48_32\lib;C:\Qt\Qt5.1.0\5.1.0\mingw48_32\bin;C:\Users\qkthings\qkthings\engineering\software\qkcore\release;C:\Users\qkthings\qkthings\engineering\software\qkapi\qt\qkapi\release
