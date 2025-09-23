from LDMX.Framework import ldmxcfg
p = ldmxcfg.Process('ana')
p.sequence = [ ldmxcfg.Analyzer.from_file('Tracking4EaTStudy.cxx') ]
import sys
p.inputFiles = sys.argv[1:]
p.histogramFile = 'hist.root'
