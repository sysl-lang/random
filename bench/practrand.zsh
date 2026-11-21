# PractRand over every generator's raw stream, to 2^32 bytes (or further: pass the limit).
#
#     caffeinate -dimsu marshal run --dir $PWD -- zsh bench/practrand.zsh <RNG_test> <outdir> [4GB]
#
# Each generator writes its next_u64 words, least significant byte first, to RNG_test stdin64. The
# seeds are the benchmark's (pcg64dxsm is seeded(1, 0)). One summary block per generator in
# <outdir>/practrand-<name>.txt.
set -e
rng_test=$1
limit=${3:-4GB}
root=$PWD
mkdir -p $2
out=${2:A}
cd bench/sysl
sysl build --release -O3 --cpu native --lib $root -o $out/sysl-bench .
cd $root

for gen in splitmix64 xoshiro256pp xoshiro256ss pcg64dxsm chacha8 chacha12 chacha20 philox4x32 mt19937 mt19937_64; do
  $out/sysl-bench stream $gen | $rng_test stdin64 -tlmax $limit -multithreaded > $out/practrand-$gen.txt 2>&1 || true
  echo "== $gen"
  tail -6 $out/practrand-$gen.txt
done
