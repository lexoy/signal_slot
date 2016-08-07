debug_options = "disable_debug"

source = main.cpp
target = main
compiler = g++
options = -std=c++1y
ifeq ($(debug_options), "enable_debug")
	debug_options = -g -O0 -Wall
endif
ifeq ($(debug_options), "disable_debug")
	debug_options = -O3
endif

$(target) : $(source)
	$(compiler) $^ -o$@ $(options) $(debug_options)


clean:
	rm -vrf $(target)