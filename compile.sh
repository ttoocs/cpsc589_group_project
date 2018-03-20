LIBS="-lglfw -lpthread -lm -lXi -lXrandr -lX11 -lXxf86vm -lXinerama -lXcursor -lGLU -ldl -lOpenGL"

g++ $LIBS ./gl_helpers.cpp ./main.cpp ./shapes.cpp -Wall -g -I ./Eigen -w
