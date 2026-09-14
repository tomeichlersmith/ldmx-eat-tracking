import argparse
parser = argparse.ArgumentParser()
parser.add_argument('input_file', nargs='+',help='input ROOT files to study')
args = parser.parse_args()

from LDMX.Framework import ldmxcfg
p = ldmxcfg.Process('z0skim')
p.sequence = [ ldmxcfg.Analyzer.from_file('Zone0Skim.cxx') ]
p.inputFiles = args.input_file
p.outputFiles = ['zone0_events.root']
p.skimDefaultIsDrop()
p.skimConsider('Zone0Skim')
