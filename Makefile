# ============================================================
# Makefile - Laboratorio de Memoria Virtual (C++)
# Compatible con Linux/macOS y Windows (MSYS2 / MinGW-w64)
# ============================================================

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Werror -Iinclude
SRCDIR   := src
OBJDIR   := build
TEST_DIR := tests/unit
BIN_DIR  := bin

# ---------------------------------------------------------
# Detección de sistema operativo
# En Windows, make/mingw32-make define la variable OS=Windows_NT
# ---------------------------------------------------------
ifeq ($(OS),Windows_NT)
    TARGET   := simulador.exe
    EXE_EXT  := .exe
    RM       := del /Q /F
    RMDIR    := rmdir /S /Q
    MKDIR    = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
    FIXPATH  = $(subst /,\,$1)
    NULLDEV  := NUL
else
    TARGET   := simulador
    EXE_EXT  :=
    RM       := rm -f
    RMDIR    := rm -rf
    MKDIR    = mkdir -p $(1)
    FIXPATH  = $1
    NULLDEV  := /dev/null
endif

# ---------------------------------------------------------
# Fuentes y objetos (busca .cpp en todas las subcarpetas de src/)
# ---------------------------------------------------------
SOURCES := $(wildcard $(SRCDIR)/*.cpp) \
           $(wildcard $(SRCDIR)/domain/*.cpp) \
           $(wildcard $(SRCDIR)/domain/config/*.cpp) \
           $(wildcard $(SRCDIR)/domain/memory/*.cpp) \
           $(wildcard $(SRCDIR)/domain/paging/*.cpp) \
           $(wildcard $(SRCDIR)/application/*.cpp) \
           $(wildcard $(SRCDIR)/infrastructure/*.cpp)

OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

# Tests implementados actualmente. test_mmu.cpp se incorporara cuando tenga main.
TEST_SRCS := $(TEST_DIR)/test_memoryconfig.cpp \
             $(TEST_DIR)/test_virtualaddress.cpp \
             $(TEST_DIR)/test_directorytableentry.cpp \
             $(TEST_DIR)/test_directorytable.cpp \
             $(TEST_DIR)/test_frametable.cpp
TEST_BINS := $(patsubst $(TEST_DIR)/%.cpp,$(BIN_DIR)/%$(EXE_EXT),$(TEST_SRCS))
TEST_BINS_WIN := $(subst /,\,$(TEST_BINS))

# Dependencias necesarias para los tests actuales, sin incluir main.cpp.
TEST_CORE_SRCS := $(SRCDIR)/domain/config/MemoryConfig.cpp \
                  $(SRCDIR)/domain/paging/VirtualAddress.cpp \
                  $(SRCDIR)/domain/paging/DirectoryTableEntry.cpp \
                  $(SRCDIR)/domain/paging/DirectoryTable.cpp \
                  $(SRCDIR)/domain/paging/PageTable.cpp \
                  $(SRCDIR)/domain/paging/PageTableEntry.cpp \
                  $(SRCDIR)/domain/memory/FrameTable.cpp

# ---------------------------------------------------------
# Targets principales
# ---------------------------------------------------------
.PHONY: all clean run test

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

test: $(TEST_BINS)
ifeq ($(OS),Windows_NT)
	@for %%t in ($(TEST_BINS_WIN)) do %%t || exit /b 1
else
	@for t in $(TEST_BINS); do ./$$t || exit 1; done
endif
	@echo Todos los tests pasaron.

$(BIN_DIR)/%$(EXE_EXT): $(TEST_DIR)/%.cpp $(TEST_CORE_SRCS)
	@$(call MKDIR,$(@D))
	$(CXX) $(CXXFLAGS) $< $(TEST_CORE_SRCS) -o $@