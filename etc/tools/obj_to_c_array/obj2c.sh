#!/bin/bash

# Parses and converts Wavefront OBJ and MTL files (https://en.wikipedia.org/wiki/Wavefront_.obj_file).
#
# Notes for Blender export:
# - Edit Mode -> Select all (A) -> Face -> Triangulate Faces (Ctrl+T)
# - Export -> Wavefront (obj) -> Forward Axis -> Y
# - To use color alpha value, use "Alpha" directly in the material's "Surface" section (not inside the "Base Color").
#
# OBJ supported elements:
# - Oject (o), only one per file.
# - Vertices (v)
# - Faces (f)
# - Material names used by faces (usemtl), referencing the accompanying mtl file.
#
# MTL supported elements:
# - Materials (newmtl)
# - Diffuse color (Kd)
# - Dissolve factor (d), the alpha value.
#

function processVertex {
    local line=$1
    local result=$(echo "$line" | sed 's/v //g')
    result=$(echo "$result" | sed 's/ /,/g')
    echo {$result},
}

function processFace {
    local line=$1
    local result=$(echo "$line" | sed 's/f //g')
    result=$(echo "$result" | sed 's/ /,/g')
    result=$(echo "$result" | sed -r 's,([0-9]+)/[0-9]+/[0-9]+,\1,g')
    
    # Convert the string into an array.
    IFS=',' read -ra faceArray <<< "$result"

    # Loop through each number, subtract 1, and join them back into a string.
    result=""
    for num in "${faceArray[@]}"; do
        ((num--))
        result+="$num,"
    done

    # Remove the trailing comma.
    result="${result%,}"
    
    echo {$result},
}

function convertVertices {
    local fileName=$1

    echo "static fw_vec3f _v[] = {"
    cat $fileName".obj" | while read line
    do
        if [[ "$line" =~ ^v" ".* ]]; then
            processVertex "$line"
        fi
    done
    echo "};"
}

function convertFaces {
    local fileName=$1

    echo "static fw_vec3i _f[] = {"
    cat $fileName".obj" | while read line
    do
        if [[ "$line" =~ ^f" ".* ]]; then
            processFace "$line"
        fi
    done
    echo "};"
}

function convertMaterials {
    local fileName=$1
    
    declare -A indexMap
    declare -A colorMap
    declare -A alphaMap
    declare -a indexToNameArray

    # Subshell and Bash variable scope: https://stackoverflow.com/a/16854326/891846
    local idx=0
    local key=""
    while read line
    do
        if [[ "$line" =~ ^newmtl" ".* ]]; then
            key=$(echo "$line" | sed 's/newmtl //g')
            indexMap["$key"]=$idx
            indexToNameArray+=( "$key" )
            ((idx++))
        fi
        if [[ "$line" =~ ^Kd" ".* ]]; then
            local color=$(echo "$line" | sed 's/Kd //g')
            color=$(echo "$color" | sed 's/ /,/g')
            colorMap["$key"]=$color
        fi
        if [[ "$line" =~ ^d" ".* ]]; then
            local alpha=$(echo "$line" | sed 's/d //g')
            alpha=$(echo "$alpha" | sed 's/ /,/g')
            alphaMap["$key"]=$alpha
        fi   
    done < <(cat $fileName".mtl")
    
    echo "static fw_vec4f _c[] = {"
    idx=0
    for i in "${indexToNameArray[@]}"
    do
        echo {"${colorMap[$i]},${alphaMap[$i]}"}, "// $idx: $i"
        ((idx++))
    done
    echo "};"
    
    echo -n "static int _cidx[] = {"
    cat $fileName".obj" | while read line
    do
        if [[ "$line" =~ ^usemtl" ".* ]]; then
            key=$(echo "$line" | sed 's/usemtl //g')
            echo
        fi
        if [[ "$line" =~ ^f" ".* ]]; then
            echo -n ${indexMap["$key"]},
        fi
    done
    echo
    echo "};"
}

function convertFaceNormals {
    # Not implemented yet.
    #echo "static fw_vec3f _fn[LEN(_f)];"
    :
}


[ $# -ne 1 ] && {
    echo "Usage:";
    echo "  $0 <filename-prefix>"
    echo ""
    echo "Arguments:"
    echo "  filename-prefix  Name of the obj/mtl file without suffix."
    exit 1;
}

fileNamePrefix=$1

convertVertices "$fileNamePrefix"
convertFaces "$fileNamePrefix"
convertMaterials "$fileNamePrefix"
convertFaceNormals "$fileNamePrefix"
