#! /bin/bash

# SHADER COMPILE ALL
# by A31Nesta

# DEPENDENCIES:
# - bgfx-shaderc

# Get output path (current directory by default)
outPath="$1"
if [ ! -d "$outPath" ]; then
    outPath="."
fi

for FILE in *; do
    fileNoExtension="${FILE%%.*}"

    # Ignore varying.def.sc
    if [ "$FILE" = "varying.def.sc" ]; then
        echo "varying.def.sc file detected, ignoring..."
        continue
    fi

    if [ ${fileNoExtension:0:2} = "v-" ]; then
        echo "$FILE: Compiling Vertex Shader..."
        bgfx-shaderc -f "$FILE" -o "$outPath/${fileNoExtension:2}.vs" --profile spirv --type vertex -i /usr/include/bgfx
        echo "Saving to: $outPath/${fileNoExtension:2}.vs"
    fi
    if [ ${fileNoExtension:0:2} = "f-" ]; then
        echo "$FILE: Compiling Fragment Shader..."
        bgfx-shaderc -f "$FILE" -o "$outPath/${fileNoExtension:2}.fs" --profile spirv --type fragment -i /usr/include/bgfx
        echo "Saving to: $outPath/${fileNoExtension:2}.fs"
    fi
done