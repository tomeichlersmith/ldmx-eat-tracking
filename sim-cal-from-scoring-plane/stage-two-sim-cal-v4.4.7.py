import argparse
from pathlib import Path
parser = argparse.ArgumentParser()
parser.add_argument("input_file", type=Path, help="input file with no-cal sim")
args = parser.parse_args()

from LDMX.Framework import ldmxcfg
p = ldmxcfg.Process('simcal')
p.inputFiles = [str(args.input_file)]
p.outputFiles = [str(args.input_file.stem) + "_with_cal.root"]
p.logFrequency = 1
p.logger.termLevel = 1

import LDMX.Ecal.EcalGeometry
import LDMX.Hcal.HcalGeometry
from LDMX.SimCore import simulator
sim = simulator.simulator(instance_name='scoring-plane-sim')
sim.setDetector("ldmx-det-v14-8gev")

from LDMX.SimCore.generators import FromScoringPlane
sim.description = "sim calorimeters using scoring plane hits from previous no-cal sim"
sim.generators = [
    FromScoringPlane(
        coll_name='EcalScoringPlaneHits',
        pass_name='simtrack',
        select_planes=[31]
    )
]

# Load the ECAL modules
import LDMX.Ecal.EcalGeometry
import LDMX.Ecal.ecal_hardcoded_conditions
import LDMX.Ecal.digi as ecal_digi

# Load the HCAL modules
import LDMX.Hcal.HcalGeometry
import LDMX.Hcal.hcal_hardcoded_conditions
import LDMX.Hcal.digi as hcal_digi
p.sequence = [
    sim,
    ecal_digi.EcalDigiProducer(),
    ecal_digi.EcalRecProducer(), 
    hcal_digi.HcalDigiProducer(),
    hcal_digi.HcalRecProducer()
]
p.pause()
