# Tracking Selection for EaT


I have not investigated the signal samples, but those (as we talked about in the meeting yesterday) don't need to be as large so we can re-generate them from scratch although I do not know how they will look without the simulation-level mock-tracker filter but including the necessary filter to require a dark brem in the ECal.
9:42 AM
 For EaT, the signal and background "look the same" in the tagger and recoil trackers, so we could look at generating a large sample that does not include the calorimeters.
This significantly improves simulation time and saves all the disk space that would keep the ECal/HCal hits.
After the copy is done, I'll do a few runs of this "no-cal" setup to try to estimate the CPU-time and disk-space requirements for a given EoT.

## Attempts to do Tracking on Old Runs
- Bad News: We don't have a large background sample for our purposes.
  - The 1e13 EoT Equivalent biased samples (Enriched Nuclear and Dimuon) have the simulation-level mock-tracker filter applied.
  - The 1B EoT unbiased sample has the tracker filter applied and most of its runs dropped the simulated hits in the tracking systems to save space
- Good News: The "True Inclusive" sample which we used to study the simulation-level mock-tracker filter does not have any filtering applied at simulation and has the tracking system simulated hits. It is 10M EoT.

With this survey done, I am choosing to run tracking on the 8GeV True Inclusive sample.

### v4.4.9
Appears to fail to convert between schemas leading to a segmentation violation when the
tracking attempts to access necessary parameters.

Full log of attempted run is in v4.4.9-attempt.log (with escape sequences included).

### v4.4.7
Tried this because maybe the `clang-tidy` run which did a mass re-name of members broke
the default schema evolution (no way for ROOT to know that `myMember_` should go to `my_member_`),
so dropping back to this version to see what happens.

**This works** :tada: (or at least finishes running)

### v3.2.10
I did try this but the tracking was not being run in the CI, so I have no evidence that it was working
at the time. When I ran it, I saw a buffer overflow error when the Tracking system was attempting to
create the geometry.

No log, but if I want to retry:
1. switch image `denv config image ldmx/pro:v3.2.10`
2. update .profile to avoid using ldmx-env-init.sh: remove `ldmx-env-init.sh` reference and just set `PYTHONPATH` and `PATH` explicitly with `LDMX_SW_INSTALL`
3. copy `Tracking/python/examples.py` configuration of seeding and tracking into config

## No-Cal Detector
Just in case we want to re-sim from scratch but fast with focus on trackers, we actually have a "no cals" detector built with ldmx-sw for tracking studies that we can utilize.

The [sim-track-no-cal-cfg.py](sim-track-no-cal-cfg.py) configuration script uses this detector.

### CPU-Time, Disk-Space Estimates
Basic survey with a single run.
```
time denv fire sim-track-no-cal-cfg.py 1 --nevents N
stat -c '%s' category_inclusivenocals..._nevents_N.root
```

First, naive attemp.

  N  |  Time / s | Disk / B
-----|-----------|--------------
1    |    19.078 |       109 573
10   |    20.216 |       209 246
100  |    20.324 |     1 200 306
1k   |    28.412 |    12 058 568
10k  |   126.265 |   123 859 074
100k |  1034.262 | 1 204 167 873
1M   | 10738.8   | 12 GB <- estimate

From this I conclude that I need to save disk space,
looking at dropping the calorimeter SDs so that empty collections don't waste space.
