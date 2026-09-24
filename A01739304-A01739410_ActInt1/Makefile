# Actividad Integradora 1
#   make visual   -> compila visual/build/algoritmos y levanta el servidor local
#   make clean    -> borra lo compilado

CXX      ?= g++
CXXFLAGS ?= -std=c++11 -O3 -Wall -Wextra -Wno-sign-compare
PYTHON   ?= python3
# Carpeta de datos que anima la visualización: make visual TEST=tests/test1e2/
TEST     ?= tests/test1/

CORE    := SuffixArray.cpp SaIs.cpp Manacher.cpp
HEADERS := SuffixArray.hpp SaIs.hpp Manacher.hpp \
           visual/SuffixArrayState.hpp visual/ManacherState.hpp
SESSION := $(wildcard visual/session/*.cpp)

visual/build/algoritmos: $(SESSION) $(CORE) $(HEADERS)
	@mkdir -p visual/build
	$(CXX) $(CXXFLAGS) $(SESSION) $(CORE) -o $@

visual: visual/build/algoritmos
	$(PYTHON) visual/server.py --exe visual/build/algoritmos --test $(TEST)

clean:
	rm -rf x visual/build

.PHONY: visual clean
