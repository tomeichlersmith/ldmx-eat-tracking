import argparse
parser = argparse.ArgumentParser()
parser.add_argument('input_file', nargs='+',help='input ROOT files to study')
parser.add_argument('-n','--n-events',type=int,default=-1,help='maximum number of events to study')
parser.add_argument('-o','--output',default='hist.root',help='output file to write histograms into')
args = parser.parse_args()

from LDMX.Framework import ldmxcfg
p = ldmxcfg.Process('ana')
p.sequence = [ ldmxcfg.Analyzer.from_file('Tracking4EaTStudy.cxx') ]
p.inputFiles = args.input_file
p.histogramFile = args.output
p.maxEvents = args.n_events
