{ pkgs ? import <nixpkgs> {} }:
	with pkgs; mkShell 
{
	buildInputs = [ 
		git
		meson
		ninja
		cmake
		gcc
	];
}
