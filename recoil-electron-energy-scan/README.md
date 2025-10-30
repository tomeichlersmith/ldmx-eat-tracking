# Recoil Electron Energy Scan
Configuration script and supporting files for generating a sample
that scans over recoil electron energies while running recoil tracking
and ECal reconstruction.

## Environment
- Using the ldmx/pro:v4.4.7 inherited from the parent directory.
- Running on zebra01 at UMN which has 16 cores so I can get this done in a day without needing to use the cluster

## Running
Using GNU's Parallel to run the same config over the variety of energies.

```
./run
```

## Sharing
Copied data to shared filesystem so that it is visible from other nodes on the cluster.
```
# organize into structure I've settled into
# maybe incorporate this into the run script?
mkdir data
mkdir data/detail
mv logs data/detail/
cp sim-track-cfg.py data/detail/
mv category_perprecoil_*.root data/
# actually copy
rsync -avmu data/ /local/cms/user/eichl008/ldmx/eat/v14/8gev/bkgd/perprecoil/
```
