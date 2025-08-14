from pathlib import Path
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('run_number', type=int, help='run number defining RNG seed')
parser.add_argument('--out-dir', type=Path, default=Path.cwd(), help='directory to put output files')
parser.add_argument('--nevents', type=int, default=10, help='number of single-electron events to sim')
args = parser.parse_args()

args.out_dir.mkdir(exist_ok = True, parents = True)

def string_counts(n):
    suffix, divisor = '', 1
    if n >= 1000 and n < 1e6:
        suffix, divisor = 'k', 1000
    elif n >= 1e6 and n < 1e9:
        suffix, divisor = 'M', 1000000
    return f'{n // divisor}{suffix}'

from LDMX.Framework import ldmxcfg
p = ldmxcfg.Process('simtrack')
p.run = args.run_number
p.maxEvents = args.nevents
p.outputFiles = [str(
    args.out_dir / (
        '_'.join([
            'category', 'inclusivenocals',
            'beam', '8gev',
            'run', str(p.run),
            'nevents', string_counts(p.maxEvents)
        ])+'.root'
    )
)]

from LDMX.SimCore import simulator, generators, sensitive_detectors
sim = simulator.simulator('sim')
sim.setDetector('ldmx-det-v14-8gev-no-cals', True)
sim.generators = [
    generators.single_8gev_e_upstream_tagger()
]
sim.description = 'single electron 8gev beam, no calorimeters, no biasing or filtering'
sim.sensitive_detectors = [
    sensitive_detectors.TrackerSD.tagger(),
    sensitive_detectors.TrackerSD.recoil(),
    sensitive_detectors.TrigScintSD.target(),
    sensitive_detectors.TrigScintSD.pad1(),
    sensitive_detectors.TrigScintSD.pad2(),
    sensitive_detectors.TrigScintSD.pad3(),
    sensitive_detectors.ScoringPlaneSD.tracker(),
    sensitive_detectors.ScoringPlaneSD.target()
]

# v4.4.7
from LDMX.Tracking import full_tracking_sequence as tracking
# dropping truth_tracking to avoid extrapolation error messages
# (and we don't need it, we want to see how the nominal setup works)
p.sequence = [
    sim,
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
