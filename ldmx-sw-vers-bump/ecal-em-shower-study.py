from pathlib import Path
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('run_number', type=int, help='run number defining RNG seed')
parser.add_argument('--out-dir', type=Path, default=Path.cwd(), help='directory to put output files')
parser.add_argument('--nevents', type=int, default=1000000, help='number of single-electron events to sim')
parser.add_argument("--mimic-signal", action="store_true", help="whether to mimic signal with a range of low incident energies or not")
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

p = ldmxcfg.Process("emshower")
p.run = args.run_number
p.max_events = args.nevents
p.output_files = [str(
    args.out_dir / (
        '_'.join([
            'category', 'ecalemshower',
            'beam', '8gev',
            'signalmimic', "yes" if args.mimic_signal else "no",
            'nevents', string_counts(p.max_events),
            'run', f'{p.run:04d}'
        ])+'.root'
    )
)]

import LDMX.Ecal.ecal_geometry
import LDMX.Hcal.hcal_geometry
import LDMX.Ecal.ecal_hardcoded_conditions
import LDMX.Hcal.hcal_hardcoded_conditions

from LDMX.SimCore import generators, simulator, photonuclear_models
sim = simulator.Simulator(instance_name="electron_em")
sim.set_detector("ldmx-det-v16-8gev", False)
sim.description = "Disable PN, electron fired into ECal front face"
sim.photonuclear_model = photonuclear_models.NoPhotoNuclearModel()

beam_energy = [
    "/gps/ene/type Mono",
    "/gps/ene/mono 8 GeV",
]

mimic_signal = [
    "/gps/ene/type Lin",
    "/gps/ene/min 0.5 GeV",
    "/gps/ene/max 3.0 GeV",
    "/gps/ene/gradient 0",
    "/gps/ene/intercept 1",
    # could use /gps/ang/... commands to alter angle of entry as well
]

sim.generators = [
    generators.Gps([
        "/gps/particle e-",
        "/gps/number 1",
        "/gps/pos/type Plane",
        "/gps/pos/shape Rectangle",
        "/gps/pos/centre 0 0 235 mm",
        "/gps/pos/halfx 10 mm",
        "/gps/pos/halfy 40 mm",
        "/gps/direction 0 0 1",
        *(mimic_signal if args.mimic_signal else beam_energy)
    ])
]

from LDMX.Ecal import digi as ecal_digi
from LDMX.Hcal import digi as hcal_digi

p.sequence = [
    sim,
    hcal_digi.HcalDigiProducer(),
    hcal_digi.HcalRecProducer(),
    ecal_digi.EcalDigiProducer(),
    ecal_digi.EcalRecProducer()
]
