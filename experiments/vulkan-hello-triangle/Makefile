CXX := g++

CFLAGS := -std=c++17 -O2
LDFLAGS := -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

vulkan-hello-triangle: main.cpp
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS)
