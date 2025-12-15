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
#include "DataFormats/Scouting/interface/Run3ScoutingVertex.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/Math/interface/LorentzVector.h"

#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TMath.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TFile.h"



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
    //edm::EDGetTokenT<std::vector<Run3ScoutingMuon>> muTokenScoutingNoVtx_;
    edm::EDGetTokenT<std::vector<Run3ScoutingVertex>> svTokenScouting_;
    edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> theTransientTrackBuilderToken_;
    std::vector<float> lxy_Vertexing, lxy_NoVtx;
    TH1D* h_lxyVertexing;
    TH1D* h_lxyNoVtx;
    double min_Pt, max_eta;
};

//Constructor
vertexing::vertexing(const edm::ParameterSet& iConfig) :
  muTokenScoutingVtx_{consumes<std::vector<Run3ScoutingMuon>>(iConfig.getParameter<edm::InputTag>("ScoutingmuonsVtx"))},
  //muTokenScoutingNoVtx_{consumes<std::vector<Run3ScoutingMuon>>(iConfig.getParameter<edm::InputTag>("ScoutingmuonsNoVtx"))},
  svTokenScouting_{consumes<std::vector<Run3ScoutingVertex>>(iConfig.getParameter<edm::InputTag>("hltScoutingMuonPacker_displacedVtx"))},
  theTransientTrackBuilderToken_{esConsumes(edm::ESInputTag("", "TransientTrackBuilder"))}
  {}


//Destructor
vertexing::~vertexing() = default;

void vertexing::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  run = iEvent.id().run();
  lumi = iEvent.id().luminosityBlock();
  evtn = iEvent.id().event();
  min_Pt = 2;
  max_eta = 2.5;
  double MuMass = 0.105658;
  double MuMass2 = MuMass * MuMass;
  double minInvMass = 0.5;
  double maxInvMass = 30;
  double minPtPair_ = 2;

  const auto& theTransientTrackBuilder = iSetup.getData(theTransientTrackBuilderToken_);
  edm::Handle<std::vector<Run3ScoutingMuon>> ScoutingmuonsVtx;
  //edm::Handle<std::vector<Run3ScoutingMuon>> ScoutingmuonsNoVtx;
  edm::Handle<std::vector<Run3ScoutingVertex>> ScoutingdisplacedVertices;
  
  iEvent.getByToken(muTokenScoutingVtx_, ScoutingmuonsVtx);
  //iEvent.getByToken(muTokenScoutingNoVtx_, ScoutingmuonsNoVtx);
  iEvent.getByToken(svTokenScouting_, ScoutingdisplacedVertices);

  const auto& muonCollectionVtx = *ScoutingmuonsVtx;
  unsigned int nMus = muonCollectionVtx.size();
  //std::cout << "nMus: " << nMus << std::endl;

  for (unsigned int iMu1 = 0; iMu1 < nMus; ++iMu1) {
    const auto& mu1 = muonCollectionVtx[iMu1];
    if (mu1.pt() < min_Pt) continue;
    if (std::abs(mu1.eta()) > max_eta) continue;

    for (unsigned int iMu2 = iMu1 + 1; iMu2 < nMus; ++iMu2) {
      const auto& mu2 = muonCollectionVtx[iMu2];

      if (mu2.pt() < min_Pt) continue;
      if (std::abs(mu2.eta()) > max_eta) continue;
      if (mu1.charge() * mu2.charge() > 0) continue;

      int charge1 = mu1.charge();
      double chi2_1 = mu1.trk_chi2();
      double ndof1 = mu1.trk_ndof();

      double px1 = mu1.trk_pt() * std::cos(mu1.trk_phi());  
      double py1 = mu1.trk_pt() * std::sin(mu1.trk_phi());  
      double pz1 = mu1.trk_pt() * std::sinh(mu1.trk_eta()); 

      int charge2 = mu2.charge();
      double chi2_2 = mu2.trk_chi2();
      double ndof2 = mu2.trk_ndof();

      double px2 = mu2.trk_pt() * std::cos(mu2.trk_phi()); 
      double py2 = mu2.trk_pt() * std::sin(mu2.trk_phi()); 
      double pz2 = mu2.trk_pt() * std::sinh(mu2.trk_eta());

      reco::TrackBase::Vector momentum1(px1, py1, pz1);
      reco::TrackBase::Vector momentum2(px2, py2, pz2);

      double e1 = std::sqrt(px1*px1 + py1*py1 + pz1*pz1 + MuMass2);
      double e2 = std::sqrt(px2*px2 + py2*py2 + pz2*pz2 + MuMass2);

      math::XYZTLorentzVector p1(px1, py1, pz1, e1);
      math::XYZTLorentzVector p2(px2, py2, pz2, e2);
      auto p = p1 + p2;


      if (p.pt() < minPtPair_) continue;

      double invmass = std::abs(p.mass());
      if (invmass < minInvMass) continue;
      if (invmass > maxInvMass) continue;


      reco::TrackBase::Point refPoint1(
        mu1.trk_vx(),
        mu1.trk_vy(),
        mu1.trk_vz()
      );

      reco::TrackBase::CovarianceMatrix cov1 = reco::TrackBase::CovarianceMatrix();
      cov1(0,0) = mu1.trk_qoverpError()*mu1.trk_qoverpError();
      cov1(1,1) = mu1.trk_lambdaError()*mu1.trk_lambdaError();
      cov1(2,2) = mu1.trk_phiError()*mu1.trk_phiError();
      cov1(3,3) = mu1.trk_dxyError()*mu1.trk_dxyError();
      cov1(4,4) = mu1.trk_dszError()*mu1.trk_dszError();

      cov1(0,1) = mu1.trk_qoverp_lambda_cov();
      cov1(1,0) = mu1.trk_qoverp_lambda_cov();
      cov1(0,2) = mu1.trk_qoverp_phi_cov();
      cov1(2,0) = mu1.trk_qoverp_phi_cov();
      cov1(0,3) = mu1.trk_qoverp_dxy_cov();
      cov1(3,0) = mu1.trk_qoverp_dxy_cov();
      cov1(0,4) = mu1.trk_qoverp_dsz_cov();
      cov1(4,0) = mu1.trk_qoverp_dsz_cov();

      cov1(1,2) = mu1.trk_lambda_phi_cov();
      cov1(2,1) = mu1.trk_lambda_phi_cov();
      cov1(1,3) = mu1.trk_lambda_dxy_cov();
      cov1(3,1) = mu1.trk_lambda_dxy_cov();
      cov1(1,4) = mu1.trk_lambda_dsz_cov();
      cov1(4,1) = mu1.trk_lambda_dsz_cov();

      cov1(2,3) = mu1.trk_phi_dxy_cov();
      cov1(3,2) = mu1.trk_phi_dxy_cov();
      cov1(2,4) = mu1.trk_phi_dsz_cov();
      cov1(4,2) = mu1.trk_phi_dsz_cov();

      cov1(3,4) = mu1.trk_dxy_dsz_cov();
      cov1(4,3) = mu1.trk_dxy_dsz_cov();

      reco::Track track1(chi2_1,
        ndof1,
        refPoint1,
        momentum1,
        charge1,
        cov1,
        reco::TrackBase::undefAlgorithm,
        reco::TrackBase::undefQuality
      );

      reco::TrackBase::CovarianceMatrix cov2 = reco::TrackBase::CovarianceMatrix();
      cov2(0,0) = mu2.trk_qoverpError()*mu2.trk_qoverpError();
      cov2(1,1) = mu2.trk_lambdaError()*mu2.trk_lambdaError();
      cov2(2,2) = mu2.trk_phiError()*mu2.trk_phiError();
      cov2(3,3) = mu2.trk_dxyError()*mu2.trk_dxyError();
      cov2(4,4) = mu2.trk_dszError()*mu2.trk_dszError();

      cov2(0,1) = mu2.trk_qoverp_lambda_cov();
      cov2(1,0) = mu2.trk_qoverp_lambda_cov();
      cov2(0,2) = mu2.trk_qoverp_phi_cov();
      cov2(2,0) = mu2.trk_qoverp_phi_cov();
      cov2(0,3) = mu2.trk_qoverp_dxy_cov();
      cov2(3,0) = mu2.trk_qoverp_dxy_cov();
      cov2(0,4) = mu2.trk_qoverp_dsz_cov();
      cov2(4,0) = mu2.trk_qoverp_dsz_cov();

      cov2(1,2) = mu2.trk_lambda_phi_cov();
      cov2(2,1) = mu2.trk_lambda_phi_cov();
      cov2(1,3) = mu2.trk_lambda_dxy_cov();
      cov2(3,1) = mu2.trk_lambda_dxy_cov();
      cov2(1,4) = mu2.trk_lambda_dsz_cov();
      cov2(4,1) = mu2.trk_lambda_dsz_cov();

      cov2(2,3) = mu2.trk_phi_dxy_cov();
      cov2(3,2) = mu2.trk_phi_dxy_cov();
      cov2(2,4) = mu2.trk_phi_dsz_cov();
      cov2(4,2) = mu2.trk_phi_dsz_cov();

      cov2(3,4) = mu2.trk_dxy_dsz_cov();
      cov2(4,3) = mu2.trk_dxy_dsz_cov();

      reco::TrackBase::Point refPoint2(
        mu2.trk_vx(),
        mu2.trk_vy(),
        mu2.trk_vz()
      );

      reco::Track track2(chi2_2,
        ndof2,
        refPoint2,
        momentum2,
        charge2,
        cov2,
        reco::TrackBase::undefAlgorithm,
        reco::TrackBase::undefQuality
      );

      std::vector<reco::TransientTrack> ttracks;
      ttracks.push_back(theTransientTrackBuilder.build(track1));
      ttracks.push_back(theTransientTrackBuilder.build(track2));

      KalmanVertexFitter kvf(true);
      TransientVertex fittedVertex = kvf.vertex(ttracks);

      if (fittedVertex.isValid() && fittedVertex.normalisedChiSquared() < 5) {
        GlobalPoint vtxPos = fittedVertex.position();
        double lxy = std::hypot(vtxPos.x(), vtxPos.y());
        lxy_Vertexing.push_back(lxy);
        h_lxyVertexing->Fill(lxy);

        //std::cout << "Vtx collection lxy: " << lxy << std::endl;

      } 
      //else {
      //  std::cout << "Vertex fit failed" << std::endl;
      //}
        
    }
  }

  const auto& sct_svCollection = *ScoutingdisplacedVertices;
  unsigned int nSVs = sct_svCollection.size();
  for (unsigned int iSV = 0; iSV < nSVs; ++iSV) {
    const auto& sv = (*ScoutingdisplacedVertices)[iSV];
    double lxy_reco = std::hypot(sv.x(), sv.y());
    lxy_NoVtx.push_back(lxy_reco);
    h_lxyNoVtx->Fill(lxy_reco);
    //std::cout << "NoVtx collection SV lxy: " << lxy_reco << std::endl;
  };
};


void vertexing::beginJob() {
  edm::Service<TFileService> fs;
  h_lxyVertexing = new TH1D("h_lxyVertexing", "Lxy from Vertexing;L_{xy} [cm];Entries", 100, 0, 40);
  h_lxyNoVtx    = new TH1D("h_lxyNoVtx",    "Lxy from NoVtx;L_{xy} [cm];Entries", 100, 0, 40);
}

void vertexing::endJob() {
    TCanvas* c1 = new TCanvas("c1", "Lxy Comparison", 800, 600);
    h_lxyVertexing->SetLineColor(kRed);
    h_lxyNoVtx->SetLineColor(kBlue);

    h_lxyVertexing->Draw("HIST");
    h_lxyNoVtx->Draw("HIST SAME");

    c1->BuildLegend();
    c1->SaveAs("lxy_comparison_after_cuts.png");
}

DEFINE_FWK_MODULE(vertexing);