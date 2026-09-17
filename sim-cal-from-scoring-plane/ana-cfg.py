import argparse
parser = argparse.ArgumentParser()
parser.add_argument('input_dir', help='input directory of files to study')
parser.add_argument('-o','--output',default='hist.root',help='output file to write histograms into')
args = parser.parse_args()

from LDMX.Framework import ldmxcfg
p = ldmxcfg.Process('ana')
p.sequence = [ ldmxcfg.Analyzer.from_file('Zone0Study.cxx') ]
p.inputDir(args.input_dir)
p.histogramFile = args.output
