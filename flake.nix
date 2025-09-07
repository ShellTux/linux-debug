{
  description = "A Nix-flake-based C/C++ development environment";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";

    pre-commit-hooks = {
      url = "github:cachix/git-hooks.nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };

    rust-overlay = {
      url = "github:oxalica/rust-overlay";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs =
    {
      self,
      nixpkgs,
      pre-commit-hooks,
      rust-overlay,
    }:
    let
      supportedSystems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];

      forEachSupportedSystem =
        f:
        nixpkgs.lib.genAttrs supportedSystems (
          system:
          f rec {
            inherit system;

            pkgs = import nixpkgs {
              inherit system;
              overlays = [
                rust-overlay.overlays.default
                self.overlays.default
              ];
            };
          }
        );
    in
    {
      overlays.default = final: prev: {
        rustToolchain =
          let
            rust = prev.rust-bin;
          in
          if builtins.pathExists ./rust-toolchain.toml then
            rust.fromRustupToolchainFile ./rust-toolchain.toml
          else if builtins.pathExists ./rust-toolchain then
            rust.fromRustupToolchainFile ./rust-toolchain
          else
            rust.stable.latest.default.override {
              extensions = [
                "rust-src"
                "rustfmt"
              ];
            };
      };

      checks = forEachSupportedSystem (
        { pkgs, system, ... }:
        {
          pre-commit-check = pre-commit-hooks.lib.${system}.run {
            src = ./.;
            hooks = {
              cargo-check.enable = true;
              clang-format.enable = true;
              # clippy.enable = true;
              nixfmt-rfc-style.enable = true;
              rustfmt.enable = true;
            };
          };
        }
      );

      devShells = forEachSupportedSystem (
        { pkgs, system, ... }:
        let
          inherit (pkgs) mkShell;
          inherit (pkgs.lib) getExe;
          inherit (pkgs) rustToolchain;
          inherit (pkgs.rust.packages.stable.rustPlatform) rustLibSrc;
          inherit (self.checks."${system}") pre-commit-check;

          onefetch = getExe pkgs.onefetch;
        in
        {
          default =
            mkShell.override
              {
                # Override stdenv in order to change compiler:
                # stdenv = pkgs.clangStdenv;
              }
              {
                packages = [
                  pkgs.clang-tools
                  pkgs.cmake
                  pkgs.codespell
                  pkgs.conan
                  pkgs.cppcheck
                  pkgs.doxygen
                  pkgs.gtest
                  pkgs.lcov
                  pkgs.vcpkg
                  pkgs.vcpkg-tool
                ]
                ++ (if system == "aarch64-darwin" then [ ] else [ pkgs.gdb ])
                ++ [
                  # Rust Toolchain
                  rustToolchain
                  pkgs.openssl
                  pkgs.pkg-config
                  pkgs.cargo-deny
                  pkgs.cargo-edit
                  pkgs.cargo-watch
                  pkgs.rust-analyzer
                ]
                ++ [
                  # Linux kernel dependencies
                  pkgs.bc
                  pkgs.bison
                  pkgs.byacc
                  pkgs.cpio
                  pkgs.elfutils
                  pkgs.flex
                  pkgs.gitFull # For git send-email 🫠
                  pkgs.glibc
                  pkgs.glibc.static
                  pkgs.global # For make gtags
                  pkgs.gmp
                  pkgs.gnumake
                  pkgs.kmod
                  pkgs.libelf
                  pkgs.libffi
                  pkgs.libmpc
                  pkgs.linuxHeaders
                  pkgs.mpfr
                  pkgs.ncurses # For make menuconfig
                  pkgs.nettools
                  pkgs.openssl
                  pkgs.pahole
                  pkgs.perl
                  pkgs.python3Minimal
                  pkgs.qemu
                  pkgs.rsync
                  pkgs.ubootTools
                  pkgs.zlib.static
                  pkgs.zstd
                ]
                ++ [
                  pkgs.autoconf
                  pkgs.automake
                  pkgs.bison
                  pkgs.gettext
                  pkgs.git
                  pkgs.gnum4
                  pkgs.gnumake
                  pkgs.gnutar
                  pkgs.gperf
                  pkgs.gzip
                  pkgs.help2man
                  pkgs.libptytty
                  pkgs.perl
                  pkgs.texinfo
                  pkgs.wget
                ]
                ++ pre-commit-check.enabledPackages;

                env = {
                  # Required by rust-analyzer
                  RUST_SRC_PATH = "${rustLibSrc}";
                };

                shellHook = ''
                  ${pre-commit-check.shellHook}
                  ${onefetch} --no-bots 2>/dev/null
                '';
              };
        }
      );
    };
}
