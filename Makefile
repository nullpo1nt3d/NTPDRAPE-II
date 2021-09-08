# This makefile will improve over time, this is just hacked together
# so that it does what I need.

MODE = release
CXX = clang++
LD = lld-link
RCTOOL = rc

CXXFLAGS = -m32 -std=c++17 -Wall -D_X86_ -D_CRT_SECURE_NO_DEPRECATE -O1 -D UNICODE -D NDEBUG
LDFLAGS = -manifest:EMBED comctl32.lib shell32.lib user32.lib winmm.lib libcmt.lib # vcruntime.lib

ifeq ($(MODE), debug)
CXXFLAGS = -m32 -std=c++17 -Wall -D_X86_ -D_CRT_SECURE_NO_DEPRECATE -D UNICODE -D _DEBUG
LDFLAGS = -manifest:EMBED comctl32.lib libcmtd.lib shell32.lib user32.lib winmm.lib
endif

RCFLAGS = -nologo
MTFLAGS = -nologo

TARFLAGS = --exclude="*.tar.gz" --exclude="bin" --exclude="obj"

PROGRAM_NAME = NTPDRAPE-II

BINDIR = bin
OBJDIR = obj

OBJS = $(OBJDIR)/$(PROGRAM_NAME).res \
	   $(OBJDIR)/utils.o \
       $(OBJDIR)/main.o

ifeq ($(MODE), debug)
all: $(BINDIR)/$(PROGRAM_NAME)_debug.exe
else
all: $(BINDIR)/$(PROGRAM_NAME).exe
endif

# Rules for output directories
$(BINDIR)/:
	@echo "[>] Creating $@ directory"
	@mkdir $(BINDIR)

$(OBJDIR)/:
	@echo "[>] Creating $@ directory"
	@mkdir $(OBJDIR)

# Linking and manifesting phase
ifeq ($(MODE), debug)
$(BINDIR)/$(PROGRAM_NAME)_debug.exe: $(BINDIR)/ $(OBJDIR)/ $(OBJS)
else
$(BINDIR)/$(PROGRAM_NAME).exe: $(BINDIR)/ $(OBJDIR)/ $(OBJS)
endif
	@echo "Linking and manifesting $@"
	@$(LD) -out:$@ $(OBJS) $(LDFLAGS) -manifestinput:src/res/$(PROGRAM_NAME).manifest
# 	@echo "UPX'ing $@"
# 	@upx -9 $@

# Compilation phase
$(OBJDIR)/%.o: src/%.cpp
	@echo "[>] CXX $<"
	@$(CXX) -c $(CXXFLAGS) $< -o$@

# Resource compilation
$(OBJDIR)/%.res: src/%.rc
	@echo "[>] RC $<"
	@$(RCTOOL) $(RCFLAGS) -Fo$@ $<

# Clean up
clean:
	@echo "[>] Cleaning up"
	@rmdir /Q /S $(BINDIR) $(OBJDIR)

# Distributions
bin_dist:
	@echo "[>] Making binary distribution"
	@tar -c -v -f $(PROGRAM_NAME)_bin_dist.tar.gz $(BINDIR)

src_dist:
	@echo "[>] Making source code distribution"
	@tar -c -v -f $(PROGRAM_NAME)_src_dist.tar.gz $(TARFLAGS) .