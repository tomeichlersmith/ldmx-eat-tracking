# sim track no cal
Simulate the tracking detectors and filter out events that are of particular interest due to weird behavior in them after reconstruction.

## set up
Building and pushing the image only needs to happen if we want to change
the version of ldmx-sw (the `ldmx/pro` BASE image) or the track analyzer code itself.
```
podman image build . -t docker://ghcr.io/tomeichlersmith/ldmx-eat-tracking:v4.9.3-$(git -C chelsea-code rev-parse --short=8 HEAD)
podman push docker://ghcr.io/tomeichlersmith/ldmx-eat-tracking:v4.9.3-$(git -C chelsea-code rev-parse --short=8 HEAD)
```

## running
create a simulating environment
```
denv init ghcr.io/tomeichlersmith/ldmx-eat-tracking:v4.9.3-SHA
```
The SHA refers to the commit SHA of `rodr1001/my_code` whiel the version is the ldmx-sw version.
The images that are options are stored on GHCR: https://github.com/users/tomeichlersmith/packages/container/package/ldmx-eat-tracking

The configuration script for ldmx-sw is in this directory and should be run in the specialized
container image built earlier in order to have access to Chelsea's track analyzer
that helps identify these "weird behavior" events (and keeps a small fraction of
"normal" events for double checking).
```
denv fire sim_track_no_cal.py 1 --nevents 10000
```
