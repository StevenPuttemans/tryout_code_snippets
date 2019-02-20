# Configuration for facial feature tracking

Important to get the following settings right to achieve top speed

* Latest OpenCV3.2 release build
* Latest dlib release build
* Add the following dependencies in your project file
  * Enable C++11 instructions for your compiler
  * In your compiler options set explicitly `-mavx -msse2 -msse4` to use all possible optimizations
  * In your linker settings, add `/usr/local/lib/libdlib.so` and `-lcblas -llapack -lgif pkg-config opencv --libs`

