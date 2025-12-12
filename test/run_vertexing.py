import FWCore.ParameterSet.Config as cms
from TrackingTools.TransientTrack.TransientTrackBuilder_cfi import *

process = cms.Process("DarkShowerAnalysis")
from Configuration.AlCa.GlobalTag import GlobalTag
process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("sct_vertex.Vertexing_Analyzer.vertexing_cfi")

process.load("TrackingTools/TransientTrack/TransientTrackBuilder_cfi")
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = '150X_mcRun3_2024_realistic_v2'

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(7000))

process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    'root://cms-xrd-global.cern.ch//store/mc/RunIII2024Summer24MiniAOD/GluGluHToDarkShowers-ScenarioA_Par-ctau-1p0-mA-2p00-mpi-10_TuneCP5_13p6TeV_powheg-pythia8/MINIAODSIM/140X_mcRun3_2024_realistic_v26-v2/140000/ad3a1bee-13b6-4d0c-90da-7ca085c674e6.root',
    'root://cms-xrd-global.cern.ch//store/mc/RunIII2024Summer24MiniAOD/GluGluHToDarkShowers-ScenarioA_Par-ctau-1p0-mA-2p00-mpi-10_TuneCP5_13p6TeV_powheg-pythia8/MINIAODSIM/140X_mcRun3_2024_realistic_v26-v2/140000/06ce5622-abe5-41a3-9952-4c8f6a4a2731.root',
    'root://cms-xrd-global.cern.ch//store/mc/RunIII2024Summer24MiniAOD/GluGluHToDarkShowers-ScenarioA_Par-ctau-1p0-mA-2p00-mpi-10_TuneCP5_13p6TeV_powheg-pythia8/MINIAODSIM/140X_mcRun3_2024_realistic_v26-v2/140000/26105aa6-a22d-4a70-a898-79c44fa762f6.root'
  ),
  duplicateCheckMode = cms.untracked.string("noDuplicateCheck")
)
process.p = cms.Path(process.scouting_vertexing)