_default:
    @just --list --justfile {{ justfile() }}

# open a jupyter lab here
jupyter:
    uv run jupyter lab --no-browser

# run tracking in local patches
track:
    mkdir -p track-out/detail
    find /local/cms/user/eichl008/ldmx/eat/v14/8gev/bkgd/true-inclusive-0 \
      -type f -name '*.root' | \
      sort > track-out/detail/input-files.list 
    parallel \
      --joblog track-out/detail/parallel-job.log \
      -j 4 \
      'denv fire tracking-cfg.py --out-dir track-out {} &> track-out/detail/{/.}.log' \
      :::: track-out/detail/input-files.list
