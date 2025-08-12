from pathlib import Path
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('input', type=Path, help='input ROOT file with TaggerSimHits and RecoilSimHits')
parser.add_argument('--max-events', type=int, help='limit number of events to process', default=-1)
args = parser.parse_args()

output = args.input.stem + '_track_yes.root'

from LDMX.Framework import ldmxcfg
p = ldmxcfg.Process('track')
p.maxEvents = args.max_events
p.inputFiles = [ str(args.input) ]
p.outputFiles = [ str(output) ]

# v4.4.9
from LDMX.Tracking import full_tracking_sequence
p.sequence = full_tracking_sequence.sequence
