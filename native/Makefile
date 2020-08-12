CC=g++
TARGET=libImageMatching.dylib
CFLAGS=--std=c++11 #`pkg-config opencv4 --cflags`
INC=-I $(JAVA_HOME)/include -I $(JAVA_HOME)/include/darwin -I /usr/local/include/opencv4
LDLIBS=-lpthread -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_highgui
OBJS=match_image.o match_image_jni.o

$(TARGET): $(OBJS)
	$(CC) -shared -o $@ $(OBJS) $(CFLAGS) $(LDLIBS)	
	rm -f *.o
	mv $(TARGET) ../

match_image.o: match_image.h match_image.cpp
	$(CC) -fPIC -c match_image.cpp $(INC) $(CFLAGS) $(LDLIBS)

match_image_jni.o: match_image_jni.h match_image_jni.cpp
	$(CC) -fPIC -c match_image_jni.cpp $(INC) $(CFLAGS)

clean:
	rm -f $(TARGET)
	rm -f result.png