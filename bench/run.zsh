# Builds the C references and the sysl benchmark, then runs every pair. Run it through marshal as
# ONE exclusive job, from the repository root:
#
#     caffeinate -dimsu marshal run --exclusive --quiet-load auto --dir $PWD -- zsh bench/run.zsh <outdir>
#
# Output: one line per kernel, "<lang> <name> <ns per 64-bit output> <checksum>". The checksums of a
# C line and its sysl twin must agree: both halves compute the same numbers.
set -e
mkdir -p ${1:-bench/out}
out=${${1:-bench/out}:A}
root=$PWD
cflags=(-O3 -march=native -w)

clang $cflags -o $out/zt tools/ziggurat_tables.c -lm
$out/zt c > bench/ziggurat.h

cd bench/c
clang $cflags -o $out/c-splitmix splitmix.c
clang $cflags -DXO='"ref/xoshiro256plusplus.c"' -DNAME='"xoshiro256pp"' -o $out/c-xpp xoshiro.c -lm
clang $cflags -DXO='"ref/xoshiro256starstar.c"' -DNAME='"xoshiro256ss"' -o $out/c-xss xoshiro.c -lm
clang $cflags -DXO='"ref/xoshiro256plusplus.c"' -DSAMPLERS -o $out/c-samplers xoshiro.c -lm
clang $cflags -o $out/c-pcg pcg64.c
for r in 8 12 20; do clang $cflags -DROUNDS=$r -DNAME="\"chacha$r\"" -o $out/c-chacha$r chacha.c; done
clang $cflags -o $out/c-philox philox.c
clang $cflags -o $out/c-mt32 mt32.c
clang $cflags -o $out/c-mt64 mt64.c
clang++ $cflags -std=c++17 -stdlib=libc++ -o $out/cpp-mt mt_std.cpp
cd $root/bench/sysl
sysl build --release -O3 --cpu native --lib $root -o $out/sysl-bench .
cd $root

for b in c-splitmix c-xpp c-xss c-pcg c-chacha8 c-chacha12 c-chacha20 c-philox c-mt32 c-mt64 cpp-mt c-samplers; do
  $out/$b | sed "s/^/${b%%-*} /"
done
$out/sysl-bench all | sed "s/^/sysl /"
