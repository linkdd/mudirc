ifeq ($(OS),Windows_NT)
	AUTOGEN         = powershell.exe ./autogen.ps1
	AUTOGEN_RELARGS = -Release
	RMDIR           = powershell.exe -c "Remove-Item -Recurse -Force"
else
	AUTOGEN         = bash ./autogen.sh
	AUTOGEN_RELARGS = --release
	RMDIR           = rm -rf
endif

.PHONY: all
all: debug

.PHONY: debug
debug:
	@$(MAKE) debug/configure
	@$(MAKE) debug/build
	@$(MAKE) debug/install

.PHONY: debug/clean
debug/clean:
	@$(MAKE) -C __build__/Debug clean
	@$(RMDIR) dist/Debug

.PHONY: debug/configure
debug/configure:
	@$(AUTOGEN)

.PHONY: debug/build
debug/build:
	@$(MAKE) -C __build__/Debug all

.PHONY: debug/install
debug/install:
	@$(MAKE) -C __build__/Debug install

.PHONY: release
release:
	@$(MAKE) release/configure
	@$(MAKE) release/build
	@$(MAKE) release/install

.PHONY: release/clean
release/clean:
	@$(MAKE) -C __build__/Release clean
	@$(RMDIR) dist/Release

.PHONY: release/configure
release/configure:
	@$(AUTOGEN) $(AUTOGEN_RELARGS)

.PHONY: release/build
release/build:
	@$(MAKE) -C __build__/Release all

.PHONY: release/install
release/install:
	@$(MAKE) -C __build__/Release install
