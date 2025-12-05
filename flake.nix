{
  description = "A Nix-flake-based C/C++ development environment for forstty";
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";
  };
  outputs =
    { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        buildInputs = with pkgs; [
          clang-tools
          clang
          gcc
          cmake
          # ninja
          # make
          gdb
          # valgrind
          bear
          sdl3
        ];
        nativeBuildInputs = [ pkgs.pkg-config ];
        env = {
          CC = "gcc";
          CXX = "g++";
        };
      };
    };
}
