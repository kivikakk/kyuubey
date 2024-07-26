{
  inputs = {
    nixpkgs.url = github:NixOS/nixpkgs/release-24.05;
    flake-utils.url = github:numtide/flake-utils;
  };

  outputs = {
    self,
    flake-utils,
    nixpkgs,
  }:
    flake-utils.lib.eachDefaultSystem (system: let
      pkgs = import nixpkgs {inherit system;};
      buildInputs = with pkgs; [
        SDL2
        bison
        flex
      ];
    in {
      formatter = pkgs.alejandra;

      devShells.default = pkgs.mkShell {
        inherit buildInputs;
      };

      packages.default = pkgs.stdenv.mkDerivation {
        name = "kyuubey";

        src = ./.;

        inherit buildInputs;

        installPhase = ''
          mkdir -p $out/bin
          cp kyuubey $out/bin/kyuubey
        '';
      };
    });
}
