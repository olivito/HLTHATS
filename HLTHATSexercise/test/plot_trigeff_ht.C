#include "TFile.h"
#include "TH1.h"
#include "TLegend.h"
#include "TString.h"
#include "TCanvas.h"
#include "TEfficiency.h"
#include "TStyle.h"

void plot_trigeff_ht (const TString& infile = "histos_HTTrigAnalyzer.root") {

  gStyle->SetPadTopMargin(0.08);
  gStyle->SetPadBottomMargin(0.12);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetOptStat(0);

  TH1::SetDefaultSumw2();
  
  TFile* f_in = new TFile(infile);

  // ---- efficiency vs ht
  
  TH1F* h_ht_denom = (TH1F*) f_in->Get("htTrigAnalyzerMiniAOD/h_ht");
  TH1F* h_ht_num = (TH1F*) f_in->Get("htTrigAnalyzerMiniAOD/h_ht_passtrig");
  
  TCanvas* c_ht = new TCanvas("c_ht","c_ht");
  c_ht->SetGrid(1,1);
  c_ht->cd();

  TH2F* h_ht_axis = new TH2F("h_ht_axis",";H_{T} [GeV];Efficiency of HLT_PFHT800",60,600,1200,20,0,1);
  h_ht_axis->GetYaxis()->SetTitleOffset(0.98);
  h_ht_axis->Draw();
  
  TEfficiency* h_ht_eff = new TEfficiency(*h_ht_num, *h_ht_denom);
  h_ht_eff->SetLineColor(kRed);
  h_ht_eff->SetMarkerColor(kRed);
  
  h_ht_eff->Draw("pe same");
  
 
  return;
}
