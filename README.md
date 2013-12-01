# QkDaemon

### Dependencies

* Qt 5.1
* qkcore
* qkapi

### Build Instructions


Open the *.pro* file with QtCreator. Create *build configuration* for *debug* and *release*.
Go to *Projects* tab (on the left pane) and disable *Shadow build* for every build configuration. Then, select *Run Settings*. Here, on *Run Environment* you need to specify the path to *qkcore* and *qkapi* libraries 

**Linux**:
	


**Windows**

If you are using MinGW, the *PATH* environment variable should has at least the following paths:

	PATH = C:\Qt\Qt5.1.0\5.1.0\mingw48_32\lib;C:\Qt\Qt5.1.0\5.1.0\mingw48_32\bin;C:\Users\qkthings\qkthings\engineering\software\qkcore\release;C:\Users\qkthings\qkthings\engineering\software\qkapi\qt\qkapi\release
