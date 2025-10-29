"""estimate run time of a sample"""

from datetime import timedelta
from pathlib import Path

import uproot
import awkward as ak
import numpy as np

def pretty_time(seconds):
    td = timedelta(seconds = int(seconds))
    return str(td)


def load(no_cache = False):
    if not no_cache and load.__cache_file__.is_file():
        return ak.from_parquet(load.__cache_file__)

    timestamps = uproot.concatenate(
        {'/local/cms/user/eichl008/ldmx/eat/v14/8gev/bkgd/true-inclusive-0-with-tracking/category_*.root':'LDMX_Run'},
        expressions = ['RunHeader/runStart_','RunHeader/runEnd_']
    )
    runtime = timestamps['RunHeader/runEnd_']-timestamps['RunHeader/runStart_']
    ak.to_parquet(runtime, load.__cache_file__)
    return runtime


load.__cache_file__ = Path('.runtime-cache.parquet')
    

def main():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--re-scan',
            action='store_true', help='force re-scan of ROOT files instead of using parquet cache')
    args = parser.parse_args()

    runtime = load(no_cache = args.re_scan)

    mean = ak.mean(runtime)
    stdd = np.sqrt(ak.sum((runtime-mean)**2)/ak.count(runtime))
    print(f'Run Time: {mean:.1f} +/- {stdd:.1f} s')
    print(f'HH:MM:SS: {pretty_time(mean)} +/- {pretty_time(stdd)}')


if __name__ == '__main__':
    main()
