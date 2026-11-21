# Builds the C references and the sysl benchmark, then runs every pair. Run it through marshal as
# ONE exclusive job, from the repository root:
#
#     caffeinate -dimsu marshal run --exclusive --quiet-load auto --dir $PWD -- zsh bench/run.zsh <outdir>
#
# Output: one line per kernel, "<lang> <name> <ns per 64-bit output> <checksum>".
set -e
out=${1:-bench/out}
mkdir -p $out
root=$PWD
cflags=(-O3 -march=native -w)

clang $cflags -o $out/zt tools/ziggurat_tables.c -lm
$out/zt c > bench/ziggurat.h

cd bench/c
clang $cflags -o $root/$out/c-splitmix splitmix.c
clang $cflags -DXO='"ref/xoshiro256plusplus.c"' -DNAME='"xoshiro256pp"' -o $root/$out/c-xpp xoshiro.c -lm
clang $cflags -DXO='"ref/xoshiro256starstar.c"' -DNAME='"xoshiro256ss"' -o $root/$out/c-xss xoshiro.c -lm
clang $cflags -DXO='"ref/xoshiro256plusplus.c"' -DSAMPLERS -o $root/$out/c-samplers xoshiro.c -lm
clang $cflags -o $root/$out/c-pcg pcg64.c
for r in 8 12 20; do clang $cflags -DROUNDS=$r -DNAME="\"chacha$r\"" -o $root/$out/c-chacha$r chacha.c; done
clang $cflags -o $root/$out/c-philox philox.c
clang $cflags -o $root/$out/c-mt32 mt32.c
clang $cflags -o $root/$out/c-mt64 mt64.c
clang++ $cflags -std=c++17 -stdlib=libc++ -o $root/$out/cpp-mt mt_std.cpp
cd $root/bench/sysl
sysl build --release -O3 --cpu native --lib $root -o $root/$out/sysl-bench .
cd $root

for b in c-splitmix c-xpp c-xss c-pcg c-chacha8 c-chacha12 c-chacha20 c-philox c-mt32 c-mt64 cpp-mt c-samplers; do
  $out/$b | sed "s/^/${b%%-*} /"
done
$out/sysl-bench all | sed "s/^/sysl /"
