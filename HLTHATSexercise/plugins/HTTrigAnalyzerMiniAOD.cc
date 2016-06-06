/** \class HTTrigAnalyzerMiniAOD
 *
 * See header file for documentation
 *
 *  \author Dominick Olivito
 *
 */

#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Common/interface/TriggerResultsByName.h"
#include "HLTHATS/HLTHATSexercise/interface/HTTrigAnalyzerMiniAOD.h"

#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

// ROOT includes
#include "Math/VectorUtil.h"

#include <cassert>

using namespace reco;
using namespace edm;

//
// constructors and destructor
//
//____________________________________________________________________________
HTTrigAnalyzerMiniAOD::HTTrigAnalyzerMiniAOD(const edm::ParameterSet& ps) 
{
  using namespace std;
  using namespace edm;

  processName_ = ps.getUntrackedParameter<std::string>("processName","HLT");
  refTriggerName_ = ps.getUntrackedParameter<std::string>("refTriggerName","HLT_PFHT475_v2");
  sigTriggerName_ = ps.getUntrackedParameter<std::string>("sigTriggerName","HLT_PFHT800_v2");
  triggerResultsToken_ = consumes<edm::TriggerResults> (ps.getUntrackedParameter<edm::InputTag>("triggerResultsTag", edm::InputTag("TriggerResults", "", "HLT")));
  pfJetsToken_ = consumes<edm::View<pat::Jet> >(ps.getUntrackedParameter<edm::InputTag>("pfJetsInputTag", edm::InputTag("slimmedJets")));
  pfJetPt_ = ps.getUntrackedParameter<double>  ( "pfJetPt", 30. );
  pfJetEta_ = ps.getUntrackedParameter<double>  ( "pfJetEta", 3.0 );
  verbose_ = ps.getUntrackedParameter<bool>("verbose",false);
    
  // histogram setup
  edm::Service<TFileService> fs;
  hists_1d_["h_passreftrig"] = fs->make<TH1F>("h_passreftrig" , "; passed ref trigger" , 2 , 0. , 2. );
  hists_1d_["h_ht_all"] = fs->make<TH1F>("h_ht" , "; H_{T} [GeV]" , 120 , 0. , 1200. );
  hists_1d_["h_ht_passtrig"] = fs->make<TH1F>("h_ht_passtrig" , "; H_{T} [GeV]" , 120 , 0. , 1200. );

}

//____________________________________________________________________________
HTTrigAnalyzerMiniAOD::~HTTrigAnalyzerMiniAOD()
{
}

//
// member functions
//
//____________________________________________________________________________
void
HTTrigAnalyzerMiniAOD::beginRun(edm::Run const & iRun, edm::EventSetup const& iSetup)
{
  using namespace std;
  using namespace edm;

  bool changed(true);
  if (hltConfig_.init(iRun,iSetup,processName_,changed)) {
    if (changed) {
      const unsigned int n(hltConfig_.size());
      // check if trigger names in (new) config
      unsigned int refTriggerIndex(hltConfig_.triggerIndex(refTriggerName_));
      if (refTriggerIndex>=n) {
	cout << "HTTrigAnalyzerMiniAOD::analyze:"
	     << " TriggerName " << refTriggerName_ 
	     << " not available in config!" << endl;
      }
      unsigned int sigTriggerIndex(hltConfig_.triggerIndex(sigTriggerName_));
      if (sigTriggerIndex>=n) {
	cout << "HTTrigAnalyzerMiniAOD::analyze:"
	     << " TriggerName " << sigTriggerName_ 
	     << " not available in config!" << endl;
      }
    } // if changed
  } else {
    cout << "HTTrigAnalyzerMiniAOD::analyze:"
	 << " config extraction failure with process name "
	 << processName_ << endl;
  }

}

//____________________________________________________________________________
// ------------ method called to produce the data  ------------
void
HTTrigAnalyzerMiniAOD::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace std;
  using namespace edm;
  using namespace reco;
  using namespace trigger;

  if (verbose_) cout << endl;

  // get event products
  iEvent.getByToken(triggerResultsToken_,triggerResultsHandle_);
  if (!triggerResultsHandle_.isValid()) {
    cout << "HTTrigAnalyzerMiniAOD::analyze: Error in getting TriggerResults product from Event!" << endl;
    return;
  }

  // sanity check
  assert(triggerResultsHandle_->size()==hltConfig_.size());

  // retrieve necessary containers
  Handle<edm::View<pat::Jet> > pfJetsHandle_;
  iEvent.getByToken( pfJetsToken_ , pfJetsHandle_ );

  if (verbose_) cout << endl;

  const unsigned int ntrigs(hltConfig_.size());
  const unsigned int refTriggerIndex(hltConfig_.triggerIndex(refTriggerName_));
  assert(refTriggerIndex==iEvent.triggerNames(*triggerResultsHandle_).triggerIndex(refTriggerName_));
  const unsigned int sigTriggerIndex(hltConfig_.triggerIndex(sigTriggerName_));
  assert(sigTriggerIndex==iEvent.triggerNames(*triggerResultsHandle_).triggerIndex(sigTriggerName_));

  // abort on invalid trigger name
  if (refTriggerIndex>=ntrigs) {
    cout << "HTTrigAnalyzerMiniAOD::analyzeTrigger: path "
	 << refTriggerName_ << " - not found!" << endl;
    return;
  }
  if (sigTriggerIndex>=ntrigs) {
    cout << "HTTrigAnalyzerMiniAOD::analyzeTrigger: path "
	 << sigTriggerName_ << " - not found!" << endl;
    return;
  }

  if (verbose_) {
    cout << "HTTrigAnalyzerMiniAOD::analyzeTrigger: reference path "
	 << refTriggerName_ << " [" << refTriggerIndex << "]" << endl;
    cout << "HTTrigAnalyzerMiniAOD::analyzeTrigger: signal path "
	 << sigTriggerName_ << " [" << sigTriggerIndex << "]" << endl;
  }
  
  // modules on this trigger path
  bool refAccept = triggerResultsHandle_->accept(refTriggerIndex);
  bool sigAccept = triggerResultsHandle_->accept(sigTriggerIndex);

  if (refAccept) hists_1d_["h_passreftrig"]->Fill(1);
  else {  
    // don't consider event if reference trigger didn't fire
    hists_1d_["h_passreftrig"]->Fill(0);
    return;
  }

  float ht = 0.;
  // loop over reco jets and compute HT
  for(View<pat::Jet>::const_iterator pfjet_it = pfJetsHandle_->begin(); pfjet_it != pfJetsHandle_->end(); pfjet_it++) {
    if (pfjet_it->pt() < pfJetPt_) continue;
    if (fabs(pfjet_it->eta()) > pfJetEta_) continue;
    ht += pfjet_it->pt();
    if (verbose_) cout << " - good jet: pt: " << pfjet_it->pt() << ", eta: " << pfjet_it->eta()
		       << ", phi: " << pfjet_it->phi() << endl;
  }

  if (verbose_) cout << "ht: " << ht << endl;

  hists_1d_["h_ht_all"]->Fill(ht);
  if (sigAccept) hists_1d_["h_ht_passtrig"]->Fill(ht);
  
  if (verbose_) cout << endl;
  return;
}

