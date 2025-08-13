from pathlib import Path
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('input', type=Path, help='input ROOT file with TaggerSimHits and RecoilSimHits')
parser.add_argument('--out-dir', type=Path, default=Path.cwd(), help='directory to put output files')
parser.add_argument('--max-events', type=int, help='limit number of events to process', default=-1)
args = parser.parse_args()

args.out_dir.mkdir(exist_ok = True, parents = True)
output = args.out_dir / (args.input.stem + '_track_yes.root')

from LDMX.Framework import ldmxcfg
p = ldmxcfg.Process('track')
p.maxEvents = args.max_events
p.inputFiles = [ str(args.input) ]
p.outputFiles = [ str(output) ]

# v4.4.7
from LDMX.Tracking import full_tracking_sequence as tracking
# dropping truth_tracking to avoid extrapolation error messages
# (and we don't need it, we want to see how the nominal setup works)
p.sequence = [
    tracking.digi_tagger,
    tracking.seeder_tagger,
    tracking.tracking_tagger,
    tracking.greedy_solver_tagger,
    tracking.GSF_tagger,
    tracking.digi_recoil,
    tracking.seeder_recoil,
    tracking.tracking_recoil,
    tracking.greedy_solver_recoil,
    tracking.GSF_recoil
]
