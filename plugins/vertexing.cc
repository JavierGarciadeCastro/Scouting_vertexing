// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "DataFormats/Scouting/interface/Run3ScoutingMuon.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"


//Header
class vertexing : public edm::one::EDAnalyzer<> {
  public:
    explicit SctParLooper(const edm::ParameterSet&);
    ~vertexing() override;
    void beginJob() override;
    void endJob() override;
    void analyze(const edm::Event&, const edm::EventSetup&) override;

  private:
};

//Constructor
vertexing::vertexing(const edm:ParameterSet& iConfig) :


//Destructor
vertexing::~vertexing() = default

void vertexing::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  run = iEvent.id().run();
  lumi = iEvent.id().luminosityBlock();
  evtn = iEvent.id().event();
  const auto& theTransientTrackBuilder = iSetup.getData(theTransientTrackBuilderToken_);
  edm::Handle<std::vector<Run3ScoutingMuon>> ScoutingmuonsVtx;
  iEvent.getByToken(muTokenScoutingVtx_, ScoutingmuonsVtx);

  TrackBase::CovarianceMatrix cov;
  cov.setZero();
  std::vector<reco::Track> trackCollection;

  const auto& muonCollectionVtx = *ScoutingmuonsVtx;
  unsigned int nMus = muonCollectionVtx.size();

  for (unsigned int iMu = 0; iMu < nMus; ++iMu) {
    const auto& mu = muonCollectionVtx[iMu];
    cov(0,0) = mu.trk_qoverpError()*mu.trk_qoverpError();
    cov(1,1) = mu.trk_lambdaError()*mu.trk_lambdaError();
    cov(2,2) = mu.trk_phiError()*mu.trk_phiError();
    cov(3,3) = mu.trk_dxyError()*mu.trk_dxyError();
    cov(4,4) = mu.trk_dszError()*mu.trk_dszError();

    cov(0,1) = mu.trk_qoverp_lambda_cov();
    cov(1,0) = mu.trk_qoverp_lambda_cov();
    cov(0,2) = mu.trk_qoverp_phi_cov();
    cov(2,0) = mu.trk_qoverp_phi_cov();
    cov(0,3) = mu.trk_qoverp_dxy_cov();
    cov(3,0) = mu.trk_qoverp_dxy_cov();
    cov(0,4) = mu.trk_qoverp_dsz_cov();
    cov(4,0) = mu.trk_qoverp_dsz_cov();

    cov(1,2) = mu.trk_lambda_phi_cov();
    cov(2,1) = mu.trk_lambda_phi_cov();
    cov(1,3) = mu.trk_lambda_dxy_cov();
    cov(3,1) = mu.trk_lambda_dxy_cov();
    cov(1,4) = mu.trk_lambda_dsz_cov();
    cov(4,1) = mu.trk_lambda_dsz_cov();

    cov(2,3) = mu.trk_phi_dxy_cov();
    cov(3,2) = mu.trk_phi_dxy_cov();
    cov(2,4) = mu.trk_phi_dsz_cov();
    cov(4,2) = mu.trk_phi_dsz_cov();

    cov(3,4) = mu.trk_dxy_dsz_cov();
    cov(4,3) = mu.trk_dxy_dsz_cov();

  }

}
