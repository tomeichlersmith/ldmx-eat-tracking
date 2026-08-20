# Sim Stage Two: simcal

After we have used the no-cal simulation to produce a lot of events and filter
out the small fraction that are "dangerous" from a tracker perspective,
we want to finish the simulation with the calorimeters so that we can study if these
dangerous events actually cause trouble for the analysis.

The simulation from the scoring plane hits of a previous simulation can be done with
relatively little C++ development.
[ldmx-sw PR #2108](https://github.com/LDMX-Software/ldmx-sw/pull/2108) introduces
this feature into a future ldmx-sw release, but I've also backported this feature
to v4.4.7 ldmx-sw since that is the version used to create a 10M EoT sample used
for initial analysis. The backport is unfortunately necessary since v4.4.8 introduced
a breaking change to the on-disk format of the event data model, making it difficult
to read files produced by the old version with a newer version of ldmx-sw.

## Backport v4.4.7
In order to be able to use this feature on the old v4.4.7-produced file,
you will need to compile backported version of the feature yourself.

```
git clone --branch v4.4.7-backports/sim-from-scoring-plane \
  git@github.com:LDMX-Software/ldmx-sw.git
denv init ldmx/dev:v5.2.4 --over
just ldmx-sw/configure
just ldmx-sw/build
denv fire stage-two-cal.py skim-file.root
```
