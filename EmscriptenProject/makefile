CC=emcc

CPPSOURCES := $(wildcard *.cpp)
CPPSOURCES += $(wildcard ../Core/*.cpp)
CPPSOURCES += $(wildcard ../PlatformAbstraction/*.cpp)
CPPSOURCES += $(wildcard ../Miscellaneous/*.cpp)
CPPSOURCES += $(wildcard ../UnitTests/*.cpp)
CPPSOURCES := $(filter-out $(wildcard ../PlatformAbstraction/Win_*.cpp), $(CPPSOURCES))
CPPSOURCES := $(filter-out $(wildcard ../Miscellaneous/Win_*.cpp), $(CPPSOURCES))

INC1 = ../Core
INC2 = ../PlatformAbstraction
INC3 = ../Miscellaneous

INC = $(INC1) $(INC2) $(INC3)
INC_PARAMS = $(foreach d, $(INC), -I$d)

COMPILER_OPTS = -std=c++1z -stdlib=libstdc++ $(INC_PARAMS)

#COMPILER_OPTIMIZATION = -O3 -fno-signed-zeros -freciprocal-math -ffp-contract=fast -ffast-math -fno-stack-protector -D_FORTIFY_SOURCE=0 -fno-sanitize=memory -fno-rtti
#COMPILER_OPTIMIZATION = -O0
COMPILER_OPTIMIZATION = -Os -fno-signed-zeros -freciprocal-math -ffp-contract=fast -ffast-math -fno-stack-protector -D_FORTIFY_SOURCE=0 -fno-sanitize=memory -fno-rtti
# there's hardly any difference between Os and Oz in terms of size (something like 0.01%), but there might be much more noticeable consequences to the performance, so we avoid Oz
#COMPILER_OPTIMIZATION = -Oz -fno-signed-zeros -freciprocal-math -ffp-contract=fast -ffast-math -fno-stack-protector -D_FORTIFY_SOURCE=0 -fno-sanitize=memory -fno-rtti

COMPILER_OPTIMIZATION += -fno-exceptions

#COMPILER_WARNING_SETTINGS = -Wno-inconsistent-missing-override

COMPILER_PARAMS := $(COMPILER_OPTS) $(COMPILER_OPTIMIZATION) $(COMPILER_WARNING_SETTINGS)

EMSCRIPTEN_PARAMS := -s NO_EXIT_RUNTIME=1 -s DISABLE_EXCEPTION_CATCHING=1 -DPLATFORM_EMSCRIPTEN -DEMSCRIPTEN_HAS_UNBOUND_TYPE_NAMES=0 -s ELIMINATE_DUPLICATE_FUNCTIONS=1
#EMSCRIPTEN_PARAMS += --closure 1
#EMSCRIPTEN_PARAMS += -s RESERVED_FUNCTION_POINTERS=50
#EMSCRIPTEN_PARAMS += --memory-init-file 0
#EMSCRIPTEN_PARAMS += -s USE_ZLIB=1 #-s USE_LIBPNG=1 #-s USE_PTHREADS=1
#EMSCRIPTEN_PARAMS += -DDEBUG -D_DEBUG
EMSCRIPTEN_PARAMS += -DNDEBUG -D_NDEBUG
#EMSCRIPTEN_PARAMS += -s ASSERTIONS=1 -s DEMANGLE_SUPPORT=1 -s GL_ASSERTIONS=1 -g -s SAFE_HEAP=1
EMSCRIPTEN_PARAMS += -s FULL_ES2=1 # this flag is desirable, but there might be bugs in Emscripten, so use carefully
EMSCRIPTEN_PARAMS += -s WASM=1

# 0 - No LLVM LTO (default)
# 1 - LLVM LTO is performed
# 2 - Combine all the bitcode and run LLVM opt on it using the specified --llvm-opts. This optimizes across modules, but is not the same as normal LTO
# 3 - Does level 2 and then level 1
# even level 3 with -Os doesn't affect size noticeably compared to level 0
EMSCRIPTEN_PARAMS += --llvm-lto 3

#EMSCRIPTEN_PARAMS += -s ALLOW_MEMORY_GROWTH=1
#EMSCRIPTEN_PARAMS += -s TOTAL_MEMORY=33554432 # 32 MB
#EMSCRIPTEN_PARAMS += -s TOTAL_MEMORY=67108864 # 64 MB
#EMSCRIPTEN_PARAMS += -s TOTAL_MEMORY=134217728 # 128 MB
#EMSCRIPTEN_PARAMS += -s TOTAL_MEMORY=268435456 # 256 MB
EMSCRIPTEN_PARAMS += -s TOTAL_MEMORY=536870912 # 512 MB
#EMSCRIPTEN_PARAMS += -s TOTAL_MEMORY=1073741824 # 1024 MB

EXPORTS_FILE=makefile_exports.txt
OUTPUT=stdlib.js

all: $(CPPSOURCES) $(OUTPUT)

$(OUTPUT): $(CPPSOURCES)
	$(CC) $(CPPSOURCES) --bind $(EMSCRIPTEN_PARAMS) -s EXPORTED_FUNCTIONS=@$(EXPORTS_FILE) -s 'EXTRA_EXPORTED_RUNTIME_METHODS=["ccall", "cwrap"]' --emrun $(COMPILER_PARAMS) -o $(OUTPUT) #--preload-file ../StockpileWeb
	
clean:
	rm $(OUTPUT)
	rm $(OUTPUT).map