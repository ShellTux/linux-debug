CC = cc
CFLAGS += -Wall -Wextra
CFLAGS += -static -ggdb

CARGO_INSTALL_FLAGS += --target=x86_64-unknown-linux-gnu
CARGO_INSTALL_FLAGS += --root='out/rust'
RUSTFLAGS := -C target-feature=+crt-static

SOURCES := $(shell find src -name "*.c")

OUT_DIR := out

INIT := $(OUT_DIR)/init
ROOTFS_IMAGE := rootfs.cpio.gz
EXECUTABLES = $(SOURCES:src/%.c=$(OUT_DIR)/usr/local/bin/%)

all: $(ROOTFS_IMAGE)

.PHONY: vars
vars:
	@echo "EXECUTABLES = $(EXECUTABLES)"
	@echo "SOURCES = $(SOURCES)"
	@echo "ROOTFS_IMAGE = $(ROOTFS_IMAGE)"

.PHONY: executables
executables: $(EXECUTABLES)

$(INIT): src/init.c
	@mkdir --parents $(shell dirname $@)
	$(CC) $(CFLAGS) -o $@ $<

$(OUT_DIR)/usr/local/bin/%: src/%.c
	@mkdir --parents $(shell dirname $@)
	$(CC) $(CFLAGS) -o $@ $^

$(ROOTFS_IMAGE): $(INIT) $(EXECUTABLES) rust
	(cd out && find . | cpio -o -H newc | gzip > ../$@)

export RUSTFLAGS

.PHONY: rust
rust:
	cargo install $(CARGO_INSTALL_FLAGS) --path=..
	# cargo install $(CARGO_INSTALL_FLAGS) eza
	# cargo install $(CARGO_INSTALL_FLAGS) --git=https://github.com/microsoft/edit edit
	# cargo install $(CARGO_INSTALL_FLAGS) ad-editor
