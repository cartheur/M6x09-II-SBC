#!/usr/bin/env bash
set -euo pipefail

repo_root=$(CDPATH= cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
assembler="$repo_root/src/assembler/as9"
source_file="$repo_root/src/assist-09/assist09.asm"
reference_image="$repo_root/src/assist-09/assist09.bin"
build_dir=$(mktemp -d "${TMPDIR:-/tmp}/assist09-build.XXXXXX")

trap 'rm -rf -- "$build_dir"' EXIT

make -C "$repo_root/src/assembler" as9
cp "$source_file" "$build_dir/assist09.asm"

(
    cd "$build_dir"
    "$assembler" assist09.asm -l c s bin s19 cre now
)

image="$build_dir/assist09.bin"
srecord="$build_dir/assist09.s19"
image_size=$(wc -c < "$image")
first_record=$(awk '/^S1/ { print substr($0, 5, 4); exit }' "$srecord")
last_record=$(awk '/^S1/ { last = substr($0, 5, 4) } END { print last }' "$srecord")
reset_vector=$(od -An -tx1 -j 2046 -N2 "$image" | tr -d '[:space:]')

test "$image_size" -eq 2048
test "$first_record" = F800
test "$last_record" = FFF0
test "$reset_vector" = f837
cmp -s "$image" "$reference_image"

printf 'ASSIST09 image verified: 2048 bytes, $F800-$FFFF, reset vector $F837.\n'
printf 'Rebuild matches src/assist-09/assist09.bin.\n'
