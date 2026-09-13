# Supported entry point for GNU Make (Linux/WSL and Windows MSYS2).
.DEFAULT_GOAL := all
PLATFORM ?= $(if $(filter Windows_NT,$(OS)),windows,linux)
ifeq ($(PLATFORM),linux)
include MakefileLin
else ifeq ($(PLATFORM),windows)
include MakefileWindows
else
$(error PLATFORM must be linux or windows)
endif
