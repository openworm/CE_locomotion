# Compatibility target for existing source-tree workflows. The installable
# package is built by the top-level CMake project and contains only main_osc.

.PHONY: all main_osc clean

all: main_osc

main_osc:
	cmake -S .. -B ../build
	cmake --build ../build --target main_osc
	cp ../build/main_osc ./main_osc

clean:
	$(RM) ./main_osc
	@if [ -d ../build ]; then cmake --build ../build --target clean; fi
