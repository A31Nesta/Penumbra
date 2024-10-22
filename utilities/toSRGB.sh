#! /bin/bash

# TO SRGB
# by A31Nesta

# DEPENDENCIES:
# - ImageMagick

inputFile="$1"
outputFile="$2"

magick "$inputFile" -colorspace sRGB -define png:color-type=6 "$outputFile"