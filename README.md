# Tracking Selection for EaT

## Attempts to do Tracking on Old Runs
Specifically, I am attempting to run Tracking on a file from the 8GeV True Inclusive Sample.

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
Just in case we want to re-sim from scratch but fast with focus on trackers.
```
denv cp /usr/local/data/detectors/ldmx-det-v15-8gev/detector.gdml
patch detector.gdml detector.gdml.patch
# run config with sim.detector = 'detector.gdml'
```
