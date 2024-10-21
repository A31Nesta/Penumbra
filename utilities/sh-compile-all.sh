#! /bin/bash

outPath="$1"

for FILE in *; do
    fileNoExtension="${FILE%%.*}"

    # Get actual file paths
    vtxShader="v-$fileNoExtension.sc"
    frgShader="f-$fileNoExtension.sc"

    echo "$vtxShader | $frgShader"

    # if [ ! -d "$outPath" ]; then
    #     echo "ERROR: The output path does not exist."
    #     exit 1
    # fi

    # if [ -f "$vtxShader" ]; then
    #     echo "Compiling Vertex Shader..."
    #     bgfx-shaderc -f "$vtxShader" -o "$outPath/$1.vs" --profile spirv --type vertex -i /usr/include/bgfx
    # fi
    # if [ -f "$frgShader" ]; then
    #     echo "Compiling Fragment Shader..."
    #     bgfx-shaderc -f "$frgShader" -o "$outPath/$1.fs" --profile spirv --type fragment -i /usr/include/bgfx
    # fi
done