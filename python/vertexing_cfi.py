import FWCore.ParameterSet.Config as cms

vertexing = cms.EDAnalyzer("vertexing",
    ScoutingmuonsVtx = cms.InputTag("hltScoutingMuonPackerVtx"),
    #ScoutingmuonsNoVtx = cms.InputTag("hltScoutingMuonPackerNoVtx"),
    hltScoutingMuonPacker_displacedVtx = cms.InputTag("hltScoutingMuonPackerNoVtx","displacedVtx","HLT")
)

scouting_vertexing = cms.Sequence(vertexing)