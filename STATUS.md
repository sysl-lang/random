# Status

Parked on 2026-11-21, to resume after sysl 0.13.0 ships.

## Done

- Every generator in the README's table, the `Rng` trait and its distributions, the C references and
  the sysl twin in `bench/`, the PractRand driver, and the four findings in `findings/`.
- 41 tests passing on sysl 0.12.1, every reference vector included.
- Mutation pass, round 1: 27 mutants, 21 red, 2 equivalent, 3 survivors that showed real gaps.

## Not yet run

1. **`sysl test .`** after the last two tests were added (`chacha_layout_high_words` in
   `tests.sysl`, `ziggurat_fast_path_is_exact` in `distribution_tests.sysl`) and the normal-tail
   sign check was added to `normal_shape`. Those tests close the three round-1 survivors.
2. **Mutation round 2**: the seven mutants of `ROUND2` (the three survivors, unit's scale, the
   multiply-high, the layer byte, the exponential's scale). The mutator is a small script that copies
   the repository per mutant, applies one `(file, old, new)` replacement and runs `sysl test .`.
3. **PractRand**: `caffeinate -dimsu marshal run --dir $PWD -- zsh bench/practrand.zsh <RNG_test>
   <outdir>` (PractRand 0.95, built from source with clang++ -O3). The first try failed on a path
   bug that is now fixed and has not been rerun.
4. **Timing**: `bench/run.zsh` as ONE `marshal run --exclusive --quiet-load auto` job. Held until
   0.13.0. Then fill in the README's Speed section with one sysl/C ratio per generator and sampler,
   check that each pair's checksums agree, and for any gap over ~5% compare
   `sysl emit-llvm -O3 --cpu native` with `clang -O3 -S` to find the cause.

## Next

- Fill in the README's PractRand and Speed sections, then send the timing gaps as compiler findings.
- Optional scope: SFMT as a vector stressor, and a hardware entropy source with a fallback.
