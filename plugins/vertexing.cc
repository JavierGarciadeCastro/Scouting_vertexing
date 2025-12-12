// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/Scouting/interface/Run3ScoutingMuon.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"



//Header
class vertexing : public edm::one::EDAnalyzer<> {
  public:
    explicit vertexing(const edm::ParameterSet&);
    ~vertexing() override;
    void beginJob() override;
    void endJob() override;
    void analyze(const edm::Event&, const edm::EventSetup&) override;

  private:
    unsigned int run, lumi, evtn;
    edm::EDGetTokenT<std::vector<Run3ScoutingMuon>> muTokenScoutingVtx_;
    edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> theTransientTrackBuilderToken_;
};

//Constructor
vertexing::vertexing(const edm::ParameterSet& iConfig) :
  muTokenScoutingVtx_{consumes<std::vector<Run3ScoutingMuon>>(iConfig.getParameter<edm::InputTag>("ScoutingmuonsVtx"))},
  theTransientTrackBuilderToken_{esConsumes(edm::ESInputTag("", "TransientTrackBuilder"))}
  {}


//Destructor
vertexing::~vertexing() = default;

void vertexing::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  run = iEvent.id().run();
  lumi = iEvent.id().luminosityBlock();
  evtn = iEvent.id().event();
  const auto& theTransientTrackBuilder = iSetup.getData(theTransientTrackBuilderToken_);
  edm::Handle<std::vector<Run3ScoutingMuon>> ScoutingmuonsVtx;
  iEvent.getByToken(muTokenScoutingVtx_, ScoutingmuonsVtx);

  std::vector<reco::Track> trackCollection;

  const auto& muonCollectionVtx = *ScoutingmuonsVtx;
  unsigned int nMus = muonCollectionVtx.size();

  for (unsigned int iMu = 0; iMu < nMus; ++iMu) {
    const auto& mu = muonCollectionVtx[iMu];

    reco::TrackBase::CovarianceMatrix cov = reco::TrackBase::CovarianceMatrix();
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

    int charge = mu.charge();
    double chi2 = mu.trk_chi2();
    double ndof = mu.trk_ndof();

    double px = mu.trk_pt() * std::cos(mu.trk_phi());   //Check this
    double py = mu.trk_pt() * std::sin(mu.trk_phi());   //Check this
    double pz = mu.trk_pt() * std::sinh(mu.trk_eta());  //Check this

    reco::TrackBase::Vector momentum(px, py, pz);
    reco::TrackBase::Point vtx(0., 0., 0.);

    reco::Track track(
      chi2,
      ndof,
      vtx,
      momentum,
      charge,
      cov,
      reco::TrackBase::undefAlgorithm,
      reco::TrackBase::undefQuality
    );

    trackCollection.push_back(track);
  };

  if (trackCollection.size() >= 2) {
    std::vector<reco::TransientTrack> ttracks;
    for (auto& trk : trackCollection) {
      ttracks.push_back(theTransientTrackBuilder.build(trk));
    }

    KalmanVertexFitter kvf(true);
    TransientVertex fittedVertex = kvf.vertex(ttracks);
    if (fittedVertex.isValid()) {
      GlobalPoint vtxPos = fittedVertex.position();
      std::cout << "Fitted vertex: "
                << vtxPos.x() << ", "
                << vtxPos.y() << ", "
                << vtxPos.z() << std::endl;
    } else {
      std::cout << "Vertex fit failed" << std::endl;
    }
  }

};


void vertexing::beginJob() {
}

void vertexing::endJob() {
}

DEFINE_FWK_MODULE(vertexing);