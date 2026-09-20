# ============================================================
# Makefile - Laboratorio de Memoria Virtual (C++)
# Compatible con Linux/macOS y Windows (MSYS2 / MinGW-w64)
# ============================================================

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Werror -Iinclude
SRCDIR   := src
OBJDIR   := build

# ---------------------------------------------------------
# Detección de sistema operativo
# En Windows, make/mingw32-make define la variable OS=Windows_NT
# ---------------------------------------------------------
ifeq ($(OS),Windows_NT)
    TARGET   := simulador.exe
    RM       := del /Q /F
    RMDIR    := rmdir /S /Q
    MKDIR    := if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
    FIXPATH  = $(subst /,\,$1)
    NULLDEV  := NUL
else
    TARGET   := simulador
    RM       := rm -f
    RMDIR    := rm -rf
    MKDIR    := mkdir -p $(1)
    FIXPATH  = $1
    NULLDEV  := /dev/null
endif

# ---------------------------------------------------------
# Fuentes y objetos (busca .cpp en todas las subcarpetas de src/)
# ---------------------------------------------------------
SOURCES := $(wildcard $(SRCDIR)/*.cpp) \
           $(wildcard $(SRCDIR)/domain/*.cpp) \
           $(wildcard $(SRCDIR)/application/*.cpp) \
           $(wildcard $(SRCDIR)/infrastructure/*.cpp)

OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

# ---------------------------------------------------------
# Targets principales
# ---------------------------------------------------------
.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)

# Regla genérica: compila cada .cpp preservando su subcarpeta dentro de build/
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@$(call MKDIR,$(dir $@))
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	@if exist "$(OBJDIR)" $(RMDIR) "$(OBJDIR)"
	@if exist "$(TARGET)" $(RM) "$(TARGET)"
else
	$(RMDIR) $(OBJDIR)
	$(RM) $(TARGET)
endif

# Uso: make run ARGS="tests/test1_basico.txt"
run: all
	./$(TARGET) $(ARGS)