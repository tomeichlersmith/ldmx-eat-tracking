from pathlib import Path
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('run_number', type=int, help='run number defining RNG seed')
parser.add_argument('--out-dir', type=Path, default=Path.cwd(), help='directory to put output files')
parser.add_argument('--nevents', type=int, default=1000000, help='number of single-electron events to sim')
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
p = ldmxcfg.Process('simtrk')
p.run = args.run_number
p.max_events = args.nevents
p.__dict__['log_performance'] = True
p.histogram_file = 'performance-log.root'
p.output_files = [str(
    args.out_dir / (
        '_'.join([
            'category', 'inclusivenocals',
            'beam', '8gev',
            'nevents', string_counts(p.max_events),
            'run', f'{p.run:04d}'
        ])+'.root'
    )
)]

from LDMX.SimCore import simulator, generators, sensitive_detectors
sim = simulator.Simulator('sim')
sim.set_detector('ldmx-det-v14-8gev-no-cals', True)
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
    sensitive_detectors.ScoringPlaneSD.target(),
    sensitive_detectors.ScoringPlaneSD.ecal()
]

# v4.4.7
from LDMX.Tracking.full_tracking_sequence import full_tracking_sequence
# dropping truth_tracking to avoid extrapolation error messages
# (and we don't need it, we want to see how the nominal setup works)
p.sequence = [
    sim,
    *full_tracking_sequence(detector="ldmx-det-v14-8gev").sequence
]
