import FWCore.ParameterSet.Config as cms

vertexing = cms.EDAnalyzer("vertexing",
    ScoutingmuonsVtx = cms.InputTag("hltScoutingMuonPackerVtx")
)

scouting_vertexing = cms.Sequence(vertexing)