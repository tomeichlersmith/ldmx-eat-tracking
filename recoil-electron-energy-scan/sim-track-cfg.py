#!/usr/bin/python

import sys
import os
import argparse

usage = "denv fire %s"%(sys.argv[0])
parser = argparse.ArgumentParser(usage,
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

parser.add_argument("run_number", default=1,
    help="Set the random seed for the simulation.",type=int)
parser.add_argument("beam_energy", default=7.5, type=float,
    help="Energy of electron leaving target in GeV")

arg = parser.parse_args()

from LDMX.Framework import ldmxcfg

p = ldmxcfg.Process( 'eat' )
p.maxTriesPerEvent = 1
p.maxEvents = 10000
p.termLogLevel = 1
p.logFrequency = 100

p.run = arg.run_number

from LDMX.Biasing import eat
from LDMX.SimCore import simulator
bkgd_sim = simulator.simulator('inclusive_eat_bkgd')
bkgd_sim.setDetector('ldmx-det-v14-8gev',True)
from LDMX.SimCore import generators
recoil = generators.gun('single_perpendicular_recoil_electron')
recoil.particle = 'e-'
recoil.position = [ 0., 0., +1.2 ] # mm
recoil.direction = [ 0., 0., 1.0 ]
recoil.energy = arg.beam_energy
bkgd_sim.generators = [ recoil ]
bkgd_sim.description = 'perpedicular recoil electron, no biasing and no filtering applied'
bkgd_sim.beamSpotSmear = [20., 80., 0.] #mm

p.outputFiles = [ 
    '_'.join([
        'category', 'perprecoil',
        'Nevents', '10k',
        'MaxTries', '1',
        'run', f'{arg.run_number:04d}',
        'energy', f'{arg.beam_energy:.2f}'
    ])+'.root'
]

import LDMX.Ecal.EcalGeometry
import LDMX.Ecal.ecal_hardcoded_conditions
import LDMX.Ecal.digi as ecal_digi
import LDMX.Ecal.vetos as ecal_vetos
from LDMX.Tracking import full_tracking_sequence as tracking

import LDMX.Hcal.HcalGeometry

# EaT C++ Analyzers not compiled into ldmx/pro:v4.4.7
# --> will need to copy PEFF deduction code into histogram filling analyzier
#
# Can't do Trigger calculation including TS electron counting
# because we want to choose energy of recoil electron and shoot
# it from downstream of the target.
# --> deduce trigger selection manually from EcalRecHits

p.sequence = [
    bkgd_sim,
    ecal_digi.EcalDigiProducer(),
    ecal_digi.EcalRecProducer(),
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
