void getTT2CSCTFRates(){
  TChain* bg = new TChain("Events");
  bg->Add("/Data/cms/emu/phase2/CMSSW_6_2_0_SLHC14-ttiRedigiJun14ana/Extended2023TTI_FwdMuons/Neutrinos/runTTAnaMakers_redoL1MuPhase1_nugun/tt_ntuple_redoL1MuPhase1_*.root");

  TChain* bgp2 = new TChain("Events");
  bgp2->Add("/Data/cms/emu/phase2/CMSSW_6_2_0_SLHC14-ttiRedigiJun14ana/Extended2023TTI_FwdMuons/Neutrinos/runTTAnaMakers_redoL1MuPhase2_nugun/tt_ntuple_redoL1MuPhase2_*.root");

  bg->Draw("min(Max$(csctfs_pt*(abs(csctfs_eta)>=1.2&&abs(csctfs_eta)<2.1)),99.99)>>hCSCTF2p1(100,0,100)");
  makeRate(hCSCTF2p1);
  bg->Draw("min(Max$(csctfs_pt*(abs(csctfs_eta)>=1.2&&abs(csctfs_eta)<2.1&&csctfs_bx==0)),99.99)>>hCSCTF2p1BX0(100,0,100)");
  makeRate(hCSCTF2p1BX0);
  bg->Draw("min(Max$(gmts_pt*(abs(gmts_eta)>=1.2&&abs(gmts_eta)<2.1&&gmts_bx==0&&gmts_quality>=4)),99.99)>>hCSCTF2p1BX0Q4(100,0,100)");
  makeRate(hCSCTF2p1BX0Q4);
  bg->Draw("min(Max$(csctfs_pt*(abs(csctfs_eta)>=1.2&&abs(csctfs_eta)<2.1&&csctfs_bx==0&&csctfs_nlcts>=2&&csctfs_ringS1>0)),99.99)>>hCSCTF2p1BX0NL2ME1(100,0,100)");
  makeRate(hCSCTF2p1BX0NL2ME1);
  bg->Draw("min(Max$(csctfs_pt*(abs(csctfs_eta)>=1.2&&abs(csctfs_eta)<2.1&&csctfs_bx==0&&csctfs_nlcts>=3&&csctfs_ringS1>0)),99.99)>>hCSCTF2p1BX0NL3ME1(100,0,100)");
  makeRate(hCSCTF2p1BX0NL3ME1);
  bg->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.2&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.1&&csctfs_bx[tts_csctf_deps2_idx]==0)),99.99)>>hTTCSCTF2p1BX0(100,0,100)");
  makeRate(hTTCSCTF2p1BX0);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);
  
  gStyle->SetOptStat(0);

  hCSCTF2p1_Rate->Draw();
  hCSCTF2p1_Rate->SetTitle("Single #mu rate using CSCTF p_{T} for 1.2<|#eta|<2.1");
  hCSCTF2p1_Rate->GetXaxis()->SetTitle("p_{T}, GeV");
  hCSCTF2p1_Rate->SetLineWidth(2);
  hCSCTF2p1_Rate->SetLineColor(1);
  gPad->SetLogy(1);
  hCSCTF2p1_Rate->SetMinimum(0.1);

  hCSCTF2p1BX0_Rate->SetLineWidth(2);
  hCSCTF2p1BX0_Rate->SetLineColor(2);
  hCSCTF2p1BX0_Rate->Draw("same");

  hCSCTF2p1BX0Q4_Rate->SetLineWidth(2);
  hCSCTF2p1BX0Q4_Rate->SetLineColor(3);
  hCSCTF2p1BX0Q4_Rate->Draw("same");

  hCSCTF2p1BX0NL2ME1_Rate->SetLineWidth(2);
  hCSCTF2p1BX0NL2ME1_Rate->SetLineColor(4);
  hCSCTF2p1BX0NL2ME1_Rate->Draw("same");

  hCSCTF2p1BX0NL3ME1_Rate->SetLineWidth(2);
  hCSCTF2p1BX0NL3ME1_Rate->SetLineColor(6);
  hCSCTF2p1BX0NL3ME1_Rate->Draw("same");

  hTTCSCTF2p1BX0_Rate->SetLineWidth(2);
  hTTCSCTF2p1BX0_Rate->SetLineColor(7);
  hTTCSCTF2p1BX0_Rate->Draw("same");

  TLegend* leg = new TLegend(0.4, 0.7, 0.9, 0.9);
  leg->SetFillColor(0);
  leg->AddEntry(hCSCTF2p1_Rate, "All CSCTFs");
  leg->AddEntry(hCSCTF2p1BX0_Rate, "CSCTFs BX=0");
  leg->AddEntry(hCSCTF2p1BX0Q4_Rate, "GMTs BX=0 GMT Quality#geq4");
  leg->AddEntry(hCSCTF2p1BX0NL2ME1_Rate, "CSCTFs BX=0 N_{LCT}#geq2 w ME1");
  leg->AddEntry(hCSCTF2p1BX0NL3ME1_Rate, "CSCTFs BX=0 N_{LCT}#geq3 w ME1");
  leg->AddEntry(hTTCSCTF2p1BX0_Rate, "TTs: matching to CSCTF BX=0");
  leg->Draw();
  gPad->SaveAs("cms/emu/June1462X/hCSCTF2p1_rate.png");
  gPad->SaveAs("cms/emu/June1462X/hCSCTF2p1_rate.pdf");
  gPad->SaveAs("cms/emu/June1462X/hCSCTF2p1_rate.eps");


  bg->Draw("min(Max$(csctfs_pt*(abs(csctfs_eta)>=2.1&&abs(csctfs_eta)<2.4)),99.99)>>hCSCTFGE2p1(100,0,100)");
  makeRate(hCSCTFGE2p1);
  bg->Draw("min(Max$(csctfs_pt*(abs(csctfs_eta)>=2.1&&abs(csctfs_eta)<2.4&&csctfs_bx==0)),99.99)>>hCSCTFGE2p1BX0(100,0,100)");
  makeRate(hCSCTFGE2p1BX0);
  bg->Draw("min(Max$(gmts_pt*(abs(gmts_eta)>=2.1&&abs(gmts_eta)<2.4&&gmts_bx==0&&gmts_quality>=4)),99.99)>>hCSCTFGE2p1BX0Q4(100,0,100)");
  makeRate(hCSCTFGE2p1BX0Q4);
  bg->Draw("min(Max$(csctfs_pt*(abs(csctfs_eta)>=2.1&&abs(csctfs_eta)<2.4&&csctfs_bx==0&&csctfs_nlcts>=2&&csctfs_ringS1>0)),99.99)>>hCSCTFGE2p1BX0NL2ME1(100,0,100)");
  makeRate(hCSCTFGE2p1BX0NL2ME1);
  bg->Draw("min(Max$(csctfs_pt*(abs(csctfs_eta)>=2.1&&abs(csctfs_eta)<2.4&&csctfs_bx==0&&csctfs_nlcts>=3&&csctfs_ringS1>0)),99.99)>>hCSCTFGE2p1BX0NL3ME1(100,0,100)");
  makeRate(hCSCTFGE2p1BX0NL3ME1);
  bg->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=2.1&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.4&&csctfs_bx[tts_csctf_deps2_idx]==0)),99.99)>>hTTCSCTFGE2p1BX0(100,0,100)");
  makeRate(hTTCSCTFGE2p1BX0);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);
  

  hCSCTFGE2p1_Rate->Draw();
  hCSCTFGE2p1_Rate->SetTitle("Single #mu rate using CSCTF p_{T} for 2.1#leq|#eta|<2.4");
  hCSCTFGE2p1_Rate->GetXaxis()->SetTitle("p_{T}, GeV");
  hCSCTFGE2p1_Rate->SetLineWidth(2);
  hCSCTFGE2p1_Rate->SetLineColor(1);
  gPad->SetLogy(1);
  hCSCTFGE2p1_Rate->SetMinimum(0.1);

  hCSCTFGE2p1BX0_Rate->SetLineWidth(2);
  hCSCTFGE2p1BX0_Rate->SetLineColor(2);
  hCSCTFGE2p1BX0_Rate->Draw("same");

  hCSCTFGE2p1BX0Q4_Rate->SetLineWidth(2);
  hCSCTFGE2p1BX0Q4_Rate->SetLineColor(3);
  hCSCTFGE2p1BX0Q4_Rate->Draw("same");

  hCSCTFGE2p1BX0NL2ME1_Rate->SetLineWidth(2);
  hCSCTFGE2p1BX0NL2ME1_Rate->SetLineColor(4);
  hCSCTFGE2p1BX0NL2ME1_Rate->Draw("same");

  hCSCTFGE2p1BX0NL3ME1_Rate->SetLineWidth(2);
  hCSCTFGE2p1BX0NL3ME1_Rate->SetLineColor(6);
  hCSCTFGE2p1BX0NL3ME1_Rate->Draw("same");

  hTTCSCTFGE2p1BX0_Rate->SetLineWidth(2);
  hTTCSCTFGE2p1BX0_Rate->SetLineColor(7);
  hTTCSCTFGE2p1BX0_Rate->Draw("same");

  TLegend* leg = new TLegend(0.4, 0.7, 0.9, 0.9);
  leg->SetFillColor(0);
  leg->AddEntry(hCSCTFGE2p1_Rate, "All CSCTFs");
  leg->AddEntry(hCSCTFGE2p1BX0_Rate, "CSCTFs BX=0");
  leg->AddEntry(hCSCTFGE2p1BX0Q4_Rate, "GMTs BX=0 GMT Quality#geq4");
  leg->AddEntry(hCSCTFGE2p1BX0NL2ME1_Rate, "CSCTFs BX=0 N_{LCT}#geq2 w ME1");
  leg->AddEntry(hCSCTFGE2p1BX0NL3ME1_Rate, "CSCTFs BX=0 N_{LCT}#geq3 w ME1");
  leg->AddEntry(hTTCSCTFGE2p1BX0_Rate, "TTs: matching to CSCTF BX=0");
  leg->Draw();
  gPad->SaveAs("cms/emu/June1462X/hCSCTFGE2p1_rate.png");
  gPad->SaveAs("cms/emu/June1462X/hCSCTFGE2p1_rate.pdf");
  gPad->SaveAs("cms/emu/June1462X/hCSCTFGE2p1_rate.eps");

  //GE1/1 range: 1.6 to 2.1
  bg->Draw("min(Max$(csctfs_pt*(abs(csctfs_eta)>=1.6&&abs(csctfs_eta)<2.1)),99.99)>>hCSCTF1p6to2p1(100,0,100)");
  makeRate(hCSCTF1p6to2p1);
  bg->Draw("min(Max$(csctfs_pt*(abs(csctfs_eta)>=1.6&&abs(csctfs_eta)<2.1&&csctfs_bx==0)),99.99)>>hCSCTF1p6to2p1BX0(100,0,100)");
  makeRate(hCSCTF1p6to2p1BX0);
  bg->Draw("min(Max$(gmts_pt*(abs(gmts_eta)>=1.6&&abs(gmts_eta)<2.1&&gmts_bx==0&&gmts_quality>=4)),99.99)>>hCSCTF1p6to2p1BX0Q4(100,0,100)");
  makeRate(hCSCTF1p6to2p1BX0Q4);
  bg->Draw("min(Max$(csctfs_pt*(abs(csctfs_eta)>=1.6&&abs(csctfs_eta)<2.1&&csctfs_bx==0&&csctfs_nlcts>=2&&csctfs_ringS1>0)),99.99)>>hCSCTF1p6to2p1BX0NL2ME1(100,0,100)");
  makeRate(hCSCTF1p6to2p1BX0NL2ME1);
  bg->Draw("min(Max$(csctfs_pt*(abs(csctfs_eta)>=1.6&&abs(csctfs_eta)<2.1&&csctfs_bx==0&&csctfs_nlcts>=3&&csctfs_ringS1>0)),99.99)>>hCSCTF1p6to2p1BX0NL3ME1(100,0,100)");
  makeRate(hCSCTF1p6to2p1BX0NL3ME1);
  bg->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.6&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.1&&csctfs_bx[tts_csctf_deps2_idx]==0)),99.99)>>hTTCSCTF1p6to2p1BX0(100,0,100)");
  makeRate(hTTCSCTF1p6to2p1BX0);
  bg->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.6&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.1&&csctfs_bx[tts_csctf_deps2_idx]==0&&csctfs_nlcts[tts_csctf_deps2_idx]>=2&&csctfs_ringS1[tts_csctf_deps2_idx]>0)),99.99)>>hTTCSCTF1p6to2p1BX0NL2ME1(100,0,100)");
  makeRate(hTTCSCTF1p6to2p1BX0NL2ME1,30E3, true, true);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);
  

  hCSCTF1p6to2p1_Rate->Draw();
  hCSCTF1p6to2p1_Rate->SetTitle("Single #mu rate using CSCTF p_{T} covered by GE1/1 1.6#leq|#eta|<2.1");
  hCSCTF1p6to2p1_Rate->GetXaxis()->SetTitle("p_{T}, GeV");
  hCSCTF1p6to2p1_Rate->SetLineWidth(2);
  hCSCTF1p6to2p1_Rate->SetLineColor(1);
  gPad->SetLogy(1);
  hCSCTF1p6to2p1_Rate->SetMinimum(0.1);

  hCSCTF1p6to2p1BX0_Rate->SetLineWidth(2);
  hCSCTF1p6to2p1BX0_Rate->SetLineColor(2);
  hCSCTF1p6to2p1BX0_Rate->Draw("same");

  hCSCTF1p6to2p1BX0Q4_Rate->SetLineWidth(2);
  hCSCTF1p6to2p1BX0Q4_Rate->SetLineColor(3);
  hCSCTF1p6to2p1BX0Q4_Rate->Draw("same");

  hCSCTF1p6to2p1BX0NL2ME1_Rate->SetLineWidth(2);
  hCSCTF1p6to2p1BX0NL2ME1_Rate->SetLineColor(4);
  hCSCTF1p6to2p1BX0NL2ME1_Rate->Draw("same");

  hCSCTF1p6to2p1BX0NL3ME1_Rate->SetLineWidth(2);
  hCSCTF1p6to2p1BX0NL3ME1_Rate->SetLineColor(6);
  hCSCTF1p6to2p1BX0NL3ME1_Rate->Draw("same");

  hTTCSCTF1p6to2p1BX0_Rate->SetLineWidth(2);
  hTTCSCTF1p6to2p1BX0_Rate->SetLineColor(7);
  hTTCSCTF1p6to2p1BX0_Rate->Draw("same");

  hTTCSCTF1p6to2p1BX0NL2ME1_Rate->SetLineWidth(2);
  hTTCSCTF1p6to2p1BX0NL2ME1_Rate->SetLineColor(6);
  hTTCSCTF1p6to2p1BX0NL2ME1_Rate->Draw("same");

  TLegend* leg = new TLegend(0.4, 0.7, 0.9, 0.9);
  leg->SetFillColor(0);
  leg->AddEntry(hCSCTF1p6to2p1_Rate, "All CSCTFs");
  leg->AddEntry(hCSCTF1p6to2p1BX0_Rate, "CSCTFs BX=0");
  leg->AddEntry(hCSCTF1p6to2p1BX0Q4_Rate, "GMTs BX=0 GMT Quality#geq4");
  leg->AddEntry(hCSCTF1p6to2p1BX0NL2ME1_Rate, "CSCTFs BX=0 N_{LCT}#geq2 w ME1");
  leg->AddEntry(hCSCTF1p6to2p1BX0NL3ME1_Rate, "CSCTFs BX=0 N_{LCT}#geq3 w ME1");
  leg->AddEntry(hTTCSCTF1p6to2p1BX0_Rate, "TTs: matching to CSCTF BX=0");
  leg->Draw();
  gPad->SaveAs("cms/emu/June1462X/hCSCTF1p6to2p1_rate.png");
  gPad->SaveAs("cms/emu/June1462X/hCSCTF1p6to2p1_rate.pdf");
  gPad->SaveAs("cms/emu/June1462X/hCSCTF1p6to2p1_rate.eps");

  //less than GE1/1 range: 1.2< eta < 1.6
  bg->Draw("min(Max$(csctfs_pt*(abs(csctfs_eta)>=1.2&&abs(csctfs_eta)<1.6)),99.99)>>hCSCTF1p2to1p6(100,0,100)");
  makeRate(hCSCTF1p2to1p6);
  bg->Draw("min(Max$(csctfs_pt*(abs(csctfs_eta)>=1.2&&abs(csctfs_eta)<1.6&&csctfs_bx==0)),99.99)>>hCSCTF1p2to1p6BX0(100,0,100)");
  makeRate(hCSCTF1p2to1p6BX0);
  bg->Draw("min(Max$(gmts_pt*(abs(gmts_eta)>=1.2&&abs(gmts_eta)<1.6&&gmts_bx==0&&gmts_quality>=4)),99.99)>>hCSCTF1p2to1p6BX0Q4(100,0,100)");
  makeRate(hCSCTF1p2to1p6BX0Q4);
  bg->Draw("min(Max$(csctfs_pt*(abs(csctfs_eta)>=1.2&&abs(csctfs_eta)<1.6&&csctfs_bx==0&&csctfs_nlcts>=2&&csctfs_ringS1>0)),99.99)>>hCSCTF1p2to1p6BX0NL2ME1(100,0,100)");
  makeRate(hCSCTF1p2to1p6BX0NL2ME1);
  bg->Draw("min(Max$(csctfs_pt*(abs(csctfs_eta)>=1.2&&abs(csctfs_eta)<1.6&&csctfs_bx==0&&csctfs_nlcts>=3&&csctfs_ringS1>0)),99.99)>>hCSCTF1p2to1p6BX0NL3ME1(100,0,100)");
  makeRate(hCSCTF1p2to1p6BX0NL3ME1);
  bg->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.2&&abs(csctfs_eta[tts_csctf_deps2_idx])<1.6&&csctfs_bx[tts_csctf_deps2_idx]==0)),99.99)>>hTTCSCTF1p2to1p6BX0(100,0,100)");
  makeRate(hTTCSCTF1p2to1p6BX0);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);
  

  hCSCTF1p2to1p6_Rate->Draw();
  hCSCTF1p2to1p6_Rate->SetTitle("Single #mu rate using CSCTF p_{T} not covered by GE1/1 1.2#leq|#eta|<1.6");
  hCSCTF1p2to1p6_Rate->GetXaxis()->SetTitle("p_{T}, GeV");
  hCSCTF1p2to1p6_Rate->SetLineWidth(2);
  hCSCTF1p2to1p6_Rate->SetLineColor(1);
  gPad->SetLogy(1);
  hCSCTF1p2to1p6_Rate->SetMinimum(0.1);

  hCSCTF1p2to1p6BX0_Rate->SetLineWidth(2);
  hCSCTF1p2to1p6BX0_Rate->SetLineColor(2);
  hCSCTF1p2to1p6BX0_Rate->Draw("same");

  hCSCTF1p2to1p6BX0Q4_Rate->SetLineWidth(2);
  hCSCTF1p2to1p6BX0Q4_Rate->SetLineColor(3);
  hCSCTF1p2to1p6BX0Q4_Rate->Draw("same");

  hCSCTF1p2to1p6BX0NL2ME1_Rate->SetLineWidth(2);
  hCSCTF1p2to1p6BX0NL2ME1_Rate->SetLineColor(4);
  hCSCTF1p2to1p6BX0NL2ME1_Rate->Draw("same");

  hCSCTF1p2to1p6BX0NL3ME1_Rate->SetLineWidth(2);
  hCSCTF1p2to1p6BX0NL3ME1_Rate->SetLineColor(6);
  hCSCTF1p2to1p6BX0NL3ME1_Rate->Draw("same");

  hTTCSCTF1p2to1p6BX0_Rate->SetLineWidth(2);
  hTTCSCTF1p2to1p6BX0_Rate->SetLineColor(7);
  hTTCSCTF1p2to1p6BX0_Rate->Draw("same");

  TLegend* leg = new TLegend(0.4, 0.7, 0.9, 0.9);
  leg->SetFillColor(0);
  leg->AddEntry(hCSCTF1p2to1p6_Rate, "All CSCTFs");
  leg->AddEntry(hCSCTF1p2to1p6BX0_Rate, "CSCTFs BX=0");
  leg->AddEntry(hCSCTF1p2to1p6BX0Q4_Rate, "GMTs BX=0 GMT Quality#geq4");
  leg->AddEntry(hCSCTF1p2to1p6BX0NL2ME1_Rate, "CSCTFs BX=0 N_{LCT}#geq2 w ME1");
  leg->AddEntry(hCSCTF1p2to1p6BX0NL3ME1_Rate, "CSCTFs BX=0 N_{LCT}#geq3 w ME1");
  leg->AddEntry(hTTCSCTF1p2to1p6BX0_Rate, "TTs: matching to CSCTF BX=0");
  leg->Draw();
  gPad->SaveAs("cms/emu/June1462X/hCSCTF1p2to1p6_rate.png");
  gPad->SaveAs("cms/emu/June1462X/hCSCTF1p2to1p6_rate.pdf");
  gPad->SaveAs("cms/emu/June1462X/hCSCTF1p2to1p6_rate.eps");

  //Now just TTs with some variations
  bg->Draw("min(Max$(tts_pt*(tts_csctf_drs2_idx>=0&&csctfs_tt_drs2_idx[tts_csctf_drs2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_drs2_idx])>=1.2&&abs(csctfs_eta[tts_csctf_drs2_idx])<2.4&&csctfs_bx[tts_csctf_drs2_idx]==0)),99.99)>>hTTCSCTF1p2to2p4BX0TTdr(100,0,100)");
  makeRate(hTTCSCTF1p2to2p4BX0TTdr,30E3, true, true);
  bg->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.2&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.4&&csctfs_bx[tts_csctf_deps2_idx]==0)),99.99)>>hTTCSCTF1p2to2p4BX0TTep(100,0,100)");
  makeRate(hTTCSCTF1p2to2p4BX0TTep,30E3, true, true);

  //baseline
  bg->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.2&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.4&&csctfs_bx[tts_csctf_deps2_idx]==0)),99.99)>>hTTCSCTF1p2to2p4BX0(100,0,100)");
  makeRate(hTTCSCTF1p2to2p4BX0,30E3, true, true);
  //baseline 1.1 to 2.4
  bg->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.1&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.4&&csctfs_bx[tts_csctf_deps2_idx]==0)),99.99)>>hTTCSCTF1p1to2p4BX0(100,0,100)");
  makeRate(hTTCSCTF1p1to2p4BX0,30E3, true, true);
  //baseline 1.1 to 2.2
  bg->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.1&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.2&&csctfs_bx[tts_csctf_deps2_idx]==0)),99.99)>>hTTCSCTF1p1to2p2BX0(100,0,100)");
  makeRate(hTTCSCTF1p1to2p2BX0,30E3, true, true);
  //baseline from gmt 0 to 1.1
  bg->Draw("min(Max$(tts_pt*(tts_gmt_deps2_idx>=0&&gmts_tt_deps2_idx[tts_gmt_deps2_idx]==Iteration$&&abs(gmts_eta[tts_gmt_deps2_idx])<=1.1&&gmts_bx[tts_gmt_deps2_idx]==0)),99.99)>>hTTGMT0to1p1BX0(100,0,100)");
  makeRate(hTTGMT0to1p1BX0,30E3, true, true);


  //baseline with YE1
  bg->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.2&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.4&&csctfs_bx[tts_csctf_deps2_idx]==0&&csctfs_nlcts[tts_csctf_deps2_idx]>=2&&csctfs_ringS1[tts_csctf_deps2_idx]>0)),99.99)>>hTTCSCTF1p2to2p4BX0NL2ME1(100,0,100)");
  makeRate(hTTCSCTF1p2to2p4BX0NL2ME1,30E3, true, true);
  //phase2 baseline
  bgp2->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.2&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.4&&csctfs_bx[tts_csctf_deps2_idx]==0)),99.99)>>hp2TTCSCTF1p2to2p4BX0(100,0,100)");
  makeRate(hp2TTCSCTF1p2to2p4BX0,30E3, true, true);
  //
  bgp2->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.2&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.4&&csctfs_bx[tts_csctf_deps2_idx]==0&&csctfs_nlcts[tts_csctf_deps2_idx]>=3)),99.99)>>hp2TTCSCTF1p2to2p4BX0NL3(100,0,100)");
  makeRate(hp2TTCSCTF1p2to2p4BX0NL3,30E3, true, true);
  //phase2 baseline with YE1
  bgp2->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.2&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.4&&csctfs_bx[tts_csctf_deps2_idx]==0&&csctfs_nlcts[tts_csctf_deps2_idx]>=2&&csctfs_ringS1[tts_csctf_deps2_idx]>0)),99.99)>>hp2TTCSCTF1p2to2p4BX0NL2ME1(100,0,100)");
  makeRate(hp2TTCSCTF1p2to2p4BX0NL2ME1,30E3, true, true);
  //phase2 baseline with YE1
  bgp2->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.2&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.4&&csctfs_bx[tts_csctf_deps2_idx]==0&&csctfs_nlcts[tts_csctf_deps2_idx]>=2&&csctfs_ringS1[tts_csctf_deps2_idx]>0\
&&(abs(csctfs_gemDPhiS1[tts_csctf_deps2_idx])>2||abs(csctfs_eta[tts_csctf_deps2_idx])>2.05||((-tts_q*csctfs_gemDPhiS1[tts_csctf_deps2_idx]>-2E-3&&-tts_q*csctfs_gemDPhiS1[tts_csctf_deps2_idx]<sqrt((0.05/tts_pt)**2 + 4E-6))&&csctfs_chamberS1[tts_csctf_deps2_idx]%2==0 )||((-tts_q*csctfs_gemDPhiS1[tts_csctf_deps2_idx]>-3.3E-3&&-tts_q*csctfs_gemDPhiS1[tts_csctf_deps2_idx]<sqrt((0.12/tts_pt)**2 + 1.E-5))&&csctfs_chamberS1[tts_csctf_deps2_idx]%2==1 ) )\
  )),99.99)>>hp2TTCSCTF1p2to2p4BX0NL2ME1dPhi(100,0,100)");
  makeRate(hp2TTCSCTF1p2to2p4BX0NL2ME1dPhi,30E3, true, true);
  //&&(abs(csctfs_gemDPhiS1[tts_csctf_deps2_idx])>2||( (-tts_q*csctfs_gemDPhiS1[tts_csctf_deps2_idx]>-2E-3 && -tts_q*csctfs_gemDPhiS1[tts_csctf_deps2_idx]<sqrt((0.12/tts_pt)**2 + 5E-6) )|| abs(csctfs_gemDPhiS1[tts_csctf_deps2_idx])<min(1E-4*tts_pt**2,0.1) ) ) \
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);
  

  hTTCSCTF1p2to2p4BX0TTdr_Rate->Draw();
  hTTCSCTF1p2to2p4BX0TTdr_Rate->SetTitle("Single #mu rate using TT p_{T} matching to CSCTF 1.2#leq|#eta|<2.4");
  hTTCSCTF1p2to2p4BX0TTdr_Rate->GetXaxis()->SetTitle("p_{T}, GeV");
  hTTCSCTF1p2to2p4BX0TTdr_Rate->SetLineWidth(2);
  hTTCSCTF1p2to2p4BX0TTdr_Rate->SetLineColor(1);
  gPad->SetLogy(1);
  hTTCSCTF1p2to2p4BX0TTdr_Rate->SetMinimum(0.1);

  hTTCSCTF1p2to2p4BX0TTep_Rate->SetLineWidth(2);
  hTTCSCTF1p2to2p4BX0TTep_Rate->SetLineColor(2);
  hTTCSCTF1p2to2p4BX0TTep_Rate->Draw("same");

  hTTCSCTF1p2to2p4BX0NL2ME1_Rate->SetLineWidth(2);
  hTTCSCTF1p2to2p4BX0NL2ME1_Rate->SetLineColor(6);
  hTTCSCTF1p2to2p4BX0NL2ME1_Rate->Draw("same");

  hTTCSCTF1p2to2p4BX0_Rate->SetLineWidth(2);
  hTTCSCTF1p2to2p4BX0_Rate->SetLineColor(7);
  hTTCSCTF1p2to2p4BX0_Rate->Draw("same");

  TLegend* leg = new TLegend(0.4, 0.7, 0.9, 0.9);
  leg->SetFillColor(0);
  leg->AddEntry(hTTCSCTF1p2to2p4BX0TTdr_Rate, "Closest in #DeltaR_{CORR} (#Delta R< 0.5)");
  leg->AddEntry(hTTCSCTF1p2to2p4BX0TTep_Rate, "In #Delta#eta#Delta#phi window");
  leg->AddEntry(hTTCSCTF1p2to2p4BX0_Rate, "Baseline: closest in #DeltaR_{CORR} and in #Delta#eta#Delta#phi window");
  leg->AddEntry(hTTCSCTF1p2to2p4BX0NL2ME1_Rate, "Baseline with CSCTF N_{LCT}#geq2 w ME1");
  leg->Draw();
  gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p2to2p4BX0TTdr_rate.png");
  gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p2to2p4BX0TTdr_rate.pdf");
  gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p2to2p4BX0TTdr_rate.eps");

  //summary
  hTTCSCTF1p2to2p4BX0TTdr_Rate->Draw();
  hTTCSCTF1p2to2p4BX0_Rate->Draw("same");

  TLegend* leg = new TLegend(0.4, 0.7, 0.9, 0.9);
  leg->SetFillColor(0);
  leg->AddEntry(hTTCSCTF1p2to2p4BX0TTdr_Rate, "Closest in #DeltaR_{CORR} (#Delta R< 0.5)");
  leg->AddEntry(hTTCSCTF1p2to2p4BX0_Rate, "Baseline: closest in #DeltaR_{CORR} and in #Delta#eta#Delta#phi window");
  leg->Draw();
  gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p2to2p4BX0TTdr_reduced_rate.png");
  gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p2to2p4BX0TTdr_reduced_rate.pdf");
  gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p2to2p4BX0TTdr_reduced_rate.eps");


  //summary
  hp2TTCSCTF1p2to2p4BX0_Rate->SetLineWidth(2);
  hp2TTCSCTF1p2to2p4BX0NL2ME1_Rate->SetLineWidth(2);
  hp2TTCSCTF1p2to2p4BX0NL2ME1dPhi_Rate->SetLineWidth(2);
  hp2TTCSCTF1p2to2p4BX0_Rate->SetLineColor(4);
  hp2TTCSCTF1p2to2p4BX0NL3_Rate->SetLineColor(3);
  hp2TTCSCTF1p2to2p4BX0NL2ME1_Rate->SetLineColor(3);
  hp2TTCSCTF1p2to2p4BX0NL2ME1dPhi_Rate->SetLineColor(2);

  hTTCSCTF1p2to2p4BX0_Rate->SetTitle("Single #mu rate using TT p_{T} matching to CSCTF 1.2#leq|#eta|<2.4");
  hTTCSCTF1p2to2p4BX0_Rate->GetXaxis()->SetTitle("p_{T}, GeV");
  hTTCSCTF1p2to2p4BX0_Rate->Draw();
  hTTCSCTF1p2to2p4BX0NL2ME1_Rate->Draw("same");
  hp2TTCSCTF1p2to2p4BX0_Rate->Draw("same");
  hp2TTCSCTF1p2to2p4BX0NL2ME1_Rate->Draw("same");
  hp2TTCSCTF1p2to2p4BX0NL2ME1dPhi_Rate->Draw("same");



  TLegend* leg = new TLegend(0.4, 0.7, 0.9, 0.9);
  leg->SetFillColor(0);
  leg->AddEntry(hTTCSCTF1p2to2p4BX0_Rate, "Baseline: closest in #DeltaR_{CORR} and in #Delta#eta#Delta#phi window");
  leg->AddEntry(hTTCSCTF1p2to2p4BX0NL2ME1_Rate, "Baseline with ME1 required");
  leg->AddEntry(hp2TTCSCTF1p2to2p4BX0_Rate, "Phase2 Baseline");
  leg->AddEntry(hp2TTCSCTF1p2to2p4BX0NL2ME1_Rate, "Phase2 Baseline with ME1 required");
  leg->AddEntry(hp2TTCSCTF1p2to2p4BX0NL2ME1dPhi_Rate, "Phase2 Baseline with ME1 angle (*)");
  leg->Draw();
  gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p2to2p4_base_wME1_rate.png");
  gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p2to2p4_base_wME1_rate.pdf");
  gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p2to2p4_base_wME1_rate.eps");
  gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p2to2p4_base_wME1_rate.root");

  //summary

  hTTCSCTF1p6to2p1BX0_Rate->SetTitle("Single #mu rate using TT p_{T} matching to CSCTF 1.6#leq|#eta|<2.1");
  hTTCSCTF1p6to2p1BX0_Rate->GetXaxis()->SetTitle("p_{T}, GeV");
  hTTCSCTF1p6to2p1BX0_Rate->Draw();
  hTTCSCTF1p6to2p1BX0NL2ME1_Rate->Draw("same");

  TLegend* leg = new TLegend(0.4, 0.7, 0.9, 0.9);
  leg->SetFillColor(0);
  leg->AddEntry(hTTCSCTF1p6to2p1BX0_Rate, "Baseline: closest in #DeltaR_{CORR} and in #Delta#eta#Delta#phi window");
  leg->AddEntry(hTTCSCTF1p6to2p1BX0NL2ME1_Rate, "Baseline with ME1 required");
  leg->Draw();
  gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p6to2p1_base_wME1_rate.png");
  gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p6to2p1_base_wME1_rate.pdf");
  gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p6to2p1_base_wME1_rate.eps");
  gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p6to2p1_base_wME1_rate.root");

  
}

void redrawTTPlots(int version){
  TFile* fTF1p2to2p4 = new TFile("cms/emu/June1462X/hTTCSCTF1p2to2p4_base_wME1_rate.root");
  TCanvas* c = (TCanvas*)fTF1p2to2p4->Get("c1");
  TList* pl = c->GetListOfPrimitives();

  TH1F* hTTCSCTF1p2to2p4BX0_Rate = (TH1F*)pl->FindObject("hTTCSCTF1p2to2p4BX0_Rate");
  TH1F* hTTCSCTF1p2to2p4BX0NL2ME1_Rate = (TH1F*)pl->FindObject("hTTCSCTF1p2to2p4BX0NL2ME1_Rate");
  TH1F* hp2TTCSCTF1p2to2p4BX0_Rate = (TH1F*)pl->FindObject("hp2TTCSCTF1p2to2p4BX0_Rate");
  TH1F* hp2TTCSCTF1p2to2p4BX0NL2ME1_Rate = (TH1F*)pl->FindObject("hp2TTCSCTF1p2to2p4BX0NL2ME1_Rate");
  TH1F* hp2TTCSCTF1p2to2p4BX0NL2ME1dPhi_Rate = (TH1F*)pl->FindObject("hp2TTCSCTF1p2to2p4BX0NL2ME1dPhi_Rate");


  hTTCSCTF1p2to2p4BX0_Rate->SetFillColor(hTTCSCTF1p2to2p4BX0_Rate->GetLineColor());
  hTTCSCTF1p2to2p4BX0NL2ME1_Rate->SetFillColor(hTTCSCTF1p2to2p4BX0NL2ME1_Rate->GetLineColor());
  hp2TTCSCTF1p2to2p4BX0_Rate->SetFillColor(hp2TTCSCTF1p2to2p4BX0_Rate->GetLineColor());
  hp2TTCSCTF1p2to2p4BX0NL2ME1_Rate->SetFillColor(hp2TTCSCTF1p2to2p4BX0NL2ME1_Rate->GetLineColor());
  hp2TTCSCTF1p2to2p4BX0NL2ME1dPhi_Rate->SetFillColor(hp2TTCSCTF1p2to2p4BX0NL2ME1dPhi_Rate->GetLineColor());


  cL = new TCanvas("cL","cL",700,450);
  cL->Clear();
  gStyle->SetTitleSize(0.055, "");
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleAlign(13); // coord in top left
  gStyle->SetTitleX(0.);
  gStyle->SetTitleY(1.);
  gStyle->SetTitleW(1);
  gStyle->SetTitleH(0.06);
  gStyle->SetTitleBorderSize(0);
  
  gStyle->SetPadLeftMargin(0.126);
  gStyle->SetPadRightMargin(0.04);
  gStyle->SetPadTopMargin(0.065);
  gStyle->SetPadBottomMargin(0.13);
  gStyle->SetOptStat(0);
  gStyle->SetMarkerStyle(1);

  addSystUncty(hTTCSCTF1p2to2p4BX0_Rate, 0.05);
  addSystUncty(hTTCSCTF1p2to2p4BX0NL2ME1_Rate, 0.05);
  addSystUncty(hp2TTCSCTF1p2to2p4BX0_Rate, 0.05);
  addSystUncty(hp2TTCSCTF1p2to2p4BX0NL2ME1_Rate, 0.05);
  addSystUncty(hp2TTCSCTF1p2to2p4BX0NL2ME1dPhi_Rate, 0.05);

  if (version == 0){
    c->cd();
    hTTCSCTF1p2to2p4BX0_Rate->Draw("e3");
    gPad->SetLogy();
    TH1F* h = hTTCSCTF1p2to2p4BX0_Rate;
    h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
    h->SetTitleSize(0.05);
    h->SetTitleOffset(1.1, "X");
    h->SetTitleOffset(0.84, "Y");
    h->GetXaxis()->SetLabelSize(0.05);
    h->GetYaxis()->SetLabelSize(0.05);
    h->GetXaxis()->SetTitleSize(0.055);
    h->GetYaxis()->SetTitleSize(0.06);
    h->SetXTitle("L1TkMu p_{T} [GeV]");
    h->SetYTitle("Trigger rate [kHz]");
    h->GetXaxis()->SetNdivisions(302);
    h->GetXaxis()->SetMoreLogLabels();
    h->SetMinimum(0.101);
    tex0 = new TLatex(10, 0.036, "10");
    tex0->SetTextFont(42);
    tex0->SetTextSize(0.05);
    tex0->Draw();
    tp = new TPave(0.4, 0.83, 0.95, 0.9, 0, "NDC"); tp->SetFillColor(0);
    tp->Draw();
    tex1 = new TLatex(.41,.85,"L1TkMu: 1.2<|#eta|<2.4 and #geq 2 CSCTF stubs");
    tex1->SetTextFont(42);
    tex1->SetTextSize(0.048);
    tex1->SetNDC();
    tex1->Draw();
    
    gPad->SetLogx();
    gPad->SetGridx();
    gPad->SetGridy();
    hTTCSCTF1p2to2p4BX0NL2ME1_Rate->Draw("same e3");
    //  hp2TTCSCTF1p2to2p4BX0_Rate->Draw("same e3");
    //  hp2TTCSCTF1p2to2p4BX0NL3_Rate->Draw("same e3");
    hp2TTCSCTF1p2to2p4BX0NL2ME1dPhi_Rate->Draw("same e3");
    
    
    TLegend* leg = new TLegend(0.4, 0.7, 0.95, 0.84);
    leg->SetFillColor(0); leg->SetBorderSize(0);
    
    leg->SetTextSize(0.040); leg->SetMargin(0.13);
    leg->AddEntry(hTTCSCTF1p2to2p4BX0_Rate, "Phase-1 muon");
    leg->AddEntry(hTTCSCTF1p2to2p4BX0NL2ME1_Rate, "Phase-1 muon with YE-1 required");
    //    leg->AddEntry(hp2TTCSCTF1p2to2p4BX0_Rate, "Phase-2 muon");
    //    leg->AddEntry(hp2TTCSCTF1p2to2p4BX0NL3_Rate, "Phase-2 muon 3+stubs");
    leg->AddEntry(hp2TTCSCTF1p2to2p4BX0NL2ME1dPhi_Rate, "Phase-2 muon with YE-1 angle cut (*)");
    leg->Draw();
    
    gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p2to2p4_base_wME1_rate_restyled_3opts.png");
    gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p2to2p4_base_wME1_rate_restyled_3opts.pdf");
    gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p2to2p4_base_wME1_rate_restyled_3opts.eps");

  }
  else if (version == 1) {
    c->cd();    
    hp2TTCSCTF1p2to2p4BX0_Rate->Draw("e3");
    gPad->SetLogy();
    TH1F* h = hp2TTCSCTF1p2to2p4BX0_Rate;
    h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
    h->SetTitleSize(0.05);
    h->SetTitleOffset(1.1, "X");
    h->SetTitleOffset(0.84, "Y");
    h->GetXaxis()->SetLabelSize(0.05);
    h->GetYaxis()->SetLabelSize(0.05);
    h->GetXaxis()->SetTitleSize(0.055);
    h->GetYaxis()->SetTitleSize(0.06);
    h->SetXTitle("L1TkMu p_{T} [GeV]");
    h->SetYTitle("Trigger rate [kHz]");
    h->GetXaxis()->SetNdivisions(302);
    h->GetXaxis()->SetMoreLogLabels();
    h->SetMinimum(0.101);
    tex0 = new TLatex(10, 0.036, "10");
    tex0->SetTextFont(42);
    tex0->SetTextSize(0.05);
    tex0->Draw();
    tp = new TPave(0.4, 0.83, 0.95, 0.9, 0, "NDC"); tp->SetFillColor(0);
    tp->Draw();
    tex1 = new TLatex(.41,.85,"L1TkMu: 1.2<|#eta|<2.4 and #geq 2 CSCTF stubs");
    tex1->SetTextFont(42);
    tex1->SetTextSize(0.048);
    tex1->SetNDC();
    tex1->Draw();
    
    gPad->SetLogx();
    gPad->SetGridx();
    gPad->SetGridy();
    //    hTTCSCTF1p2to2p4BX0NL2ME1_Rate->Draw("same e3");
    //    hp2TTCSCTF1p2to2p4BX0_Rate->Draw("same e3");
    //    hp2TTCSCTF1p2to2p4BX0NL2ME1_Rate->Draw("same e3");
    hp2TTCSCTF1p2to2p4BX0NL2ME1dPhi_Rate->Draw("same e3");
    
    
    TLegend* leg = new TLegend(0.4, 0.74, 0.95, 0.84);
    leg->SetFillColor(0); leg->SetBorderSize(0);
    
    leg->SetTextSize(0.040); leg->SetMargin(0.13);
    //    leg->AddEntry(hTTCSCTF1p2to2p4BX0_Rate, "Phase-1 muon");
    //    leg->AddEntry(hTTCSCTF1p2to2p4BX0NL2ME1_Rate, "Phase-1 muon with YE-1 required");
    leg->AddEntry(hp2TTCSCTF1p2to2p4BX0_Rate, "Phase-2 muon");
    //    leg->AddEntry(hp2TTCSCTF1p2to2p4BX0NL2ME1_Rate, "Phase-2 muon with YE-1 required");
    leg->AddEntry(hp2TTCSCTF1p2to2p4BX0NL2ME1dPhi_Rate, "Phase-2 muon with MS 1 bending angle cut");
    leg->Draw();
    
    gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p2to2p4_base_wME1_rate_restyled_version1.png");
    gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p2to2p4_base_wME1_rate_restyled_version1.pdf");
    gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p2to2p4_base_wME1_rate_restyled_version1.eps");
  } else if (version == 2){
    c->cd();    
    hTTCSCTF1p2to2p4BX0_Rate->Draw("e3");
    gPad->SetLogy();
    TH1F* h = hTTCSCTF1p2to2p4BX0_Rate;
    h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
    h->SetTitleSize(0.05);
    h->SetTitleOffset(1.1, "X");
    h->SetTitleOffset(0.84, "Y");
    h->GetXaxis()->SetLabelSize(0.05);
    h->GetYaxis()->SetLabelSize(0.05);
    h->GetXaxis()->SetTitleSize(0.055);
    h->GetYaxis()->SetTitleSize(0.06);
    h->SetXTitle("L1TkMu p_{T} [GeV]");
    h->SetYTitle("Trigger rate [kHz]");
    h->GetXaxis()->SetNdivisions(302);
    h->GetXaxis()->SetMoreLogLabels();
    h->SetMinimum(0.101);
    tex0 = new TLatex(10, 0.036, "10");
    tex0->SetTextFont(42);
    tex0->SetTextSize(0.05);
    tex0->Draw();
    tp = new TPave(0.4, 0.86, 0.95, 0.92, 0, "NDC"); tp->SetFillColor(0);
    tp->Draw();
    tex1 = new TLatex(.41,.88,"L1TkMu: 1.2<|#eta|<2.4 and #geq 2 CSCTF stubs");
    tex1->SetTextFont(42);
    tex1->SetTextSize(0.048);
    tex1->SetNDC();
    tex1->Draw();
    
    gPad->SetLogx();
    gPad->SetGridx();
    gPad->SetGridy();
    hTTCSCTF1p2to2p4BX0NL2ME1_Rate->Draw("same e3");
    hp2TTCSCTF1p2to2p4BX0_Rate->Draw("same e3");
    hp2TTCSCTF1p2to2p4BX0NL2ME1_Rate->Draw("same e3");
    hp2TTCSCTF1p2to2p4BX0NL2ME1dPhi_Rate->Draw("same e3");
    
    
    TLegend* leg = new TLegend(0.4, 0.71, 0.95, 0.87);
    leg->SetFillColor(0); leg->SetBorderSize(0);
    
    leg->SetTextSize(0.040); leg->SetMargin(0.13);
    leg->AddEntry(hTTCSCTF1p2to2p4BX0_Rate, "Phase-1 muon");
    leg->AddEntry(hTTCSCTF1p2to2p4BX0NL2ME1_Rate, "Phase-1 muon with YE-1 required");
    leg->AddEntry(hp2TTCSCTF1p2to2p4BX0_Rate, "Phase-2 muon");
    leg->AddEntry(hp2TTCSCTF1p2to2p4BX0NL2ME1_Rate, "Phase-2 muon with YE-1 required");
    leg->AddEntry(hp2TTCSCTF1p2to2p4BX0NL2ME1dPhi_Rate, "Phase-2 muon with YE-1 bending angle cut");
    leg->Draw();
    
    gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p2to2p4_base_wME1_rate_restyled_version2.png");
    gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p2to2p4_base_wME1_rate_restyled_version2.pdf");
    gPad->SaveAs("cms/emu/June1462X/hTTCSCTF1p2to2p4_base_wME1_rate_restyled_version2.eps");

  }
  
}

void getL1TkMuVSL1MuRates(){
  gStyle->SetTitleSize(0.055, "");
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleAlign(13); // coord in top left
  gStyle->SetTitleX(0.);
  gStyle->SetTitleY(1.);
  gStyle->SetTitleW(1);
  gStyle->SetTitleH(0.06);
  gStyle->SetTitleBorderSize(0);
  
  gStyle->SetPadLeftMargin(0.126);
  gStyle->SetPadRightMargin(0.04);
  gStyle->SetPadTopMargin(0.065);
  gStyle->SetPadBottomMargin(0.13);
  gStyle->SetOptStat(0);
  gStyle->SetMarkerStyle(1);

  TChain* bg = new TChain("Events");
  bg->Add("/Data/cms/emu/phase2/CMSSW_6_2_0_SLHC14-ttiRedigiJun14ana/Extended2023TTI_FwdMuons/Neutrinos/runTTAnaMakers_redoL1MuPhase1_nugun/tt_ntuple_redoL1MuPhase1_*.root");

  TChain* bgp2 = new TChain("Events");
  bgp2->Add("/Data/cms/emu/phase2/CMSSW_6_2_0_SLHC14-ttiRedigiJun14ana/Extended2023TTI_FwdMuons/Neutrinos/runTTAnaMakers_redoL1MuPhase2_nugun/tt_ntuple_redoL1MuPhase2_*.root");

  TChain* bgp2v13 = new TChain("Events");
  bgp2v13->Add("/Data/cms/emu/phase2/CMSSW_6_2_0_SLHC12/Extended2023TTI_FwdMuons/Neutrinos/tt_ntuple_redoL1Mu_*.root");

  TChain* bgw = new TChain("Events");
  bgw->Add("/Data/cms/emu/phase2/CMSSW_6_2_0_SLHC14-ttiRedigiJun14ana/WTomunu_TuneZ2star_14TeV_pythia6/TTI2023Upg14D-PU140bx25-ILT_SLHC14-ttnt-redoL1MuPhase1/5cac9a74090da3b2068f65a6d15fa6ca/tt_ntuple_redoL1MuPhase1_*.root");

  //in this version the match between gmts and csctfs is broken for eta< 1.6 due to differences in eta scales
  bg->Draw("min(Max$(gmts_pt*(abs(gmts_eta)>=1.1&&abs(gmts_eta)<2.2\
&&gmts_bx==0\
&&gmts_quality>=4&&gmts_quality!=5)),49.99)>>hL1Mu1p1to2p2Baseline(45,5,50)");
  makeRate(hL1Mu1p1to2p2Baseline,30E3, true, true);

  bg->Draw("min(Max$(gmts_pt*(abs(gmts_eta)>=1.1&&abs(gmts_eta)<2.1\
&&gmts_bx==0\
&&gmts_quality>=4&&gmts_quality!=5)),49.99)>>hL1Mu1p1to2p1Baseline(45,5,50)");
  makeRate(hL1Mu1p1to2p1Baseline,30E3, true, true);

  bg->Draw("min(Max$(gmts_pt*(abs(gmts_eta)>1.64&&abs(gmts_eta)<2.14\
&&gmts_bx==0\
&&gmts_quality>=4&&gmts_quality!=5)),49.99)>>hL1Mu1p64to2p14Baseline(45,5,50)");
  makeRate(hL1Mu1p64to2p14Baseline,30E3, true, true);

  bg->Draw("min(Max$(gmts_pt*(abs(gmts_eta)>=1.1&&abs(gmts_eta)<2.4\
&&gmts_bx==0\
&&gmts_quality>=4&&gmts_quality!=5)),49.99)>>hL1Mu1p1to2p4Baseline(45,5,50)");
  makeRate(hL1Mu1p1to2p4Baseline,30E3, true, true);
  
  bg->Draw("min(Max$(gmts_pt*(abs(gmts_eta)<1.1\
&&gmts_bx==0\
&&gmts_quality>=4&&gmts_quality!=5)),49.99)>>hL1Mu0to1p1Baseline(45,5,50)");
  makeRate(hL1Mu0to1p1Baseline,30E3, true, true);


  bg->Draw("min(Max$(csctfs_pt*(abs(csctfs_eta)>1.64&&abs(csctfs_eta)<2.14\
&&csctfs_bx==0\
&&csctfs_nlcts>=2&&csctfs_ringS1>0)),49.99)>>hL1Mu1p64to2p14nS2w1(45,5,50)");
  makeRate(hL1Mu1p64to2p14nS2w1,30E3, true, true);


  bg->Draw("min(Max$(l1tkmus_pt*(abs(l1tkmus_eta)>=1.1&&abs(l1tkmus_eta)<2.2\
)),49.99)>>hL1TkMu1p1to2p2Baseline(45,5,50)");
  makeRate(hL1TkMu1p1to2p2Baseline,30E3, true, true);

  bg->Draw("min(Max$(l1tkmus_pt*(abs(l1tkmus_eta)>1.64&&abs(l1tkmus_eta)<2.14\
)),49.99)>>hL1TkMu1p64to2p14Baseline(45,5,50)");
  makeRate(hL1TkMu1p64to2p14Baseline,30E3, true, true);

  bg->Draw("min(Max$(l1tkmus_pt*(abs(l1tkmus_eta)>=1.1&&abs(l1tkmus_eta)<2.4\
)),49.99)>>hL1TkMu1p1to2p4Baseline(45,5,50)");
  makeRate(hL1TkMu1p1to2p4Baseline,30E3, true, true);

  bg->Draw("min(Max$(l1tkmus_pt*(abs(l1tkmus_eta)>=1.6&&abs(l1tkmus_eta)<2.1\
)),49.99)>>hL1TkMu1p6to2p1BaselineMuCh(45,5,50)");
  makeRate(hL1TkMu1p6to2p1BaselineMuCh,30E3, true, true);

  bg->Draw("min(Max$(l1tkmus_pt*(abs(l1tkmus_eta)>=1.6&&abs(l1tkmus_eta)<2.4\
)),49.99)>>hL1TkMu1p6to2p4BaselineMuCh(45,5,50)");
  makeRate(hL1TkMu1p6to2p4BaselineMuCh,30E3, true, true);

  bg->Draw("min(Max$(l1tkmus_pt*(abs(l1tkmus_eta)>=1.1&&abs(l1tkmus_eta)<2.4\
)),49.99)>>hL1TkMu1p1to2p4BaselineMuCh(45,5,50)");
  makeRate(hL1TkMu1p1to2p4BaselineMuCh,30E3, true, true);

  bg->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.1&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.4&&csctfs_bx[tts_csctf_deps2_idx]==0&&(csctfs_pt[tts_csctf_deps2_idx]>0.5*tts_pt||csctfs_pt[tts_csctf_deps2_idx]>20)) ),99.99)>>hL1TkMu1p1to2p4BaselineTF0p5PTor20MuChDeco(45,5,50)");
  makeRate(hL1TkMu1p1to2p4BaselineTF0p5PTor20MuChDeco,30E3, true, true);

  bgw->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.1&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.4&&csctfs_bx[tts_csctf_deps2_idx]==0&&(csctfs_pt[tts_csctf_deps2_idx]>0.5*tts_pt||csctfs_pt[tts_csctf_deps2_idx]>20)) ),99.99)>>hL1TkMu1p1to2p4BaselineTF0p5PTor20MuChDecoEWK(45,5,50)");
  makeRate(hL1TkMu1p1to2p4BaselineTF0p5PTor20MuChDecoEWK,1.15, true, true);
  

  bgp2->Draw("min(Max$(l1tkmus_pt*(abs(l1tkmus_eta)>=1.1&&abs(l1tkmus_eta)<2.4\
)),49.99)>>hp2L1TkMu1p1to2p4BaselineMuCh(45,5,50)");
  makeRate(hp2L1TkMu1p1to2p4BaselineMuCh,30E3, true, true);

  bgp2->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.1&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.4&&csctfs_bx[tts_csctf_deps2_idx]==0)),99.99)>>hp2L1TkMu1p1to2p4BaselineMuChDeco(45,5,50)");
  makeRate(hp2L1TkMu1p1to2p4BaselineMuChDeco,30E3, true, true);

  bgp2->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.1&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.4&&csctfs_bx[tts_csctf_deps2_idx]==0&&csctfs_ringS1[tts_csctf_deps2_idx]>0)),99.99)>>hp2L1TkMu1p1to2p4BaselineWS1MuChDeco(45,5,50)");
  makeRate(hp2L1TkMu1p1to2p4BaselineWS1MuChDeco,30E3, true, true);

  bgp2->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.6&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.4&&csctfs_bx[tts_csctf_deps2_idx]==0&&csctfs_ringS1[tts_csctf_deps2_idx]>0)),99.99)>>hp2L1TkMu1p6to2p4BaselineWS1MuChDeco(45,5,50)");
  makeRate(hp2L1TkMu1p6to2p4BaselineWS1MuChDeco,30E3, true, true);

  bgp2->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.6&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.1&&csctfs_bx[tts_csctf_deps2_idx]==0&&csctfs_ringS1[tts_csctf_deps2_idx]>0\
&&( (csctfs_gemDPhiS1[tts_csctf_deps2_idx])<-2||abs(csctfs_eta[tts_csctf_deps2_idx])>2.05||((-tts_q*csctfs_gemDPhiS1[tts_csctf_deps2_idx]>-2E-3&&-tts_q*csctfs_gemDPhiS1[tts_csctf_deps2_idx]<sqrt((0.05/tts_pt)**2 + 4E-6))&&csctfs_chamberS1[tts_csctf_deps2_idx]%2==0 )||((-tts_q*csctfs_gemDPhiS1[tts_csctf_deps2_idx]>-3.3E-3&&-tts_q*csctfs_gemDPhiS1[tts_csctf_deps2_idx]<sqrt((0.12/tts_pt)**2 + 1.E-5))&&csctfs_chamberS1[tts_csctf_deps2_idx]%2==1 ) )\
)),99.99)>>hp2L1TkMu1p6to2p1BaselineWS1dPhiMuChDeco(45,5,50)");
  makeRate(hp2L1TkMu1p6to2p1BaselineWS1dPhiMuChDeco,30E3, true, true);

  bgp2->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.6&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.1&&csctfs_bx[tts_csctf_deps2_idx]==0&&csctfs_ringS1[tts_csctf_deps2_idx]>0\
&&(csctfs_pt[tts_csctf_deps2_idx]>0.5*tts_pt||csctfs_pt[tts_csctf_deps2_idx]>20)\
&&( (csctfs_gemDPhiS1[tts_csctf_deps2_idx])<-2||abs(csctfs_eta[tts_csctf_deps2_idx])>2.05||((-tts_q*csctfs_gemDPhiS1[tts_csctf_deps2_idx]>-2E-3&&-tts_q*csctfs_gemDPhiS1[tts_csctf_deps2_idx]<sqrt((0.05/tts_pt)**2 + 4E-6))&&csctfs_chamberS1[tts_csctf_deps2_idx]%2==0 )||((-tts_q*csctfs_gemDPhiS1[tts_csctf_deps2_idx]>-3.3E-3&&-tts_q*csctfs_gemDPhiS1[tts_csctf_deps2_idx]<sqrt((0.12/tts_pt)**2 + 1.E-5))&&csctfs_chamberS1[tts_csctf_deps2_idx]%2==1 ) )\
) ),99.99)>>hp2L1TkMu1p6to2p1BaselineTF0p5PTor20WS1dPhiMuChDeco(45,5,50)");
  makeRate(hp2L1TkMu1p6to2p1BaselineTF0p5PTor20WS1dPhiMuChDeco,30E3, true, true);

  bgp2v13->Draw("min(Max$(tts_pt*(tts_csctf_deps2_idx>=0&&csctfs_tt_deps2_idx[tts_csctf_deps2_idx]==Iteration$&&abs(csctfs_eta[tts_csctf_deps2_idx])>=1.1&&abs(csctfs_eta[tts_csctf_deps2_idx])<2.4&&csctfs_bx[tts_csctf_deps2_idx]==0)),99.99)>>hp2v13L1TkMu1p1to2p4BaselineMuChDeco(45,5,50)");
  makeRate(hp2v13L1TkMu1p1to2p4BaselineMuChDeco,30E3, true, true);


  
  bg->Draw("min(Max$(l1tkmus_pt*(abs(l1tkmus_eta)<1.1\
)),49.99)>>hL1TkMu0to1p1Baseline(45,5,50)");
  makeRate(hL1TkMu0to1p1Baseline,30E3, true, true);


  //same as above, now p2

  bgp2->Draw("min(Max$(gmts_pt*(abs(gmts_eta)>=1.1&&abs(gmts_eta)<2.2\
&&gmts_bx==0\
&&gmts_quality>=4&&gmts_quality!=5)),49.99)>>hp2L1Mu1p1to2p2Baseline(45,5,50)");
  makeRate(hp2L1Mu1p1to2p2Baseline,30E3, true, true);

  bgp2->Draw("min(Max$(gmts_pt*(abs(gmts_eta)>=1.1&&abs(gmts_eta)<2.4\
&&gmts_bx==0\
&&gmts_quality>=4&&gmts_quality!=5)),49.99)>>hp2L1Mu1p1to2p4Baseline(45,5,50)");
  makeRate(hp2L1Mu1p1to2p4Baseline,30E3, true, true);
  
  bgp2->Draw("min(Max$(gmts_pt*(abs(gmts_eta)<1.1\
&&gmts_bx==0\
&&gmts_quality>=4&&gmts_quality!=5)),49.99)>>hp2L1Mu0to1p1Baseline(45,5,50)");
  makeRate(hp2L1Mu0to1p1Baseline,30E3, true, true);


  bgp2->Draw("min(Max$(l1tkmus_pt*(abs(l1tkmus_eta)>=1.1&&abs(l1tkmus_eta)<2.2\
)),49.99)>>hp2L1TkMu1p1to2p2Baseline(45,5,50)");
  makeRate(hp2L1TkMu1p1to2p2Baseline,30E3, true, true);

  bgp2->Draw("min(Max$(l1tkmus_pt*(abs(l1tkmus_eta)>=1.1&&abs(l1tkmus_eta)<2.4\
)),49.99)>>hp2L1TkMu1p1to2p4Baseline(45,5,50)");
  makeRate(hp2L1TkMu1p1to2p4Baseline,30E3, true, true);
  
  bgp2->Draw("min(Max$(l1tkmus_pt*(abs(l1tkmus_eta)<1.1\
)),49.99)>>hp2L1TkMu0to1p1Baseline(45,5,50)");
  makeRate(hp2L1TkMu0to1p1Baseline,30E3, true, true);

  addSystUncty(hL1Mu1p1to2p2Baseline_Rate, 0.05);
  addSystUncty(hL1Mu1p1to2p1Baseline_Rate, 0.05);
  addSystUncty(hL1Mu1p64to2p14Baseline_Rate, 0.05);
  addSystUncty(hL1Mu1p1to2p4Baseline_Rate, 0.05);
  addSystUncty(hL1Mu0to1p1Baseline_Rate, 0.05);
  addSystUncty(hL1TkMu1p1to2p2Baseline_Rate, 0.05);
  addSystUncty(hL1TkMu1p64to2p14Baseline_Rate, 0.05);
  addSystUncty(hL1TkMu1p1to2p4Baseline_Rate, 0.05);
  addSystUncty(hL1TkMu1p1to2p4BaselineMuCh_Rate, 0.05);
  addSystUncty(hp2L1TkMu1p1to2p4BaselineMuCh_Rate, 0.05);
  addSystUncty(hp2L1TkMu1p1to2p4BaselineMuChDeco_Rate, 0.05);
  addSystUncty(hp2L1TkMu1p1to2p4BaselineWS1MuChDeco_Rate, 0.05);
  addSystUncty(hp2v13L1TkMu1p1to2p4BaselineMuChDeco_Rate, 0.05);

  addSystUncty(hL1TkMu1p1to2p4BaselineTF0p5PTor20MuChDeco_Rate, 0.05);
  addSystUncty(hL1TkMu1p1to2p4BaselineTF0p5PTor20MuChDecoEWK_Rate, 0.05);

  addSystUncty(hL1TkMu1p6to2p1BaselineMuCh_Rate, 0.05);
  //  addSystUncty(hp2L1TkMu1p6to2p1BaselineWS1MuChDeco_Rate, 0.05);
  addSystUncty(hp2L1TkMu1p6to2p1BaselineWS1dPhiMuChDeco_Rate, 0.05);
  addSystUncty(hp2L1TkMu1p6to2p1BaselineTF0p5PTor20WS1dPhiMuChDeco_Rate, 0.05);

  addSystUncty(hL1TkMu0to1p1Baseline_Rate, 0.05);

  addSystUncty(hp2L1Mu1p1to2p2Baseline_Rate, 0.05);
  addSystUncty(hp2L1Mu1p1to2p4Baseline_Rate, 0.05);
  addSystUncty(hp2L1Mu0to1p1Baseline_Rate, 0.05);
  addSystUncty(hp2L1TkMu1p1to2p2Baseline_Rate, 0.05);
  addSystUncty(hp2L1TkMu1p1to2p4Baseline_Rate, 0.05);
  addSystUncty(hp2L1TkMu0to1p1Baseline_Rate, 0.05);

  cr = new TCanvas("cr","cr",700,450);
  cr->Clear();
  gPad->SetLogx(0);
  gPad->SetLogy(1);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);

  TH1F* h = hL1Mu1p1to2p2Baseline_Rate;
  h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
  h->GetXaxis()->SetTitle("Muon trigger p_{T} [GeV]");
  h->GetYaxis()->SetTitle("Trigger rate [kHz]");
  h->SetTitleSize(0.05);
  h->SetTitleOffset(1.1, "X");
  h->SetTitleOffset(0.84, "Y");
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetTitleSize(0.055);
  h->GetYaxis()->SetTitleSize(0.06);
  h->SetMinimum(0.101);
  h->SetMaximum(5E3);


//   hL1Mu1p1to2p2Baseline_Rate;
//   hL1Mu0to1p1Baseline_Rate;
//   hL1TkMu1p1to2p2Baseline_Rate;
//   hL1TkMu0to1p1Baseline_Rate;   
   hL1Mu1p1to2p2Baseline_Rate->SetMarkerStyle(25);   
   hL1Mu0to1p1Baseline_Rate->SetMarkerStyle(20);	   
   hL1TkMu1p1to2p2Baseline_Rate->SetMarkerStyle(25); 
   hL1TkMu0to1p1Baseline_Rate->SetMarkerStyle(20);   

   hL1Mu1p1to2p2Baseline_Rate->SetMarkerSize(1.2);   
   hL1Mu0to1p1Baseline_Rate->SetMarkerSize(1.2);	   
   hL1TkMu1p1to2p2Baseline_Rate->SetMarkerSize(1.2); 
   hL1TkMu0to1p1Baseline_Rate->SetMarkerSize(1.2);   

   hL1Mu1p1to2p2Baseline_Rate->SetMarkerColor(2);   
   hL1Mu0to1p1Baseline_Rate->SetMarkerColor(2);	   
   hL1Mu1p1to2p2Baseline_Rate->SetLineColor(2);   
   hL1Mu0to1p1Baseline_Rate->SetLineColor(2);	   

   hL1TkMu1p1to2p2Baseline_Rate->SetMarkerColor(1);   
   hL1TkMu0to1p1Baseline_Rate->SetMarkerColor(1);	   
   hL1TkMu1p1to2p2Baseline_Rate->SetLineColor(1);   
   hL1TkMu0to1p1Baseline_Rate->SetLineColor(1);	   

   hL1Mu1p1to2p2Baseline_Rate->SetLineWidth(1);   
   hL1Mu0to1p1Baseline_Rate->SetLineWidth(1);	   
   hL1TkMu1p1to2p2Baseline_Rate->SetLineWidth(1); 
   hL1TkMu0to1p1Baseline_Rate->SetLineWidth(1);   


   hL1Mu1p1to2p2Baseline_Rate->Draw("e1");   
   hL1Mu0to1p1Baseline_Rate->Draw("e1 same");	   
   hL1TkMu1p1to2p2Baseline_Rate->Draw("e1 same"); 
   hL1TkMu0to1p1Baseline_Rate->Draw("e1 same");   
   
   TLegend* leg = new TLegend(0.45, 0.65, 0.9, 0.9);
   leg->SetFillColor(0); leg->SetBorderSize(0);
   leg->SetTextSize(0.04); leg->SetMargin(0.13);
   leg->AddEntry(hL1Mu0to1p1Baseline_Rate, "L1Muons (qual #geq 4) |#eta| < 1.1");
   leg->AddEntry(hL1TkMu0to1p1Baseline_Rate, "L1TkMuons             |#eta| < 1.1");
   leg->AddEntry(hL1Mu1p1to2p2Baseline_Rate, "L1Muons (qual #geq 4) 1.1 < |#eta| < 2.2");
   leg->AddEntry(hL1TkMu1p1to2p2Baseline_Rate, "L1TkMuons             1.1 < |#eta| < 2.2");
   leg->Draw();
   
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1MuVL1TkMu.png");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1MuVL1TkMu.pdf");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1MuVL1TkMu.eps");
   
   //mixed eta mixh
  crmixh2 = new TCanvas("crmixh2","crmixh2",700,450);
  crmixh2->Clear();
  gPad->SetLogx(0);
  gPad->SetLogy(1);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);

  TH1F* h = hL1Mu1p1to2p4Baseline_Rate;
  h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
  h->GetXaxis()->SetTitle("Muon trigger p_{T} [GeV]");
  h->GetYaxis()->SetTitle("Trigger rate [kHz]");
  h->SetTitleSize(0.05);
  h->SetTitleOffset(1.1, "X");
  h->SetTitleOffset(0.84, "Y");
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetTitleSize(0.055);
  h->GetYaxis()->SetTitleSize(0.06);
  h->SetMinimum(0.101);
  h->SetMaximum(5E3);


   hL1Mu1p1to2p4Baseline_Rate->SetMarkerStyle(25);   
   hL1Mu0to1p1Baseline_Rate->SetMarkerStyle(20);	   
   hL1TkMu1p1to2p4Baseline_Rate->SetMarkerStyle(25); 
   hL1TkMu0to1p1Baseline_Rate->SetMarkerStyle(20);   

   hL1Mu1p1to2p4Baseline_Rate->SetMarkerSize(1.2);   
   hL1Mu0to1p1Baseline_Rate->SetMarkerSize(1.2);	   
   hL1TkMu1p1to2p4Baseline_Rate->SetMarkerSize(1.2); 
   hL1TkMu0to1p1Baseline_Rate->SetMarkerSize(1.2);   

   hL1Mu1p1to2p4Baseline_Rate->SetMarkerColor(2);   
   hL1Mu0to1p1Baseline_Rate->SetMarkerColor(2);	   
   hL1Mu1p1to2p4Baseline_Rate->SetLineColor(2);   
   hL1Mu0to1p1Baseline_Rate->SetLineColor(2);	   

   hL1TkMu1p1to2p4Baseline_Rate->SetMarkerColor(1);   
   hL1TkMu0to1p1Baseline_Rate->SetMarkerColor(1);	   
   hL1TkMu1p1to2p4Baseline_Rate->SetLineColor(1);   
   hL1TkMu0to1p1Baseline_Rate->SetLineColor(1);	   

   hL1Mu1p1to2p4Baseline_Rate->SetLineWidth(1);   
   hL1Mu0to1p1Baseline_Rate->SetLineWidth(1);	   
   hL1TkMu1p1to2p4Baseline_Rate->SetLineWidth(1); 
   hL1TkMu0to1p1Baseline_Rate->SetLineWidth(1);   


   hL1Mu1p1to2p4Baseline_Rate->Draw("e1");   
   hL1Mu0to1p1Baseline_Rate->Draw("e1 same");	   
   hL1TkMu1p1to2p4Baseline_Rate->Draw("e1 same"); 
   hL1TkMu0to1p1Baseline_Rate->Draw("e1 same");   
   
   TLegend* leg = new TLegend(0.45, 0.65, 0.9, 0.9);
   leg->SetFillColor(0); leg->SetBorderSize(0);
   leg->SetTextSize(0.04); leg->SetMargin(0.13);
   leg->AddEntry(hL1Mu0to1p1Baseline_Rate, "L1Muons (qual #geq 4) |#eta| < 1.1");
   leg->AddEntry(hL1TkMu0to1p1Baseline_Rate, "L1TkMuons             |#eta| < 1.1");
   leg->AddEntry(hL1Mu1p1to2p4Baseline_Rate, "L1Muons (qual #geq 4) 1.1 < |#eta| < 2.4");
   leg->AddEntry(hL1TkMu1p1to2p4Baseline_Rate, "L1TkMuons             1.1 < |#eta| < 2.4");
   leg->Draw();
   
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1MuVL1TkMu_mixh2.png");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1MuVL1TkMu_mixh2.pdf");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1MuVL1TkMu_mixh2.eps");

   //mixed eta mixh
  crmixh = new TCanvas("crmixh","crmixh",700,450);
  crmixh->Clear();
  gPad->SetLogx(0);
  gPad->SetLogy(1);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);

  TH1F* h = hL1Mu1p1to2p1Baseline_Rate;
  h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
  h->GetXaxis()->SetTitle("Muon trigger p_{T} [GeV]");
  h->GetYaxis()->SetTitle("Trigger rate [kHz]");
  h->SetTitleSize(0.05);
  h->SetTitleOffset(1.1, "X");
  h->SetTitleOffset(0.84, "Y");
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetTitleSize(0.055);
  h->GetYaxis()->SetTitleSize(0.06);
  h->SetMinimum(0.101);
  h->SetMaximum(5E3);


   hL1Mu1p1to2p1Baseline_Rate->SetMarkerStyle(25);   
   hL1Mu1p1to2p4Baseline_Rate->SetMarkerStyle(25);   
   hL1Mu0to1p1Baseline_Rate->SetMarkerStyle(20);	   
   hL1TkMu1p1to2p4Baseline_Rate->SetMarkerStyle(25); 
   hL1TkMu0to1p1Baseline_Rate->SetMarkerStyle(20);   

   hL1Mu1p1to2p1Baseline_Rate->SetMarkerSize(1.2);   
   hL1Mu1p1to2p4Baseline_Rate->SetMarkerSize(1.2);   
   hL1Mu0to1p1Baseline_Rate->SetMarkerSize(1.2);	   
   hL1TkMu1p1to2p4Baseline_Rate->SetMarkerSize(1.2); 
   hL1TkMu0to1p1Baseline_Rate->SetMarkerSize(1.2);   

   hL1Mu1p1to2p1Baseline_Rate->SetMarkerColor(2);   
   hL1Mu1p1to2p4Baseline_Rate->SetMarkerColor(3);   
   hL1Mu0to1p1Baseline_Rate->SetMarkerColor(2);	   
   hL1Mu1p1to2p1Baseline_Rate->SetLineColor(2);   
   hL1Mu1p1to2p4Baseline_Rate->SetLineColor(3);   
   hL1Mu0to1p1Baseline_Rate->SetLineColor(2);	   

   hL1TkMu1p1to2p4Baseline_Rate->SetMarkerColor(1);   
   hL1TkMu0to1p1Baseline_Rate->SetMarkerColor(1);	   
   hL1TkMu1p1to2p4Baseline_Rate->SetLineColor(1);   
   hL1TkMu0to1p1Baseline_Rate->SetLineColor(1);	   

   hL1Mu1p1to2p1Baseline_Rate->SetLineWidth(1);   
   hL1Mu1p1to2p4Baseline_Rate->SetLineWidth(1);   
   hL1Mu0to1p1Baseline_Rate->SetLineWidth(1);	   
   hL1TkMu1p1to2p4Baseline_Rate->SetLineWidth(1); 
   hL1TkMu0to1p1Baseline_Rate->SetLineWidth(1);   


   hL1Mu1p1to2p1Baseline_Rate->Draw("e1");   
   hL1Mu0to1p1Baseline_Rate->Draw("e1 same");	   
   hL1TkMu1p1to2p4Baseline_Rate->Draw("e1 same"); 
   hL1TkMu0to1p1Baseline_Rate->Draw("e1 same");   
   
   TLegend* leg = new TLegend(0.45, 0.65, 0.9, 0.9);
   leg->SetFillColor(0); leg->SetBorderSize(0);
   leg->SetTextSize(0.04); leg->SetMargin(0.13);
   leg->AddEntry(hL1Mu0to1p1Baseline_Rate, "L1Muons (qual #geq 4) |#eta| < 1.1");
   leg->AddEntry(hL1TkMu0to1p1Baseline_Rate, "L1TkMuons             |#eta| < 1.1");
   leg->AddEntry(hL1Mu1p1to2p1Baseline_Rate, "L1Muons (qual #geq 4) 1.1 < |#eta| < 2.1");
   leg->AddEntry(hL1TkMu1p1to2p4Baseline_Rate, "L1TkMuons             1.1 < |#eta| < 2.4");
   leg->Draw();
   
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1MuVL1TkMu_mixh.png");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1MuVL1TkMu_mixh.pdf");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1MuVL1TkMu_mixh.eps");

   hL1Mu1p1to2p1Baseline_Rate->Draw("e1");   
   hL1Mu1p1to2p4Baseline_Rate->Draw("e1 same");   
   hL1Mu0to1p1Baseline_Rate->Draw("e1 same");	   
   hL1TkMu1p1to2p4Baseline_Rate->Draw("e1 same"); 
   hL1TkMu0to1p1Baseline_Rate->Draw("e1 same");   
   
   TLegend* leg = new TLegend(0.45, 0.65, 0.9, 0.9);
   leg->SetFillColor(0); leg->SetBorderSize(0);
   leg->SetTextSize(0.04); leg->SetMargin(0.13);
   leg->AddEntry(hL1Mu0to1p1Baseline_Rate, "L1Muons (qual #geq 4) |#eta| < 1.1");
   leg->AddEntry(hL1TkMu0to1p1Baseline_Rate, "L1TkMuons             |#eta| < 1.1");
   leg->AddEntry(hL1Mu1p1to2p1Baseline_Rate, "L1Muons (qual #geq 4) 1.1 < |#eta| < 2.1");
   leg->AddEntry(hL1Mu1p1to2p4Baseline_Rate, "L1Muons (qual #geq 4) 1.1 < |#eta| < 2.4");
   leg->AddEntry(hL1TkMu1p1to2p4Baseline_Rate, "L1TkMuons             1.1 < |#eta| < 2.4");
   leg->Draw();
   
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1MuVL1TkMu_mixhMore.png");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1MuVL1TkMu_mixhMore.pdf");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1MuVL1TkMu_mixhMore.eps");
   

   //GE1/1 range
  crge11 = new TCanvas("crge11","crge11",700,450);
  crge11->Clear();
  gPad->SetLogx(0);
  gPad->SetLogy(1);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);

  TH1F* h = hL1Mu1p64to2p14Baseline_Rate;
  h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
  h->GetXaxis()->SetTitle("Muon trigger p_{T} [GeV]");
  h->GetYaxis()->SetTitle("Trigger rate [kHz]");
  h->SetTitleSize(0.05);
  h->SetTitleOffset(1.1, "X");
  h->SetTitleOffset(0.84, "Y");
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetTitleSize(0.055);
  h->GetYaxis()->SetTitleSize(0.06);
  h->SetMinimum(0.101);
  h->SetMaximum(5E3);


//   hL1Mu1p1to2p2Baseline_Rate;
//   hL1Mu0to1p1Baseline_Rate;
//   hL1TkMu1p1to2p2Baseline_Rate;
//   hL1TkMu0to1p1Baseline_Rate;   
   hL1Mu1p64to2p14Baseline_Rate->SetMarkerStyle(25);   
   hL1Mu1p64to2p14nS2w1_Rate->SetMarkerStyle(20);   
   hL1TkMu1p64to2p14Baseline_Rate->SetMarkerStyle(25); 

   hL1Mu1p64to2p14Baseline_Rate->SetMarkerSize(1.2);   
   hL1Mu1p64to2p14nS2w1_Rate->SetMarkerSize(1.2);   
   hL1TkMu1p64to2p14Baseline_Rate->SetMarkerSize(1.2); 

   hL1Mu1p64to2p14Baseline_Rate->SetMarkerColor(2);   
   hL1Mu1p64to2p14nS2w1_Rate->SetMarkerColor(2);   
   hL1Mu1p64to2p14Baseline_Rate->SetLineColor(2);   

   hL1TkMu1p64to2p14Baseline_Rate->SetMarkerColor(1);   
   hL1TkMu1p64to2p14Baseline_Rate->SetLineColor(1);   

   hL1Mu1p64to2p14Baseline_Rate->SetLineWidth(1);   
   hL1TkMu1p64to2p14Baseline_Rate->SetLineWidth(1); 

   hL1Mu1p64to2p14Baseline_Rate->Draw("e1");   
   hL1Mu1p64to2p14nS2w1_Rate->Draw("e1 same");   
   hL1TkMu1p64to2p14Baseline_Rate->Draw("e1 same"); 
   
   TLegend* leg = new TLegend(0.45, 0.65, 0.9, 0.95);
   leg->SetFillColor(0); leg->SetBorderSize(0);
   leg->SetTextSize(0.04); leg->SetMargin(0.13);
   leg->AddEntry(hL1Mu1p64to2p14Baseline_Rate, "L1Muons (qual #geq 4)");
   leg->AddEntry(hL1Mu1p64to2p14nS2w1_Rate, "L1Muons (2+ stubs w MS1)");
   leg->AddEntry(hL1TkMu1p64to2p14Baseline_Rate, "L1TkMuons          ");
   leg->Draw();
   
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineh1p64to2p14L1MuVL1TkMu.png");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineh1p64to2p14L1MuVL1TkMu.pdf");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineh1p64to2p14L1MuVL1TkMu.eps");
   



  crtkmu = new TCanvas("crtkmu","crtkmu",700,450);
  crtkmu->Clear();
  gPad->SetLogx(0);
  gPad->SetLogy(1);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);

  TH1F* h = hL1TkMu1p1to2p4Baseline_Rate;
  h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
  h->GetXaxis()->SetTitle("Muon trigger p_{T} [GeV]");
  h->GetYaxis()->SetTitle("Trigger rate [kHz]");
  h->SetTitleSize(0.05);
  h->SetTitleOffset(1.1, "X");
  h->SetTitleOffset(0.84, "Y");
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetTitleSize(0.055);
  h->GetYaxis()->SetTitleSize(0.06);
  h->SetMinimum(0.101);
  h->SetMaximum(5E3);


//   hL1Mu1p1to2p2Baseline_Rate;
//   hL1Mu0to1p1Baseline_Rate;
//   hL1TkMu1p1to2p2Baseline_Rate;
//   hL1TkMu0to1p1Baseline_Rate;   
   hL1Mu1p1to2p2Baseline_Rate->SetMarkerStyle(25);   
   hL1Mu0to1p1Baseline_Rate->SetMarkerStyle(20);	   
   hL1TkMu1p1to2p4Baseline_Rate->SetMarkerStyle(25); 
   hL1TkMu0to1p1Baseline_Rate->SetMarkerStyle(20);   

   hL1Mu1p1to2p2Baseline_Rate->SetMarkerSize(1.2);   
   hL1Mu0to1p1Baseline_Rate->SetMarkerSize(1.2);	   
   hL1TkMu1p1to2p4Baseline_Rate->SetMarkerSize(1.2); 
   hL1TkMu0to1p1Baseline_Rate->SetMarkerSize(1.2);   

   hL1Mu1p1to2p2Baseline_Rate->SetMarkerColor(2);   
   hL1Mu0to1p1Baseline_Rate->SetMarkerColor(2);	   
   hL1Mu1p1to2p2Baseline_Rate->SetLineColor(2);   
   hL1Mu0to1p1Baseline_Rate->SetLineColor(2);	   

   hL1TkMu1p1to2p4Baseline_Rate->SetMarkerColor(1);   
   hL1TkMu0to1p1Baseline_Rate->SetMarkerColor(1);	   
   hL1TkMu1p1to2p4Baseline_Rate->SetLineColor(1);   
   hL1TkMu0to1p1Baseline_Rate->SetLineColor(1);	   

   hL1Mu1p1to2p2Baseline_Rate->SetLineWidth(1);   
   hL1Mu0to1p1Baseline_Rate->SetLineWidth(1);	   
   hL1TkMu1p1to2p4Baseline_Rate->SetLineWidth(1); 
   hL1TkMu0to1p1Baseline_Rate->SetLineWidth(1);   


   //   hL1Mu1p1to2p2Baseline_Rate->Draw("e1");   
   //   hL1Mu0to1p1Baseline_Rate->Draw("e1 same");	   
   hL1TkMu1p1to2p4Baseline_Rate->Draw("e1"); 
   hL1TkMu0to1p1Baseline_Rate->Draw("e1 same");   
   
   TLegend* leg = new TLegend(0.45, 0.65, 0.9, 0.9);
   leg->SetFillColor(0); leg->SetBorderSize(0);
   leg->SetTextSize(0.04); leg->SetMargin(0.13);
   leg->AddEntry(hL1TkMu0to1p1Baseline_Rate, "L1TkMuons             |#eta| < 1.1");
   leg->AddEntry(hL1TkMu1p1to2p4Baseline_Rate, "L1TkMuons             1.1 < |#eta| < 2.4");
   leg->Draw();
   
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1TkMu.png");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1TkMu.pdf");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1TkMu.eps");
   

   hL1Mu1p1to2p4Baseline_Rate->SetLineColor(3);
   hL1Mu1p1to2p4Baseline_Rate->SetMarkerColor(3);
   hL1Mu1p1to2p4Baseline_Rate->SetMarkerStyle(25);
   hL1Mu1p1to2p4Baseline_Rate->SetMarkerSize(1.2);

   hL1Mu1p1to2p2Baseline_Rate->Draw("e1"); 
   hL1Mu1p1to2p4Baseline_Rate->Draw("e1 same"); 
   hL1Mu0to1p1Baseline_Rate->Draw("e1 same");   
   hL1Mu1p1to2p2Baseline_Rate->SetMinimum(0.99);
   TLegend* leg = new TLegend(0.45, 0.65, 0.9, 0.9);
   leg->SetFillColor(0); leg->SetBorderSize(0);
   leg->SetTextSize(0.04); leg->SetMargin(0.13);
   leg->AddEntry(hL1Mu0to1p1Baseline_Rate,   "L1Muons             |#eta| < 1.1");
   leg->AddEntry(hL1Mu1p1to2p4Baseline_Rate, "L1Muons             1.1 < |#eta| < 2.4");
   leg->AddEntry(hL1Mu1p1to2p2Baseline_Rate, "L1Muons             1.1 < |#eta| < 2.2");
   leg->Draw();
   
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1Mu.png");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1Mu.pdf");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1Mu.eps");


  crtkmuopt1 = new TCanvas("crtkmuopt1","crtkmuopt1",700,450);
  crtkmuopt1->Clear();
  gPad->SetLogx(0);
  gPad->SetLogy(1);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);

  TH1F* h = hL1TkMu1p1to2p4BaselineMuCh_Rate;
  h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
  h->GetXaxis()->SetTitle("Muon trigger p_{T} [GeV]");
  h->GetYaxis()->SetTitle("Trigger rate [kHz]");
  h->SetTitleSize(0.05);
  h->SetTitleOffset(1.1, "X");
  h->SetTitleOffset(0.84, "Y");
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetTitleSize(0.055);
  h->GetYaxis()->SetTitleSize(0.06);
  h->SetMinimum(0.101);
  h->SetMaximum(5E3);


   hL1TkMu1p1to2p4BaselineMuCh_Rate->SetLineColor(kBlue+1);   
   hL1TkMu1p1to2p4BaselineMuCh_Rate->SetFillColor(kBlue+1);   
   hL1TkMu1p1to2p4BaselineMuCh_Rate->SetLineWidth(1); 

   hp2L1TkMu1p1to2p4BaselineMuCh_Rate->SetLineColor(kBlue-2);   
   hp2L1TkMu1p1to2p4BaselineMuCh_Rate->SetFillColor(kBlue-2);   
   hp2L1TkMu1p1to2p4BaselineMuCh_Rate->SetLineWidth(1); 

   hp2L1TkMu1p1to2p4BaselineMuChDeco_Rate->SetLineColor(kCyan);   
   hp2L1TkMu1p1to2p4BaselineMuChDeco_Rate->SetFillColor(kCyan);   
   hp2L1TkMu1p1to2p4BaselineMuChDeco_Rate->SetLineWidth(1); 

   hp2v13L1TkMu1p1to2p4BaselineMuChDeco_Rate->SetLineColor(kCyan-2);   
   hp2v13L1TkMu1p1to2p4BaselineMuChDeco_Rate->SetFillColor(kCyan-2);   
   hp2v13L1TkMu1p1to2p4BaselineMuChDeco_Rate->SetLineWidth(1); 


   hL1TkMu1p1to2p4BaselineMuCh_Rate->Draw("e3"); 
   hp2L1TkMu1p1to2p4BaselineMuCh_Rate->Draw("e3 same"); 
   hp2L1TkMu1p1to2p4BaselineMuChDeco_Rate->Draw("e3 same"); 
   hp2v13L1TkMu1p1to2p4BaselineMuChDeco_Rate->Draw("e3 same"); 
   
   tp = new TPave(0.4, 0.83, 0.95, 0.9, 0, "NDC"); tp->SetFillColor(0);
   tp->Draw();
   tex1 = new TLatex(.41,.85,"L1TkMu: 1.1<|#eta|<2.4");
   tex1->SetTextFont(42);
   tex1->SetTextSize(0.048);
   tex1->SetNDC();
   tex1->Draw();

   TLegend* leg = new TLegend(0.4, 0.74, 0.95, 0.84);
   leg->SetFillColor(0); leg->SetBorderSize(0);
   leg->SetTextSize(0.04); leg->SetMargin(0.13);
   leg->AddEntry(hp2L1TkMu1p1to2p4BaselineMuCh_Rate, "Phase-2 muon");
   leg->AddEntry(hp2L1TkMu1p1to2p4BaselineMuChDeco_Rate, "Phase-2 muon on-the-fly");
   leg->AddEntry(hp2v13L1TkMu1p1to2p4BaselineMuChDeco_Rate, "Phase-2 muon V13 on-the-fly");
   leg->AddEntry(hL1TkMu1p1to2p4BaselineMuCh_Rate, "Phase-1 muon");
   leg->Draw();
   
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1TkMu_muChOpt1debug.png");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1TkMu_muChOpt1debug.pdf");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1TkMu_muChOpt1debug.eps");
   

   hp2L1TkMu1p1to2p4BaselineWS1MuChDeco_Rate->SetLineColor(kRed);   
   hp2L1TkMu1p1to2p4BaselineWS1MuChDeco_Rate->SetFillColor(kRed);   
   hp2L1TkMu1p1to2p4BaselineWS1MuChDeco_Rate->SetLineWidth(1); 

   hL1TkMu1p1to2p4BaselineMuCh_Rate->Draw("e3"); 
   hp2L1TkMu1p1to2p4BaselineWS1MuChDeco_Rate->Draw("e3 same");
   
   tp = new TPave(0.4, 0.83, 0.95, 0.9, 0, "NDC"); tp->SetFillColor(0);
   tp->Draw();
   tex1 = new TLatex(.41,.85,"L1TkMu: 1.1<|#eta|<2.4");
   tex1->SetTextFont(42);
   tex1->SetTextSize(0.048);
   tex1->SetNDC();
   tex1->Draw();

   TLegend* leg = new TLegend(0.4, 0.74, 0.95, 0.84);
   leg->SetFillColor(0); leg->SetBorderSize(0);
   leg->SetTextSize(0.04); leg->SetMargin(0.13);
   leg->AddEntry(hL1TkMu1p1to2p4BaselineMuCh_Rate, "Phase-1 muon");
   leg->AddEntry(hp2L1TkMu1p1to2p4BaselineWS1MuChDeco_Rate, "Phase-2 muon with hits in MS1");
   leg->Draw();
   
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1TkMu_muChOpt1.png");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1TkMu_muChOpt1.pdf");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1TkMu_muChOpt1.eps");
   

  crtkmuopt1ewk = new TCanvas("crtkmuopt1ewk","crtkmuopt1ewk",700,450);
  crtkmuopt1ewk->Clear();
  gPad->SetLogx(0);
  gPad->SetLogy(1);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);

  TH1F* h = hL1TkMu1p1to2p4BaselineMuCh_Rate;
  h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
  h->GetXaxis()->SetTitle("Muon trigger p_{T} [GeV]");
  h->GetYaxis()->SetTitle("Trigger rate [kHz]");
  h->SetTitleSize(0.05);
  h->SetTitleOffset(1.1, "X");
  h->SetTitleOffset(0.84, "Y");
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetTitleSize(0.055);
  h->GetYaxis()->SetTitleSize(0.06);
  h->SetMinimum(0.03);
  h->SetMaximum(5E3);


   hL1TkMu1p1to2p4BaselineMuCh_Rate->SetLineColor(kBlue+1);   
   hL1TkMu1p1to2p4BaselineMuCh_Rate->SetFillColor(kBlue+1);   
   hL1TkMu1p1to2p4BaselineMuCh_Rate->SetLineWidth(1); 

   hL1TkMu1p1to2p4BaselineTF0p5PTor20MuChDeco_Rate->SetLineColor(kCyan+2);
   hL1TkMu1p1to2p4BaselineTF0p5PTor20MuChDeco_Rate->SetFillColor(kCyan+2);
   hL1TkMu1p1to2p4BaselineTF0p5PTor20MuChDeco_Rate->SetLineWidth(1);

   hL1TkMu1p1to2p4BaselineTF0p5PTor20MuChDecoEWK_Rate->SetLineColor(kMagenta);
   hL1TkMu1p1to2p4BaselineTF0p5PTor20MuChDecoEWK_Rate->SetFillColor(kMagenta);
   hL1TkMu1p1to2p4BaselineTF0p5PTor20MuChDecoEWK_Rate->SetLineWidth(1);


   hL1TkMu1p1to2p4BaselineMuCh_Rate->Draw("e3"); 
   hL1TkMu1p1to2p4BaselineTF0p5PTor20MuChDeco_Rate->Draw("e3 same");
   hL1TkMu1p1to2p4BaselineTF0p5PTor20MuChDecoEWK_Rate->Draw("e3 same");
   
   tp = new TPave(0.4, 0.83, 0.95, 0.9, 0, "NDC"); tp->SetFillColor(0);
   tp->Draw();
   tex1 = new TLatex(.41,.85,"L1TkMu: 1.1<|#eta|<2.4 with Phase-1 muon");
   tex1->SetTextFont(42);
   tex1->SetTextSize(0.048);
   tex1->SetNDC();
   tex1->Draw();

   TLegend* leg = new TLegend(0.4, 0.6, 0.95, 0.84);
   leg->SetFillColor(0); leg->SetBorderSize(0);
   leg->SetTextSize(0.04); leg->SetMargin(0.13);
   leg->AddEntry(hL1TkMu1p1to2p4BaselineMuCh_Rate, "Baseline: MinBias");
   leg->AddEntry(hL1TkMu1p1to2p4BaselineTF0p5PTor20MuChDeco_Rate, "with [ p_{T}^{TF} > min(p_{T}^{TT}/2, 20): MinBias");
   leg->AddEntry(hL1TkMu1p1to2p4BaselineTF0p5PTor20MuChDecoEWK_Rate, "with [ p_{T}^{TF} > min(p_{T}^{TT}/2, 20): Wjets");

   leg->Draw();
   
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1TkMu_muChOpt1ewk.png");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1TkMu_muChOpt1ewk.pdf");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1TkMu_muChOpt1ewk.eps");

  crtkmuopt2 = new TCanvas("crtkmuopt2","crtkmuopt2",700,450);
  crtkmuopt2->Clear();
  gPad->SetLogx(0);
  gPad->SetLogy(1);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);

  TH1F* h = hL1TkMu1p6to2p1BaselineMuCh_Rate;
  h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
  h->GetXaxis()->SetTitle("Muon trigger p_{T} [GeV]");
  h->GetYaxis()->SetTitle("Trigger rate [kHz]");
  h->SetTitleSize(0.05);
  h->SetTitleOffset(1.1, "X");
  h->SetTitleOffset(0.84, "Y");
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetTitleSize(0.055);
  h->GetYaxis()->SetTitleSize(0.06);
  h->SetMinimum(0.101);
  h->SetMaximum(5E3);


   hL1TkMu1p6to2p1BaselineMuCh_Rate->SetLineColor(kBlue+1);   
   hL1TkMu1p6to2p1BaselineMuCh_Rate->SetFillColor(kBlue+1);   
   hL1TkMu1p6to2p1BaselineMuCh_Rate->SetLineWidth(1); 

   hp2L1TkMu1p6to2p1BaselineWS1dPhiMuChDeco_Rate->SetLineColor(kRed+2);   
   hp2L1TkMu1p6to2p1BaselineWS1dPhiMuChDeco_Rate->SetFillColor(kRed+2);   
   hp2L1TkMu1p6to2p1BaselineWS1dPhiMuChDeco_Rate->SetLineWidth(1); 

   hL1TkMu1p6to2p1BaselineMuCh_Rate->Draw("e3"); 
   hp2L1TkMu1p6to2p1BaselineWS1dPhiMuChDeco_Rate->Draw("e3 same");
   
   tp = new TPave(0.4, 0.83, 0.95, 0.9, 0, "NDC"); tp->SetFillColor(0);
   tp->Draw();
   tex1 = new TLatex(.41,.85,"L1TkMu: 1.6<|#eta|<2.1");
   tex1->SetTextFont(42);
   tex1->SetTextSize(0.048);
   tex1->SetNDC();
   tex1->Draw();

   TLegend* leg = new TLegend(0.4, 0.74, 0.95, 0.84);
   leg->SetFillColor(0); leg->SetBorderSize(0);
   leg->SetTextSize(0.04); leg->SetMargin(0.13);
   leg->AddEntry(hL1TkMu1p6to2p1BaselineMuCh_Rate, "Phase-1 muon");
   leg->AddEntry(hp2L1TkMu1p6to2p1BaselineWS1dPhiMuChDeco_Rate, "Phase-2 muon with MS1 and bending cut");
   leg->Draw();
   
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1TkMu_muChOpt2.png");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1TkMu_muChOpt2.pdf");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1TkMu_muChOpt2.eps");
   

  crtkmuopt2tfpt = new TCanvas("crtkmuopt2tfpt","crtkmuopt2tfpt",700,450);
  crtkmuopt2tfpt->Clear();
  gPad->SetLogx(0);
  gPad->SetLogy(1);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);

  TH1F* h = hL1TkMu1p6to2p1BaselineMuCh_Rate;
  h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
  h->GetXaxis()->SetTitle("Muon trigger p_{T} [GeV]");
  h->GetYaxis()->SetTitle("Trigger rate [kHz]");
  h->SetTitleSize(0.05);
  h->SetTitleOffset(1.1, "X");
  h->SetTitleOffset(0.84, "Y");
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetTitleSize(0.055);
  h->GetYaxis()->SetTitleSize(0.06);
  h->SetMinimum(0.45);
  h->SetMaximum(5E2);


   hL1TkMu1p6to2p1BaselineMuCh_Rate->SetLineColor(kBlue+1);   
   hL1TkMu1p6to2p1BaselineMuCh_Rate->SetFillColor(kBlue+1);   
   hL1TkMu1p6to2p1BaselineMuCh_Rate->SetLineWidth(1); 

   hp2L1TkMu1p6to2p1BaselineWS1dPhiMuChDeco_Rate->SetLineColor(kRed+2);   
   hp2L1TkMu1p6to2p1BaselineWS1dPhiMuChDeco_Rate->SetFillColor(kRed+2);   
   hp2L1TkMu1p6to2p1BaselineWS1dPhiMuChDeco_Rate->SetLineWidth(1); 

   hp2L1TkMu1p6to2p1BaselineTF0p5PTor20WS1dPhiMuChDeco_Rate->SetLineColor(kMagenta);   
   hp2L1TkMu1p6to2p1BaselineTF0p5PTor20WS1dPhiMuChDeco_Rate->SetFillColor(kMagenta);   
   hp2L1TkMu1p6to2p1BaselineTF0p5PTor20WS1dPhiMuChDeco_Rate->SetLineWidth(1); 

   hL1TkMu1p6to2p1BaselineMuCh_Rate->Draw("e3"); 
   hp2L1TkMu1p6to2p1BaselineWS1dPhiMuChDeco_Rate->Draw("e3 same");
   hp2L1TkMu1p6to2p1BaselineTF0p5PTor20WS1dPhiMuChDeco_Rate->Draw("e3 same");

   tp = new TPave(0.4, 0.83, 0.95, 0.9, 0, "NDC"); tp->SetFillColor(0);
   tp->Draw();
   tex1 = new TLatex(.41,.85,"L1TkMu: 1.6<|#eta|<2.1");
   tex1->SetTextFont(42);
   tex1->SetTextSize(0.048);
   tex1->SetNDC();
   tex1->Draw();

   TLegend* leg = new TLegend(0.4, 0.74, 0.95, 0.84);
   leg->SetFillColor(0); leg->SetBorderSize(0);
   leg->SetTextSize(0.04); leg->SetMargin(0.13);
   leg->AddEntry(hL1TkMu1p6to2p1BaselineMuCh_Rate, "Phase-1 muon");
   leg->AddEntry(hp2L1TkMu1p6to2p1BaselineWS1dPhiMuChDeco_Rate, "Phase-2 muon with MS1 and bending cut");
   leg->AddEntry(hp2L1TkMu1p6to2p1BaselineTF0p5PTor20WS1dPhiMuChDeco_Rate, "Phase-2 muon with #Delta#phi and TF p_{T} cuts");
   leg->Draw();
   
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1TkMu_muChOpt2tfpt.png");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1TkMu_muChOpt2tfpt.pdf");
   gPad->SaveAs("cms/emu/June1462X/hrate_baselineL1TkMu_muChOpt2tfpt.eps");
   


   

}

void addSystUncty(TH1F* h, float syst){
  int nB = h->GetNbinsX();
  for (int i = 0; i<= nB+1; ++i){
    float val = h->GetBinContent(i);
    float e0 = h->GetBinError(i);
    float e1 = sqrt(val*val*syst*syst + e0*e0);
    h->SetBinError(i, e1);
  }

}

//30E3 will be in kHz by default, assuming the input is not pre-filtered
void makeRate(const TH1F* hIn, float scale = 30E3, bool print20 = true, bool print10 = false){
  int nB = hIn->GetNbinsX();
  float xmin = hIn->GetXaxis()->GetBinLowEdge(1);
  float xmax = hIn->GetXaxis()->GetBinUpEdge(nB);
  hRate = new TH1F(Form("%s_Rate",hIn->GetName()), Form("%s_Rate",hIn->GetName()), nB, xmin, xmax);
  float inInt = hIn->Integral(0,nB+1);
  for (int i = 0; i < nB +2; ++i){
    float sumW2 =0;
    for (int j = i; j< nB+2; ++j) sumW2+= hIn->GetBinError(j)*hIn->GetBinError(j);
    
    float iInt = hIn->Integral(i, nB+1);
    float rVal = scale* iInt/ inInt;
    hRate->SetBinContent(i, rVal);
    hRate->SetBinError(i, sqrt(sumW2) * scale / inInt);
    TAxis* ax = hRate->GetXaxis();
    float iLo = ax->GetBinLowEdge(i);
    float iHi = ax->GetBinUpEdge(i);
    if (print20){
      if (iLo <= 20. && iHi > 20){
	std::cout<<hRate->GetName()<<" at 20 GeV "<<hRate->GetBinContent(i)<<" pm "<<hRate->GetBinError(i)<<std::endl;
      }
    }
    if (print10){
      if (iLo <= 10. && iHi > 10){
	std::cout<<hRate->GetName()<<" at 10 GeV "<<hRate->GetBinContent(i)<<" pm "<<hRate->GetBinError(i)<<std::endl;
      }
    }
  }
  hRate->GetYaxis()->SetTitle("Rate for p_{T} cut, kHz");
  
}

void getEfficiencyTTtoTFdetails(){
  gStyle->SetOptStat(0);
  gStyle->SetTitleSize(0.055, "");
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleAlign(13); // coord in top left
  gStyle->SetTitleX(0.);
  gStyle->SetTitleY(1.);
  gStyle->SetTitleW(1);
  gStyle->SetTitleH(0.06);
  gStyle->SetTitleBorderSize(0);
  
  gStyle->SetPadLeftMargin(0.126);
  gStyle->SetPadRightMargin(0.04);
  gStyle->SetPadTopMargin(0.065);
  gStyle->SetPadBottomMargin(0.13);
  gStyle->SetOptStat(0);
  gStyle->SetMarkerStyle(1);


  TChain* sig = new TChain("Events");
  sig->Add("/Data/cms/emu/phase2/CMSSW_6_2_0_SLHC13-ttiRedigiJun14ana/SingleMuPlusFlatPt0p2To150/TTI2023Upg14D-PU140bx25-ILT_SLHC14ttnt-redoL1MuPhase1/f7346d9906da6f33ac6d3facaf675a25/tt_ntuple_redoL1Mu_*.root");
  sig->Add("/Data/cms/emu/phase2/CMSSW_6_2_0_SLHC13-ttiRedigiJun14ana/SingleMuMinusFlatPt0p2To150/TTI2023Upg14D-PU140bx25-ILT_SLHC14ttnt-redoL1MuPhase1/f7346d9906da6f33ac6d3facaf675a25/tt_ntuple_redoL1Mu_*.root");

  TChain* sigp2 = new TChain("Events");
  sigp2->Add("/Data/cms/emu/phase2/CMSSW_6_2_0_SLHC13-ttiRedigiJun14ana/SingleMuPlusFlatPt0p2To150/TTI2023Upg14D-PU140bx25-ILT_SLHC14ttnt-redoL1Mu/f444b074e21342f24eed8d9f060fcad3/tt_ntuple_redoL1Mu_*.root");
  sigp2->Add("/Data/cms/emu/phase2/CMSSW_6_2_0_SLHC13-ttiRedigiJun14ana/SingleMuMinusFlatPt0p2To150/TTI2023Upg14D-PU140bx25-ILT_SLHC14ttnt-redoL1Mu/f444b074e21342f24eed8d9f060fcad3/tt_ntuple_redoL1Mu_*.root");


  // 
  sig->Draw("sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2&&abs(csctfs_eta[sims_csctf_drs2_idx])>=1.2&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.1&&csctfs_bx[sims_csctf_drs2_idx]==0:min(sims_pt,39.99)>>hsim1p25to2p05EffCSCTF(40,0,40)", "abs(sims_eta)>1.25&&abs(sims_eta)<2.05", "prof");
  
  sig->Draw("sims_tt_dr_idx>=0&&sims_tt_dr_dr<0.02&&tts_csctf_drs2_idx[sims_tt_dr_idx]>=0 &&sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2&&abs(csctfs_eta[sims_csctf_drs2_idx])>=1.2&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.1&&csctfs_bx[sims_csctf_drs2_idx]==0 &&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]>=0:min(sims_pt,39.99)>>hsim1p25to2p05EffCSCTFnTT(40,0,40)", "abs(sims_eta)>1.25&&abs(sims_eta)<2.05", "prof");
  
  sig->Draw("sims_tt_dr_idx>=0&&sims_tt_dr_dr<0.02&&tts_csctf_drs2_idx[sims_tt_dr_idx]>=0 &&sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2&&abs(csctfs_eta[sims_csctf_drs2_idx])>=1.2&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.1&&csctfs_bx[sims_csctf_drs2_idx]==0 &&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]>=0&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]==sims_tt_dr_idx:min(sims_pt,39.99)>>hsim1p25to2p05EffCSCTFnTTcorr(40,0,40)", "abs(sims_eta)>1.25&&abs(sims_eta)<2.05", "prof");
  
  sig->Draw("sims_tt_dr_idx>=0&&sims_tt_dr_dr<0.02&&tts_csctf_drs2_idx[sims_tt_dr_idx]>=0 &&sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2&&abs(csctfs_eta[sims_csctf_drs2_idx])>=1.2&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.1&&csctfs_bx[sims_csctf_drs2_idx]==0 &&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]>=0&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]==sims_tt_dr_idx &&tts_csctf_deps2_idx[sims_tt_dr_idx]>=0:min(sims_pt,39.99)>>hsim1p25to2p05EffCSCTFnTTcorrEPCut(40,0,40)", "abs(sims_eta)>1.25&&abs(sims_eta)<2.05", "prof");
  
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);
  
  hsim1p25to2p05EffCSCTF->SetLineWidth(2);
  hsim1p25to2p05EffCSCTFnTT->SetLineWidth(2);
  hsim1p25to2p05EffCSCTFnTTcorr->SetLineWidth(2);
  hsim1p25to2p05EffCSCTFnTTcorrEPCut->SetLineWidth(2);
  
  hsim1p25to2p05EffCSCTF->SetLineColor(2);
  hsim1p25to2p05EffCSCTFnTT->SetLineColor(1);
  hsim1p25to2p05EffCSCTFnTTcorr->SetLineColor(6);
  hsim1p25to2p05EffCSCTFnTTcorrEPCut->SetLineColor(7);
  
  
  hsim1p25to2p05EffCSCTF->Draw();
  hsim1p25to2p05EffCSCTF->SetTitle("Combined TT-CSCTF single muon efficiency, 1.25<|#eta_{SIM}|<2.05");
  hsim1p25to2p05EffCSCTF->GetXaxis()->SetTitle("p_{T}^{SIM}");
  hsim1p25to2p05EffCSCTF->GetYaxis()->SetTitle("Efficiency");
  hsim1p25to2p05EffCSCTFnTT->Draw("same");
  hsim1p25to2p05EffCSCTFnTTcorr->Draw("same");
  hsim1p25to2p05EffCSCTFnTTcorrEPCut->Draw("same");
  
  TLegend* leg = new TLegend(0.25, 0.3, 0.99, 0.6);
  leg->SetFillColor(0);
  leg->AddEntry(hsim1p25to2p05EffCSCTF, "TF-SIM");
  leg->AddEntry(hsim1p25to2p05EffCSCTFnTT, "TF-SIM and TT-SIM");
  leg->AddEntry(hsim1p25to2p05EffCSCTFnTTcorr, "TF-SIM and TT-SIM and TT-closest-TF");
  leg->AddEntry(hsim1p25to2p05EffCSCTFnTTcorrEPCut, "Baseline: TF-SIM and TT-SIM and TT-closest-TF and in #Delta#eta#Delta#phi");
  leg->Draw();
  
  gPad->SaveAs("cms/emu/June1462X/hsim1p25to2p05Eff_baselineVsteps.png");
  gPad->SaveAs("cms/emu/June1462X/hsim1p25to2p05Eff_baselineVsteps.pdf");
  gPad->SaveAs("cms/emu/June1462X/hsim1p25to2p05Eff_baselineVsteps.eps");
  
  
  // ME1/a region
  sig->Draw("sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2&&abs(csctfs_eta[sims_csctf_drs2_idx])>=2.0&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.4&&csctfs_bx[sims_csctf_drs2_idx]==0:min(sims_pt,39.99)>>hsim2p05to2p35EffCSCTF(40,0,40)", "abs(sims_eta)>2.05&&abs(sims_eta)<2.35", "prof");
  
  sig->Draw("sims_tt_dr_idx>=0&&sims_tt_dr_dr<0.02&&tts_csctf_drs2_idx[sims_tt_dr_idx]>=0 &&sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2&&abs(csctfs_eta[sims_csctf_drs2_idx])>=2.0&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.4&&csctfs_bx[sims_csctf_drs2_idx]==0 &&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]>=0:min(sims_pt,39.99)>>hsim2p05to2p35EffCSCTFnTT(40,0,40)", "abs(sims_eta)>2.05&&abs(sims_eta)<2.35", "prof");
  
  sig->Draw("sims_tt_dr_idx>=0&&sims_tt_dr_dr<0.02&&tts_csctf_drs2_idx[sims_tt_dr_idx]>=0 &&sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2&&abs(csctfs_eta[sims_csctf_drs2_idx])>=2.0&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.4&&csctfs_bx[sims_csctf_drs2_idx]==0 &&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]>=0&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]==sims_tt_dr_idx:min(sims_pt,39.99)>>hsim2p05to2p35EffCSCTFnTTcorr(40,0,40)", "abs(sims_eta)>2.05&&abs(sims_eta)<2.35", "prof");
  
  sig->Draw("sims_tt_dr_idx>=0&&sims_tt_dr_dr<0.02&&tts_csctf_drs2_idx[sims_tt_dr_idx]>=0 &&sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2&&abs(csctfs_eta[sims_csctf_drs2_idx])>=2.0&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.4&&csctfs_bx[sims_csctf_drs2_idx]==0 &&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]>=0&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]==sims_tt_dr_idx &&tts_csctf_deps2_idx[sims_tt_dr_idx]>=0:min(sims_pt,39.99)>>hsim2p05to2p35EffCSCTFnTTcorrEPCut(40,0,40)", "abs(sims_eta)>2.05&&abs(sims_eta)<2.35", "prof");
  
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);
  
  hsim2p05to2p35EffCSCTF->SetLineWidth(2);
  hsim2p05to2p35EffCSCTFnTT->SetLineWidth(2);
  hsim2p05to2p35EffCSCTFnTTcorr->SetLineWidth(2);
  hsim2p05to2p35EffCSCTFnTTcorrEPCut->SetLineWidth(2);

  hsim2p05to2p35EffCSCTF->SetLineColor(2);
  hsim2p05to2p35EffCSCTFnTT->SetLineColor(1);
  hsim2p05to2p35EffCSCTFnTTcorr->SetLineColor(6);
  hsim2p05to2p35EffCSCTFnTTcorrEPCut->SetLineColor(7);

  
  hsim2p05to2p35EffCSCTF->Draw();
  hsim2p05to2p35EffCSCTF->SetTitle("Combined TT-CSCTF single muon efficiency, 2.05<|#eta_{SIM}|<2.35");
  hsim2p05to2p35EffCSCTF->GetXaxis()->SetTitle("p_{T}^{SIM}");
  hsim2p05to2p35EffCSCTF->GetYaxis()->SetTitle("Efficiency");
  hsim2p05to2p35EffCSCTFnTT->Draw("same");
  hsim2p05to2p35EffCSCTFnTTcorr->Draw("same");
  hsim2p05to2p35EffCSCTFnTTcorrEPCut->Draw("same");

  TLegend* leg = new TLegend(0.25, 0.3, 0.99, 0.6);
  leg->SetFillColor(0);
  leg->AddEntry(hsim2p05to2p35EffCSCTF, "TF-SIM");
  leg->AddEntry(hsim2p05to2p35EffCSCTFnTT, "TF-SIM and TT-SIM");
  leg->AddEntry(hsim2p05to2p35EffCSCTFnTTcorr, "TF-SIM and TT-SIM and TT-closest-TF");
  leg->AddEntry(hsim2p05to2p35EffCSCTFnTTcorrEPCut, "Baseline: TF-SIM and TT-SIM and TT-closest-TF and in #Delta#eta#Delta#phi");
  leg->Draw();
  
  gPad->SaveAs("cms/emu/June1462X/hsim2p05to2p35Eff_baselineVsteps.png");
  gPad->SaveAs("cms/emu/June1462X/hsim2p05to2p35Eff_baselineVsteps.pdf");
  gPad->SaveAs("cms/emu/June1462X/hsim2p05to2p35Eff_baselineVsteps.eps");

}

void getEfficiencyTTtoTFcompareOpts(){
  gStyle->SetOptStat(0);
  gStyle->SetTitleSize(0.055, "");
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleAlign(13); // coord in top left
  gStyle->SetTitleX(0.);
  gStyle->SetTitleY(1.);
  gStyle->SetTitleW(1);
  gStyle->SetTitleH(0.06);
  gStyle->SetTitleBorderSize(0);
  
  gStyle->SetPadLeftMargin(0.126);
  gStyle->SetPadRightMargin(0.04);
  gStyle->SetPadTopMargin(0.065);
  gStyle->SetPadBottomMargin(0.13);
  gStyle->SetOptStat(0);
  gStyle->SetMarkerStyle(1);


  TChain* sig = new TChain("Events");
  sig->Add("/Data/cms/emu/phase2/CMSSW_6_2_0_SLHC14-ttiRedigiJun14ana/SingleMuPlusFlatPt0p2To150/TTI2023Upg14D-PU140bx25-ILT_SLHC14ttnt-dd93c9b-redoL1MuPhase1/5cac9a74090da3b2068f65a6d15fa6ca/tt_ntuple_redoL1Mu_*.root");
  sig->Add("/Data/cms/emu/phase2/CMSSW_6_2_0_SLHC14-ttiRedigiJun14ana/SingleMuMinusFlatPt0p2To150/TTI2023Upg14D-PU140bx25-ILT_SLHC14ttnt-dd93c9b-redoL1MuPhase1/5cac9a74090da3b2068f65a6d15fa6ca/tt_ntuple_redoL1Mu_*.root");


  TChain* sigp2 = new TChain("Events");
  sigp2->Add("/Data/cms/emu/phase2/CMSSW_6_2_0_SLHC14-ttiRedigiJun14ana/SingleMuPlusFlatPt0p2To150/TTI2023Upg14D-PU140bx25-ILT_SLHC14ttnt-dd93c9b-redoL1MuPhase2/cbe86603d425084d1b6f04e25036d6ce/tt_ntuple_redoL1Mu_*.root");
  sigp2->Add("/Data/cms/emu/phase2/CMSSW_6_2_0_SLHC14-ttiRedigiJun14ana/SingleMuMinusFlatPt0p2To150/TTI2023Upg14D-PU140bx25-ILT_SLHC14ttnt-dd93c9b-redoL1MuPhase2/cbe86603d425084d1b6f04e25036d6ce/tt_ntuple_redoL1Mu_*.root");


  float xbins[27] = {0, 
		     1, 1.5,  2, 2.5,  3,  4,  5,  6,  7,  8, 
		     9,  10, 12,  14, 16, 20, 25, 30, 35, 40, 
		     50, 60, 70,  80, 90, 100};
  TProfile* hsim1p2to2p4EffTFTTBaseline = new TProfile("hsim1p2to2p4EffTFTTBaseline", "hsim1p2to2p4EffTFTTBaseline",
						       26,  xbins);
  TProfile* hsim1p2to2p4EffTFTTBaselineWME1 = new TProfile("hsim1p2to2p4EffTFTTBaselineWME1", "hsim1p2to2p4EffTFTTBaselineWME1",
						       26,  xbins);

  float xbinsTT20[20] = {5, 6, 7, 8, 10,  
			 15, 18, 19, 19.5, 19.75, 20.0, 20.5, 21, 25,  
			 30, 35, 40, 45, 49, 50 };
  TProfile* hsim1p2to2p4EffTFTTBaselineTT20 = new TProfile("hsim1p2to2p4EffTFTTBaselineTT20", "hsim1p2to2p4EffTFTTBaselineTT20",
							   19,  xbinsTT20);
  TProfile* hsim1p2to2p4EffTFTTBaselineWME1TT20 = new TProfile("hsim1p2to2p4EffTFTTBaselineWME1TT20", "hsim1p2to2p4EffTFTTBaselineWME1TT20",
							       19,  xbinsTT20);
  TProfile* hp2sim1p2to2p4EffTFTTBaselineTT20 = new TProfile("hp2sim1p2to2p4EffTFTTBaselineTT20", "hp2sim1p2to2p4EffTFTTBaselineTT20",
							     19,  xbinsTT20);
  TProfile* hp2sim1p2to2p4EffTFTTBaselineWME1TT20 = new TProfile("hp2sim1p2to2p4EffTFTTBaselineWME1TT20", "hp2sim1p2to2p4EffTFTTBaselineWME1TT20",
								 19,  xbinsTT20);
  TProfile* hp2sim1p2to2p4EffTFTTBaselineWME1dPhiTT20 = new TProfile("hp2sim1p2to2p4EffTFTTBaselineWME1dPhiTT20", "hp2sim1p2to2p4EffTFTTBaselineWME1dPhiTT20",
								     19,  xbinsTT20);

  //some variations for TP
  TProfile* hsim1p1to2p4EffTFTTBaselineTT20 = new TProfile("hsim1p1to2p4EffTFTTBaselineTT20", "hsim1p1to2p4EffTFTTBaselineTT20",
							   19,  xbinsTT20);
  TProfile* hsim1p1to2p2EffTFTTBaselineTT20 = new TProfile("hsim1p1to2p2EffTFTTBaselineTT20", "hsim1p1to2p2EffTFTTBaselineTT20",
							   19,  xbinsTT20);
  TProfile* hsim0to1p1EffTFTTBaselineTT20 = new TProfile("hsim0to1p1EffTFTTBaselineTT20", "hsim0to1p1EffTFTTBaselineTT20",
							 19,  xbinsTT20);

  TProfile* hsim1p6to2p1EffTFTTBaselineMuChTT20 = new TProfile("hsim1p6to2p1EffTFTTBaselineMuChTT20", "hsim1p6to2p1EffTFTTBaselineMuChTT20",
							       19,  xbinsTT20);
  TProfile* hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDecoTT20 = new TProfile("hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDecoTT20", 
									    "hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDecoTT20",
									    19,  xbinsTT20);
  float xbins5to50[16] = {5,  6,  7,  8,  9,  10, 12,  14, 16, 20, 
			  25, 30, 35, 40, 49, 50};
  TProfile* hsim1p6to2p1EffTFTTBaselineMuCh = new TProfile("hsim1p6to2p1EffTFTTBaselineMuCh", "hsim1p6to2p1EffTFTTBaselineMuCh",
							   15,  xbins5to50);
  TProfile* hp2sim1p6to2p1EffTFTTBaselineMuCh = new TProfile("hp2sim1p6to2p1EffTFTTBaselineMuCh", "hp2sim1p6to2p1EffTFTTBaselineMuCh",
							     15,  xbins5to50);
  TProfile* hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDeco = new TProfile("hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDeco", 
									"hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDeco",
									15,  xbins5to50);
  TProfile* hp2sim1p6to2p1EffTFTTBaselineTF0p5PTor20WS1dPhiMuChDeco = new TProfile("hp2sim1p6to2p1EffTFTTBaselineTF0p5PTor20WS1dPhiMuChDeco", 
									"hp2sim1p6to2p1EffTFTTBaselineTF0p5PTor20WS1dPhiMuChDeco",
									15,  xbins5to50);
  
  float xbins50[23] = {0, 
		     1, 1.5,  2, 2.5,  3,  4,  5,  6,  7,  8, 
		     9,  10, 12,  14, 16, 20, 25, 30, 35, 40, 
		       49, 50};
  TProfile* hsim1p1to2p4EffTFTTBaseline = new TProfile("hsim1p1to2p4EffTFTTBaseline", "hsim1p1to2p4EffTFTTBaseline",
						       22,  xbins50);
  TProfile* hsim1p1to2p2EffTFTTBaseline = new TProfile("hsim1p1to2p2EffTFTTBaseline", "hsim1p1to2p2EffTFTTBaseline",
						       22,  xbins50);
  TProfile* hsim0to1p1EffTFTTBaseline = new TProfile("hsim0to1p1EffTFTTBaseline", "hsim0to1p1EffTFTTBaseline",
						     22,  xbins50);

  float xbinsTF20[20] = {5, 6, 7, 8,  10, 12, 14, 16, 18, 20, 22, 
			 24, 26, 28, 30, 35, 40, 45, 49, 50};

  //			 30, 35, 40, 45, 50};
  //, 60, 70, 80, 90, 100, 			 120, 140, 150};
  //some variations for TP
  TProfile* hsim1p1to2p4EffTFBaselineTF20 = new TProfile("hsim1p1to2p4EffTFBaselineTF20", "hsim1p1to2p4EffTFBaselineTF20",
							 19,  xbinsTF20);
  TProfile* hsim1p1to2p1EffTFBaselineTF20 = new TProfile("hsim1p1to2p1EffTFBaselineTF20", "hsim1p1to2p1EffTFBaselineTF20",
							 19,  xbinsTF20);
  TProfile* hsim1p1to2p2EffTFBaselineTF20 = new TProfile("hsim1p1to2p2EffTFBaselineTF20", "hsim1p1to2p2EffTFBaselineTF20",
							 19,  xbinsTF20);
  TProfile* hsim0to1p1EffTFBaselineTF20 = new TProfile("hsim0to1p1EffTFBaselineTF20", "hsim0to1p1EffTFBaselineTF20",
						       19,  xbinsTF20);

  TProfile* hsim1p1to2p4EffTFBaseline = new TProfile("hsim1p1to2p4EffTFBaseline", "hsim1p1to2p4EffTFBaseline",
						     22,  xbins50);
  TProfile* hsim1p1to2p2EffTFBaseline = new TProfile("hsim1p1to2p2EffTFBaseline", "hsim1p1to2p2EffTFBaseline",
						     22,  xbins50);
  TProfile* hsim0to1p1EffTFBaseline = new TProfile("hsim0to1p1EffTFBaseline", "hsim0to1p1EffTFBaseline",
						   22,  xbins50);
  
  bool makeCompBasedPlots = false;
  if (makeCompBasedPlots){
  // all endcap: 1.2 to 2.4
  sig->Draw("sims_tt_dr_idx>=0&&sims_tt_dr_dr<0.02\
&&tts_csctf_drs2_idx[sims_tt_dr_idx]>=0 &&sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2\
&&abs(csctfs_eta[sims_csctf_drs2_idx])>=1.2&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.4&&csctfs_bx[sims_csctf_drs2_idx]==0\
&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]>=0&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]==sims_tt_dr_idx&&tts_csctf_deps2_idx[sims_tt_dr_idx]>=0\
:sims_pt>>hsim1p2to2p4EffTFTTBaseline", "abs(sims_eta)>1.2&&abs(sims_eta)<2.4&&sims_pt>1.0", "prof");

  sig->Draw("sims_tt_dr_idx>=0&&sims_tt_dr_dr<0.02\
&&tts_csctf_drs2_idx[sims_tt_dr_idx]>=0 &&sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2\
&&abs(csctfs_eta[sims_csctf_drs2_idx])>=1.2&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.4&&csctfs_bx[sims_csctf_drs2_idx]==0\
&&csctfs_nlcts[sims_csctf_drs2_idx]>=2&&csctfs_ringS1[sims_csctf_drs2_idx]>0\
&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]>=0&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]==sims_tt_dr_idx&&tts_csctf_deps2_idx[sims_tt_dr_idx]>=0\
:sims_pt>>hsim1p2to2p4EffTFTTBaselineWME1", "abs(sims_eta)>1.2&&abs(sims_eta)<2.4&&sims_pt>1.0", "prof");


  // all endcap: 1.2 to 2.4
  sig->Draw("sims_tt_dr_idx>=0&&sims_tt_dr_dr<0.02\
&&tts_csctf_drs2_idx[sims_tt_dr_idx]>=0 &&sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2\
&&abs(csctfs_eta[sims_csctf_drs2_idx])>=1.2&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.4&&csctfs_bx[sims_csctf_drs2_idx]==0\
&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]>=0&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]==sims_tt_dr_idx&&tts_csctf_deps2_idx[sims_tt_dr_idx]>=0\
&&tts_pt[sims_tt_dr_idx]>20\
:min(sims_pt,49.99)>>hsim1p2to2p4EffTFTTBaselineTT20", "abs(sims_eta)>1.2&&abs(sims_eta)<2.4&&sims_pt>1.0", "prof");

  sig->Draw("sims_tt_dr_idx>=0&&sims_tt_dr_dr<0.02\
&&tts_csctf_drs2_idx[sims_tt_dr_idx]>=0 &&sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2\
&&abs(csctfs_eta[sims_csctf_drs2_idx])>=1.2&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.4&&csctfs_bx[sims_csctf_drs2_idx]==0\
&&csctfs_nlcts[sims_csctf_drs2_idx]>=2&&csctfs_ringS1[sims_csctf_drs2_idx]>0\
&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]>=0&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]==sims_tt_dr_idx&&tts_csctf_deps2_idx[sims_tt_dr_idx]>=0\
&&tts_pt[sims_tt_dr_idx]>20\
:min(sims_pt,49.99)>>hsim1p2to2p4EffTFTTBaselineWME1TT20", "abs(sims_eta)>1.2&&abs(sims_eta)<2.4&&sims_pt>1.0", "prof");

  sigp2->Draw("sims_tt_dr_idx>=0&&sims_tt_dr_dr<0.02\
&&tts_csctf_drs2_idx[sims_tt_dr_idx]>=0 &&sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2\
&&abs(csctfs_eta[sims_csctf_drs2_idx])>=1.2&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.4&&csctfs_bx[sims_csctf_drs2_idx]==0\
&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]>=0&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]==sims_tt_dr_idx&&tts_csctf_deps2_idx[sims_tt_dr_idx]>=0\
&&tts_pt[sims_tt_dr_idx]>20\
:min(sims_pt,49.99)>>hp2sim1p2to2p4EffTFTTBaselineTT20", "abs(sims_eta)>1.2&&abs(sims_eta)<2.4&&sims_pt>1.0", "prof");

  sigp2->Draw("sims_tt_dr_idx>=0&&sims_tt_dr_dr<0.02\
&&tts_csctf_drs2_idx[sims_tt_dr_idx]>=0 &&sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2\
&&abs(csctfs_eta[sims_csctf_drs2_idx])>=1.2&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.4&&csctfs_bx[sims_csctf_drs2_idx]==0\
&&csctfs_nlcts[sims_csctf_drs2_idx]>=2&&csctfs_ringS1[sims_csctf_drs2_idx]>0\
&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]>=0&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]==sims_tt_dr_idx&&tts_csctf_deps2_idx[sims_tt_dr_idx]>=0\
&&tts_pt[sims_tt_dr_idx]>20\
:min(sims_pt,49.99)>>hp2sim1p2to2p4EffTFTTBaselineWME1TT20", "abs(sims_eta)>1.2&&abs(sims_eta)<2.4&&sims_pt>1.0", "prof");

  sigp2->Draw("sims_tt_dr_idx>=0&&sims_tt_dr_dr<0.02\
&&tts_csctf_drs2_idx[sims_tt_dr_idx]>=0 &&sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2\
&&abs(csctfs_eta[sims_csctf_drs2_idx])>=1.2&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.4&&csctfs_bx[sims_csctf_drs2_idx]==0\
&&csctfs_nlcts[sims_csctf_drs2_idx]>=2&&csctfs_ringS1[sims_csctf_drs2_idx]>0\
&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]>=0&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]==sims_tt_dr_idx&&tts_csctf_deps2_idx[sims_tt_dr_idx]>=0\
&&(abs(csctfs_gemDPhiS1[sims_csctf_drs2_idx])>2||abs(csctfs_eta[sims_csctf_drs2_idx])>2.05||((-tts_q[sims_tt_dr_idx]*csctfs_gemDPhiS1[sims_csctf_drs2_idx]>-2E-3&&-tts_q[sims_tt_dr_idx]*csctfs_gemDPhiS1[sims_csctf_drs2_idx]<sqrt((0.05/tts_pt[sims_tt_dr_idx])**2 + 4E-6))&&csctfs_chamberS1[sims_csctf_drs2_idx]%2==0 )||((-tts_q[sims_tt_dr_idx]*csctfs_gemDPhiS1[sims_csctf_drs2_idx]>-3.3E-3&&-tts_q[sims_tt_dr_idx]*csctfs_gemDPhiS1[sims_csctf_drs2_idx]<sqrt((0.12/tts_pt[sims_tt_dr_idx])**2 + 1.E-5))&&csctfs_chamberS1[sims_csctf_drs2_idx]%2==1 ) )\
&&tts_pt[sims_tt_dr_idx]>20\
:min(sims_pt,49.99)>>hp2sim1p2to2p4EffTFTTBaselineWME1dPhiTT20", "abs(sims_eta)>1.2&&abs(sims_eta)<2.4&&sims_pt>1.0", "prof");
  }

  //some variations for TP
  sig->Draw("sims_l1tkmu_dr_idx>=0&&sims_l1tkmu_dr_dr<0.02\
&&abs(l1tkmus_eta[sims_l1tkmu_dr_idx])>=1.1&&abs(l1tkmus_eta[sims_l1tkmu_dr_idx])<2.4&&l1tkmus_pt[sims_l1tkmu_dr_idx]>20\
:min(sims_pt,49.99)>>hsim1p1to2p4EffTFTTBaselineTT20", "abs(sims_eta)>1.1&&abs(sims_eta)<2.4&&sims_pt>1.0", "prof");

  sig->Draw("sims_l1tkmu_dr_idx>=0&&sims_l1tkmu_dr_dr<0.02\
&&abs(l1tkmus_eta[sims_l1tkmu_dr_idx])>=1.1&&abs(l1tkmus_eta[sims_l1tkmu_dr_idx])<2.2&&l1tkmus_pt[sims_l1tkmu_dr_idx]>20\
:min(sims_pt,49.99)>>hsim1p1to2p2EffTFTTBaselineTT20", "abs(sims_eta)>1.1&&abs(sims_eta)<2.2&&sims_pt>1.0", "prof");

  sig->Draw("sims_l1tkmu_dr_idx>=0&&sims_l1tkmu_dr_dr<0.02\
&&abs(l1tkmus_eta[sims_l1tkmu_dr_idx])<=1.1&&l1tkmus_pt[sims_l1tkmu_dr_idx]>20\
:min(sims_pt,49.99)>>hsim0to1p1EffTFTTBaselineTT20", "abs(sims_eta)<1.1&&sims_pt>1.0", "prof");
  //no cut
  sig->Draw("sims_l1tkmu_dr_idx>=0&&sims_l1tkmu_dr_dr<0.02\
&&abs(l1tkmus_eta[sims_l1tkmu_dr_idx])>=1.1&&abs(l1tkmus_eta[sims_l1tkmu_dr_idx])<2.4\
:min(sims_pt,49.99)>>hsim1p1to2p4EffTFTTBaseline", "abs(sims_eta)>1.1&&abs(sims_eta)<2.4&&sims_pt>1.0", "prof");

  sig->Draw("sims_l1tkmu_dr_idx>=0&&sims_l1tkmu_dr_dr<0.02\
&&abs(l1tkmus_eta[sims_l1tkmu_dr_idx])>=1.1&&abs(l1tkmus_eta[sims_l1tkmu_dr_idx])<2.2\
:min(sims_pt,49.99)>>hsim1p1to2p2EffTFTTBaseline", "abs(sims_eta)>1.1&&abs(sims_eta)<2.2&&sims_pt>1.0", "prof");

  sig->Draw("sims_l1tkmu_dr_idx>=0&&sims_l1tkmu_dr_dr<0.02\
&&abs(l1tkmus_eta[sims_l1tkmu_dr_idx])<=1.1\
:min(sims_pt,49.99)>>hsim0to1p1EffTFTTBaseline", "abs(sims_eta)<1.1&&sims_pt>1.0", "prof");

  //"muChOpt2" plots
  sig->Draw("sims_l1tkmu_dr_idx>=0&&sims_l1tkmu_dr_dr<0.02\
&&abs(l1tkmus_eta[sims_l1tkmu_dr_idx])>=1.6&&abs(l1tkmus_eta[sims_l1tkmu_dr_idx])<2.1\
:min(sims_pt,49.99)>>hsim1p6to2p1EffTFTTBaselineMuCh", "abs(sims_eta)>1.6&&abs(sims_eta)<2.1&&sims_pt>1.0", "prof");
  sigp2->Draw("sims_l1tkmu_dr_idx>=0&&sims_l1tkmu_dr_dr<0.02\
&&abs(l1tkmus_eta[sims_l1tkmu_dr_idx])>=1.6&&abs(l1tkmus_eta[sims_l1tkmu_dr_idx])<2.1\
:min(sims_pt,49.99)>>hp2sim1p6to2p1EffTFTTBaselineMuCh", "abs(sims_eta)>1.6&&abs(sims_eta)<2.1&&sims_pt>1.0", "prof");
  sig->Draw("sims_l1tkmu_dr_idx>=0&&sims_l1tkmu_dr_dr<0.02\
&&abs(l1tkmus_eta[sims_l1tkmu_dr_idx])>=1.6&&abs(l1tkmus_eta[sims_l1tkmu_dr_idx])<2.1&&l1tkmus_pt[sims_l1tkmu_dr_idx]>20\
:min(sims_pt,49.99)>>hsim1p6to2p1EffTFTTBaselineMuChTT20", "abs(sims_eta)>1.6&&abs(sims_eta)<2.1&&sims_pt>1.0", "prof");

  sigp2->Draw("sims_tt_dr_idx>=0&&sims_tt_dr_dr<0.02\
&&tts_csctf_drs2_idx[sims_tt_dr_idx]>=0 &&sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2\
&&abs(csctfs_eta[sims_csctf_drs2_idx])>=1.6&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.1&&csctfs_bx[sims_csctf_drs2_idx]==0\
&&csctfs_nlcts[sims_csctf_drs2_idx]>=2&&csctfs_ringS1[sims_csctf_drs2_idx]>0\
&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]>=0&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]==sims_tt_dr_idx&&tts_csctf_deps2_idx[sims_tt_dr_idx]>=0\
&&(csctfs_gemDPhiS1[sims_csctf_drs2_idx]<-2||abs(csctfs_eta[sims_csctf_drs2_idx])>2.05||((-tts_q[sims_tt_dr_idx]*csctfs_gemDPhiS1[sims_csctf_drs2_idx]>-2E-3&&-tts_q[sims_tt_dr_idx]*csctfs_gemDPhiS1[sims_csctf_drs2_idx]<sqrt((0.05/tts_pt[sims_tt_dr_idx])**2 + 4E-6))&&csctfs_chamberS1[sims_csctf_drs2_idx]%2==0 )||((-tts_q[sims_tt_dr_idx]*csctfs_gemDPhiS1[sims_csctf_drs2_idx]>-3.3E-3&&-tts_q[sims_tt_dr_idx]*csctfs_gemDPhiS1[sims_csctf_drs2_idx]<sqrt((0.12/tts_pt[sims_tt_dr_idx])**2 + 1.E-5))&&csctfs_chamberS1[sims_csctf_drs2_idx]%2==1 ) )\
:min(sims_pt,49.99)>>hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDeco", "abs(sims_eta)>1.6&&abs(sims_eta)<2.1&&sims_pt>1.0", "prof");

  sigp2->Draw("sims_tt_dr_idx>=0&&sims_tt_dr_dr<0.02\
&&tts_csctf_drs2_idx[sims_tt_dr_idx]>=0 &&sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2\
&&abs(csctfs_eta[sims_csctf_drs2_idx])>=1.6&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.1&&csctfs_bx[sims_csctf_drs2_idx]==0\
&&csctfs_nlcts[sims_csctf_drs2_idx]>=2&&csctfs_ringS1[sims_csctf_drs2_idx]>0\
&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]>=0&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]==sims_tt_dr_idx&&tts_csctf_deps2_idx[sims_tt_dr_idx]>=0\
&&(csctfs_gemDPhiS1[sims_csctf_drs2_idx]<-2||abs(csctfs_eta[sims_csctf_drs2_idx])>2.05||((-tts_q[sims_tt_dr_idx]*csctfs_gemDPhiS1[sims_csctf_drs2_idx]>-2E-3&&-tts_q[sims_tt_dr_idx]*csctfs_gemDPhiS1[sims_csctf_drs2_idx]<sqrt((0.05/tts_pt[sims_tt_dr_idx])**2 + 4E-6))&&csctfs_chamberS1[sims_csctf_drs2_idx]%2==0 )||((-tts_q[sims_tt_dr_idx]*csctfs_gemDPhiS1[sims_csctf_drs2_idx]>-3.3E-3&&-tts_q[sims_tt_dr_idx]*csctfs_gemDPhiS1[sims_csctf_drs2_idx]<sqrt((0.12/tts_pt[sims_tt_dr_idx])**2 + 1.E-5))&&csctfs_chamberS1[sims_csctf_drs2_idx]%2==1 ) )\
&&tts_pt[sims_tt_dr_idx]>20\
:min(sims_pt,49.99)>>hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDecoTT20", "abs(sims_eta)>1.6&&abs(sims_eta)<2.1&&sims_pt>1.0", "prof");


  sigp2->Draw("sims_tt_dr_idx>=0&&sims_tt_dr_dr<0.02\
&&tts_csctf_drs2_idx[sims_tt_dr_idx]>=0 &&sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2\
&&abs(csctfs_eta[sims_csctf_drs2_idx])>=1.6&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.1&&csctfs_bx[sims_csctf_drs2_idx]==0\
&&csctfs_nlcts[sims_csctf_drs2_idx]>=2&&csctfs_ringS1[sims_csctf_drs2_idx]>0\
&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]>=0&&csctfs_tt_drs2_idx[sims_csctf_drs2_idx]==sims_tt_dr_idx&&tts_csctf_deps2_idx[sims_tt_dr_idx]>=0\
&&(csctfs_pt[sims_csctf_drs2_idx]>0.5*tts_pt[sims_tt_dr_idx]||csctfs_pt[sims_csctf_drs2_idx]>20)\
&&(csctfs_gemDPhiS1[sims_csctf_drs2_idx]<-2||abs(csctfs_eta[sims_csctf_drs2_idx])>2.05||((-tts_q[sims_tt_dr_idx]*csctfs_gemDPhiS1[sims_csctf_drs2_idx]>-2E-3&&-tts_q[sims_tt_dr_idx]*csctfs_gemDPhiS1[sims_csctf_drs2_idx]<sqrt((0.05/tts_pt[sims_tt_dr_idx])**2 + 4E-6))&&csctfs_chamberS1[sims_csctf_drs2_idx]%2==0 )||((-tts_q[sims_tt_dr_idx]*csctfs_gemDPhiS1[sims_csctf_drs2_idx]>-3.3E-3&&-tts_q[sims_tt_dr_idx]*csctfs_gemDPhiS1[sims_csctf_drs2_idx]<sqrt((0.12/tts_pt[sims_tt_dr_idx])**2 + 1.E-5))&&csctfs_chamberS1[sims_csctf_drs2_idx]%2==1 ) )\
:min(sims_pt,49.99)>>hp2sim1p6to2p1EffTFTTBaselineTF0p5PTor20WS1dPhiMuChDeco", "abs(sims_eta)>1.6&&abs(sims_eta)<2.1&&sims_pt>1.0", "prof");


  //GMT/CSCTF only
  sig->Draw("sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2\
&&abs(csctfs_eta[sims_csctf_drs2_idx])>=1.1&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.4&&csctfs_pt[sims_csctf_drs2_idx]>20\
&&csctfs_bx[sims_csctf_drs2_idx]==0\
:min(sims_pt,49.99)>>hsim1p1to2p4EffTFBaselineTF20", "abs(sims_eta)>1.1&&abs(sims_eta)<2.4&&sims_pt>1.0", "prof");

  sig->Draw("sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2\
&&abs(csctfs_eta[sims_csctf_drs2_idx])>=1.1&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.2&&csctfs_pt[sims_csctf_drs2_idx]>20\
&&csctfs_bx[sims_csctf_drs2_idx]==0\
&&sims_gmt_drs2_idx>=0&&sims_gmt_drs2_dr<0.2\
&&gmts_quality[sims_gmt_drs2_idx]>=4&&gmts_quality[sims_gmt_drs2_idx]!=5\
:min(sims_pt,49.99)>>hsim1p1to2p2EffTFBaselineTF20", "abs(sims_eta)>1.1&&abs(sims_eta)<2.2&&sims_pt>1.0", "prof");

  sig->Draw("sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2\
&&abs(csctfs_eta[sims_csctf_drs2_idx])>=1.1&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.1&&csctfs_pt[sims_csctf_drs2_idx]>20\
&&csctfs_bx[sims_csctf_drs2_idx]==0\
&&sims_gmt_drs2_idx>=0&&sims_gmt_drs2_dr<0.2\
&&gmts_quality[sims_gmt_drs2_idx]>=4&&gmts_quality[sims_gmt_drs2_idx]!=5\
:min(sims_pt,49.99)>>hsim1p1to2p1EffTFBaselineTF20", "abs(sims_eta)>1.1&&abs(sims_eta)<2.1&&sims_pt>1.0", "prof");

  sig->Draw("sims_gmt_drs2_idx>=0&&sims_gmt_drs2_dr<0.2\
&&abs(gmts_eta[sims_gmt_drs2_idx])<=1.1&&gmts_pt[sims_gmt_drs2_idx]>20\
&&gmts_bx[sims_gmt_drs2_idx]==0&&gmts_quality[sims_gmt_drs2_idx]>=4&&gmts_quality[sims_gmt_drs2_idx]!=5\
:min(sims_pt,49.99)>>hsim0to1p1EffTFBaselineTF20", "abs(sims_eta)<1.1&&sims_pt>1.0", "prof");
  //no pt cut
  sig->Draw("sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2\
&&abs(csctfs_eta[sims_csctf_drs2_idx])>=1.1&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.4\
&&csctfs_bx[sims_csctf_drs2_idx]==0\
:min(sims_pt,49.99)>>hsim1p1to2p4EffTFBaseline", "abs(sims_eta)>1.1&&abs(sims_eta)<2.4&&sims_pt>1.0", "prof");

  sig->Draw("sims_csctf_drs2_idx>=0&&sims_csctf_drs2_dr<0.2\
&&abs(csctfs_eta[sims_csctf_drs2_idx])>=1.1&&abs(csctfs_eta[sims_csctf_drs2_idx])<2.2\
&&csctfs_bx[sims_csctf_drs2_idx]==0\
&&sims_gmt_drs2_idx>=0&&sims_gmt_drs2_dr<0.2\
&&gmts_quality[sims_gmt_drs2_idx]>=4&&gmts_quality[sims_gmt_drs2_idx]!=5\
:min(sims_pt,49.99)>>hsim1p1to2p2EffTFBaseline", "abs(sims_eta)>1.1&&abs(sims_eta)<2.2&&sims_pt>1.0", "prof");

  sig->Draw("sims_gmt_drs2_idx>=0&&sims_gmt_drs2_dr<0.2\
&&abs(gmts_eta[sims_gmt_drs2_idx])<=1.1\
&&gmts_bx[sims_gmt_drs2_idx]==0&&gmts_quality[sims_gmt_drs2_idx]>=4&&gmts_quality[sims_gmt_drs2_idx]!=5\
:min(sims_pt,49.99)>>hsim0to1p1EffTFBaseline", "abs(sims_eta)<1.1&&sims_pt>1.0", "prof");


  hsim1p2to2p4EffTFTTBaseline->SetLineWidth(2);
  hsim1p2to2p4EffTFTTBaselineWME1->SetLineWidth(2);

  hsim1p2to2p4EffTFTTBaseline->SetLineColor(7);
  hsim1p2to2p4EffTFTTBaselineWME1->SetLineColor(6);
  hsim1p2to2p4EffTFTTBaseline->SetFillColor(7);
  hsim1p2to2p4EffTFTTBaselineWME1->SetFillColor(6);

  hsim1p2to2p4EffTFTTBaselineTT20->SetLineWidth(2);
  hsim1p2to2p4EffTFTTBaselineWME1TT20->SetLineWidth(2);

  hsim1p2to2p4EffTFTTBaselineTT20->SetLineColor(7);
  hsim1p2to2p4EffTFTTBaselineWME1TT20->SetLineColor(6);
  hsim1p2to2p4EffTFTTBaselineTT20->SetFillColor(7);
  hsim1p2to2p4EffTFTTBaselineWME1TT20->SetFillColor(6);

  hp2sim1p2to2p4EffTFTTBaselineTT20->SetLineWidth(2);
  hp2sim1p2to2p4EffTFTTBaselineTT20->SetLineColor(4);
  hp2sim1p2to2p4EffTFTTBaselineTT20->SetFillColor(4);
  hp2sim1p2to2p4EffTFTTBaselineWME1TT20->SetLineWidth(2);
  hp2sim1p2to2p4EffTFTTBaselineWME1TT20->SetLineColor(3);
  hp2sim1p2to2p4EffTFTTBaselineWME1TT20->SetFillColor(3);
  hp2sim1p2to2p4EffTFTTBaselineWME1dPhiTT20->SetLineWidth(2);
  hp2sim1p2to2p4EffTFTTBaselineWME1dPhiTT20->SetLineColor(2);
  hp2sim1p2to2p4EffTFTTBaselineWME1dPhiTT20->SetFillColor(2);


  c = new TCanvas("c","c",700,450);
  c->Clear();
  
  if (makeCompBasedPlots){
    hsim1p2to2p4EffTFTTBaseline->Draw("e3");
    TProfile* h = hsim1p2to2p4EffTFTTBaseline;
    h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
    h->GetXaxis()->SetTitle("Simulated muon p_{T}");
    h->GetYaxis()->SetTitle("Efficiency");
    h->SetTitleSize(0.05);
    h->SetTitleOffset(1.1, "X");
    h->SetTitleOffset(0.84, "Y");
    h->GetXaxis()->SetLabelSize(0.05);
    h->GetYaxis()->SetLabelSize(0.05);
    h->GetXaxis()->SetTitleSize(0.055);
    h->GetYaxis()->SetTitleSize(0.06);
    h->GetXaxis()->SetNdivisions(302);
    h->GetXaxis()->SetMoreLogLabels();
    tex0 = new TLatex(10, -0.078, "10");
    tex0->SetTextFont(42);
    tex0->SetTextSize(0.05);
    tex0->Draw();
    tp = new TPave(0.4, 0.43, 0.95, 0.5, 0, "NDC"); tp->SetFillColor(0);
    tp->Draw();
    tex1 = new TLatex(.41,.45,"L1 Single TkMu efficiency 1.2<|#eta|<2.4");
    tex1->SetTextFont(42);
    tex1->SetTextSize(0.05);
    tex1->SetNDC();
    tex1->Draw();
    gPad->SetLogx();

    hsim1p2to2p4EffTFTTBaselineWME1->Draw("same e3");

    gPad->SetGridx();
    gPad->SetGridy();
    gPad->SetTickx(1);
    gPad->SetTicky(1);

    TLegend* leg = new TLegend(0.4, 0.3, 0.95, 0.44);
    leg->SetFillColor(0); leg->SetBorderSize(0);
    leg->SetTextSize(0.048); leg->SetMargin(0.13);
    leg->AddEntry(hsim1p2to2p4EffTFTTBaseline, "Phase-1 muon: 2+ stubs in CSCTF");
    leg->AddEntry(hsim1p2to2p4EffTFTTBaselineWME1, "Phase-1 muon: 2+ stubs with YE-1 in CSCTF");
    leg->Draw();
  
    gPad->SaveAs("cms/emu/June1462X/hsim1p2to2p4Eff_baselineVME1.png");
    gPad->SaveAs("cms/emu/June1462X/hsim1p2to2p4Eff_baselineVME1.pdf");
    gPad->SaveAs("cms/emu/June1462X/hsim1p2to2p4Eff_baselineVME1.eps");

    //
    hsim1p2to2p4EffTFTTBaseline->Draw("e3");
    tex0 = new TLatex(10, -0.078, "10");
    tex0->SetTextFont(42);
    tex0->SetTextSize(0.05);
    tex0->Draw();
    tp = new TPave(0.4, 0.43, 0.95, 0.5, 0, "NDC"); tp->SetFillColor(0);
    tp->Draw();
    tex1 = new TLatex(.41,.45,"L1 Single TkMu efficiency 1.2<|#eta|<2.4");
    tex1->SetTextFont(42);
    tex1->SetTextSize(0.05);
    tex1->SetNDC();
    tex1->Draw();
    gPad->SetLogx();

    hsim1p2to2p4EffTFTTBaselineWME1->Draw("same e3");

    TH1F* htmp = new TH1F("htmp", "htmp", 10,-10,0);
    htmp->SetFillColor(2);

    TLegend* leg = new TLegend(0.4, 0.3, 0.95, 0.44);
    leg->SetFillColor(0); leg->SetBorderSize(0);
    leg->SetTextSize(0.04); leg->SetMargin(0.13);
    leg->AddEntry(hsim1p2to2p4EffTFTTBaseline, "Phase-1 muon: 2+ stubs in CSCTF");
    leg->AddEntry(hsim1p2to2p4EffTFTTBaselineWME1, "Phase-1 muon: 2+ stubs with YE-1 in CSCTF");
    leg->AddEntry(htmp, "Phase-2 muon: 2+ stubs with YE-1 angle cut (soon)");
    leg->Draw();
  
    gPad->SaveAs("cms/emu/June1462X/hsim1p2to2p4Eff_baselineVME1_tmp.png");
    gPad->SaveAs("cms/emu/June1462X/hsim1p2to2p4Eff_baselineVME1_tmp.pdf");
    gPad->SaveAs("cms/emu/June1462X/hsim1p2to2p4Eff_baselineVME1_tmp.eps");

    // NOW TT20 plots
    cTT20 = new TCanvas("cTT20","cTT20",700,450);
    cTT20->Clear();
    gPad->SetLogx(0);
  
    hsim1p2to2p4EffTFTTBaselineTT20->Draw("e3");
    TProfile* h = hsim1p2to2p4EffTFTTBaselineTT20;
    h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
    h->GetXaxis()->SetTitle("Simulated muon p_{T}");
    h->GetYaxis()->SetTitle("Efficiency");
    h->SetTitleSize(0.05);
    h->SetTitleOffset(1.1, "X");
    h->SetTitleOffset(0.84, "Y");
    h->GetXaxis()->SetLabelSize(0.05);
    h->GetYaxis()->SetLabelSize(0.05);
    h->GetXaxis()->SetTitleSize(0.055);
    h->GetYaxis()->SetTitleSize(0.06);
    tp = new TPave(0.4, 0.71, 0.95, 0.78, 0, "NDC"); tp->SetFillColor(0);
    tp->Draw();
    tex1 = new TLatex(.41,.73,"p_{T}^{trig}> 20 GeV, 1.2<|#eta|<2.4 and #geq2 CSCTF stubs");
    tex1->SetTextFont(42);
    tex1->SetTextSize(0.042);
    tex1->SetNDC();
    tex1->Draw();

    hsim1p2to2p4EffTFTTBaselineWME1TT20->Draw("same e3");

    gPad->SetGridx();
    gPad->SetGridy();
    gPad->SetTickx(1);
    gPad->SetTicky(1);

    TLegend* leg = new TLegend(0.4, 0.56, 0.95, 0.7);
    leg->SetFillColor(0); leg->SetBorderSize(0);
    leg->SetTextSize(0.04); leg->SetMargin(0.13);
    leg->AddEntry(hsim1p2to2p4EffTFTTBaselineTT20, "Phase-1 muon");
    leg->AddEntry(hsim1p2to2p4EffTFTTBaselineWME1TT20, "Phase-1 muon with YE-1 required");
    leg->Draw();
  
    gPad->SaveAs("cms/emu/June1462X/hsim1p2to2p4EffTT20_baselineVME1.png");
    gPad->SaveAs("cms/emu/June1462X/hsim1p2to2p4EffTT20_baselineVME1.pdf");
    gPad->SaveAs("cms/emu/June1462X/hsim1p2to2p4EffTT20_baselineVME1.eps");

    //
    hsim1p2to2p4EffTFTTBaselineTT20->Draw("e3");
    tp = new TPave(0.4, 0.71, 0.95, 0.78, 0, "NDC"); tp->SetFillColor(0);
    tp->Draw();
    tex1 = new TLatex(.41,.73,"p_{T}^{trig}> 20 GeV, 1.2<|#eta|<2.4 and #geq 2 CSCTF stubs");
    tex1->SetTextFont(42);
    tex1->SetTextSize(0.042);
    tex1->SetNDC();
    tex1->Draw();
    gPad->SetLogx(0);

    hsim1p2to2p4EffTFTTBaselineWME1TT20->Draw("same e3");
    hp2sim1p2to2p4EffTFTTBaselineTT20->Draw("same e3");
    hp2sim1p2to2p4EffTFTTBaselineWME1TT20->Draw("same e3");
    hp2sim1p2to2p4EffTFTTBaselineWME1dPhiTT20->Draw("same e3");

    TLegend* leg = new TLegend(0.4, 0.46, 0.95, 0.7);
    leg->SetFillColor(0); leg->SetBorderSize(0);
    leg->SetTextSize(0.04); leg->SetMargin(0.13);
    leg->AddEntry(hsim1p2to2p4EffTFTTBaselineTT20, "Phase-1 muon");
    leg->AddEntry(hsim1p2to2p4EffTFTTBaselineWME1TT20, "Phase-1 muon with YE-1 required");
    leg->AddEntry(hp2sim1p2to2p4EffTFTTBaselineTT20, "Phase-2 muon");
    leg->AddEntry(hp2sim1p2to2p4EffTFTTBaselineWME1TT20, "Phase-2 muon with YE-1 required");
    leg->AddEntry(hp2sim1p2to2p4EffTFTTBaselineWME1dPhiTT20, "Phase-2 muon with YE-1 angle cut (*)");
    leg->Draw();
  
    gPad->SaveAs("cms/emu/June1462X/hsim1p2to2p4EffTT20_baselineVME1_5opts.png");
    gPad->SaveAs("cms/emu/June1462X/hsim1p2to2p4EffTT20_baselineVME1_5opts.pdf");
    gPad->SaveAs("cms/emu/June1462X/hsim1p2to2p4EffTT20_baselineVME1_5opts.eps");

    //
    hsim1p2to2p4EffTFTTBaselineTT20->Draw("e3");
    tp = new TPave(0.4, 0.71, 0.95, 0.78, 0, "NDC"); tp->SetFillColor(0);
    tp->Draw();
    tex1 = new TLatex(.41,.73,"p_{T}^{trig}> 20 GeV, 1.2<|#eta|<2.4 and #geq 2 CSCTF stubs");
    tex1->SetTextFont(42);
    tex1->SetTextSize(0.042);
    tex1->SetNDC();
    tex1->Draw();
    gPad->SetLogx(0);

    //  hsim1p2to2p4EffTFTTBaselineWME1TT20->Draw("same e3");
    hp2sim1p2to2p4EffTFTTBaselineTT20->Draw("same e3");
    //  hp2sim1p2to2p4EffTFTTBaselineWME1TT20->Draw("same e3");
    //  hp2sim1p2to2p4EffTFTTBaselineWME1dPhiTT20->Draw("same e3");

    TLegend* leg = new TLegend(0.4, 0.56, 0.95, 0.7);
    leg->SetFillColor(0); leg->SetBorderSize(0);
    leg->SetTextSize(0.04); leg->SetMargin(0.13);
    leg->AddEntry(hsim1p2to2p4EffTFTTBaselineTT20, "Phase-1 muon");
    //  leg->AddEntry(hsim1p2to2p4EffTFTTBaselineWME1TT20, "Phase-1 muon with YE-1 required");
    leg->AddEntry(hp2sim1p2to2p4EffTFTTBaselineTT20, "Phase-2 muon");
    //  leg->AddEntry(hp2sim1p2to2p4EffTFTTBaselineWME1TT20, "Phase-2 muon with YE-1 required");
    //  leg->AddEntry(hp2sim1p2to2p4EffTFTTBaselineWME1dPhiTT20, "Phase-2 muon with YE-1 angle cut (*)");
    leg->Draw();
  
    gPad->SaveAs("cms/emu/June1462X/hsim1p2to2p4EffTT20_baselineVME1_2opts.png");
    gPad->SaveAs("cms/emu/June1462X/hsim1p2to2p4EffTT20_baselineVME1_2opts.pdf");
    gPad->SaveAs("cms/emu/June1462X/hsim1p2to2p4EffTT20_baselineVME1_2opts.eps");

    //
    hsim1p2to2p4EffTFTTBaselineTT20->Draw("e3");
    tp = new TPave(0.4, 0.71, 0.95, 0.78, 0, "NDC"); tp->SetFillColor(0);
    tp->Draw();
    tex1 = new TLatex(.41,.73,"p_{T}^{trig}> 20 GeV, 1.2<|#eta|<2.4 and #geq 2 CSCTF stubs");
    tex1->SetTextFont(42);
    tex1->SetTextSize(0.042);
    tex1->SetNDC();
    tex1->Draw();
    gPad->SetLogx(0);

    hsim1p2to2p4EffTFTTBaselineWME1TT20->Draw("same e3");
    //  hp2sim1p2to2p4EffTFTTBaselineTT20->Draw("same e3");
    //  hp2sim1p2to2p4EffTFTTBaselineWME1TT20->Draw("same e3");
    hp2sim1p2to2p4EffTFTTBaselineWME1dPhiTT20->Draw("same e3");

    TLegend* leg = new TLegend(0.4, 0.50, 0.95, 0.7);
    leg->SetFillColor(0); leg->SetBorderSize(0);
    leg->SetTextSize(0.04); leg->SetMargin(0.13);
    leg->AddEntry(hsim1p2to2p4EffTFTTBaselineTT20, "Phase-1 muon");
    leg->AddEntry(hsim1p2to2p4EffTFTTBaselineWME1TT20, "Phase-1 muon with YE-1 required");
    //  leg->AddEntry(hp2sim1p2to2p4EffTFTTBaselineTT20, "Phase-2 muon");
    //  leg->AddEntry(hp2sim1p2to2p4EffTFTTBaselineWME1TT20, "Phase-2 muon with YE-1 required");
    leg->AddEntry(hp2sim1p2to2p4EffTFTTBaselineWME1dPhiTT20, "Phase-2 muon with YE-1 angle cut (*)");
    leg->Draw();
  
    gPad->SaveAs("cms/emu/June1462X/hsim1p2to2p4EffTT20_baselineVME1_3opts.png");
    gPad->SaveAs("cms/emu/June1462X/hsim1p2to2p4EffTT20_baselineVME1_3opts.pdf");
    gPad->SaveAs("cms/emu/June1462X/hsim1p2to2p4EffTT20_baselineVME1_3opts.eps");
  }

  // NOW TT20 plots
  cTT20 = new TCanvas("cTT20","cTT20",700,450);
  cTT20->Clear();
  gPad->SetLogx(0);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);
  
  TProfile* h = hsim1p1to2p2EffTFTTBaselineTT20;
  h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
  h->GetXaxis()->SetTitle("Simulated muon p_{T}");
  h->GetYaxis()->SetTitle("Efficiency");
  h->SetTitleSize(0.05);
  h->SetTitleOffset(1.1, "X");
  h->SetTitleOffset(0.84, "Y");
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetTitleSize(0.055);
  h->GetYaxis()->SetTitleSize(0.06);
  
  //
  hsim1p1to2p2EffTFTTBaselineTT20->SetMarkerStyle(25);
  hsim0to1p1EffTFTTBaselineTT20->SetMarkerStyle(20);
  hsim1p1to2p2EffTFTTBaselineTT20->SetMarkerSize(1.2);
  hsim0to1p1EffTFTTBaselineTT20->SetMarkerSize(1.2);
  hsim1p1to2p2EffTFTTBaselineTT20->SetLineWidth(2);
  hsim0to1p1EffTFTTBaselineTT20->SetLineWidth(2);
  hsim1p1to2p2EffTFTTBaselineTT20->SetLineColor(1);
  hsim0to1p1EffTFTTBaselineTT20->SetLineColor(1);
  hsim1p1to2p2EffTFTTBaselineTT20->SetMarkerColor(1);
  hsim0to1p1EffTFTTBaselineTT20->SetMarkerColor(1);

  hsim1p1to2p2EffTFTTBaselineTT20->Draw("E1");
  gPad->SetLogx(0);
  hsim0to1p1EffTFTTBaselineTT20->Draw("same e1");

  hsim1p1to2p2EffTFTTBaselineTT20->SetMinimum(0);
  hsim1p1to2p2EffTFTTBaselineTT20->SetMaximum(1.05);

  hsim1p1to2p2EffTFBaselineTF20->SetMarkerStyle(25);
  hsim0to1p1EffTFBaselineTF20->SetMarkerStyle(20);
  hsim1p1to2p2EffTFBaselineTF20->SetMarkerSize(1.2);
  hsim0to1p1EffTFBaselineTF20->SetMarkerSize(1.2);
  hsim1p1to2p2EffTFBaselineTF20->SetLineWidth(2);
  hsim0to1p1EffTFBaselineTF20->SetLineWidth(2);
  hsim1p1to2p2EffTFBaselineTF20->SetLineColor(2);
  hsim0to1p1EffTFBaselineTF20->SetLineColor(2);
  hsim1p1to2p2EffTFBaselineTF20->SetMarkerColor(2);
  hsim0to1p1EffTFBaselineTF20->SetMarkerColor(2);

  hsim1p1to2p2EffTFBaselineTF20->Draw("same e1");
  hsim0to1p1EffTFBaselineTF20->Draw("same e1");

  TLegend* leg = new TLegend(0.5, 0.45, 0.95, 0.7);
  leg->SetFillColor(0); leg->SetBorderSize(0);
  leg->SetTextSize(0.045); leg->SetMargin(0.13);
  leg->AddEntry(hsim0to1p1EffTFBaselineTF20, "L1Muons (qual #geq 4) |#eta| < 1.1");
  leg->AddEntry(hsim0to1p1EffTFTTBaselineTT20, "L1TkMuons             |#eta| < 1.1");
  leg->AddEntry(hsim1p1to2p2EffTFBaselineTF20, "L1Muons (qual #geq 4) 1.1 < |#eta| < 2.2");
  leg->AddEntry(hsim1p1to2p2EffTFTTBaselineTT20, "L1TkMuons             1.1 < |#eta| < 2.2");
  leg->Draw();
  
  gPad->SaveAs("cms/emu/June1462X/hsimEff20_baselineL1MuVL1TkMu.png");
  gPad->SaveAs("cms/emu/June1462X/hsimEff20_baselineL1MuVL1TkMu.pdf");
  gPad->SaveAs("cms/emu/June1462X/hsimEff20_baselineL1MuVL1TkMu.eps");


  // NOW TT20 plots
  cTT20mixh = new TCanvas("cTT20mixh","cTT20mixh",700,450);
  cTT20mixh->Clear();
  gPad->SetLogx(0);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);
  
  TProfile* h = hsim1p1to2p4EffTFTTBaselineTT20;
  h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
  h->GetXaxis()->SetTitle("Simulated muon p_{T}");
  h->GetYaxis()->SetTitle("Efficiency");
  h->SetTitleSize(0.05);
  h->SetTitleOffset(1.1, "X");
  h->SetTitleOffset(0.84, "Y");
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetTitleSize(0.055);
  h->GetYaxis()->SetTitleSize(0.06);
  
  //
  hsim1p1to2p4EffTFTTBaselineTT20->SetMarkerStyle(25);
  hsim0to1p1EffTFTTBaselineTT20->SetMarkerStyle(20);
  hsim1p1to2p4EffTFTTBaselineTT20->SetMarkerSize(1.2);
  hsim0to1p1EffTFTTBaselineTT20->SetMarkerSize(1.2);
  hsim1p1to2p4EffTFTTBaselineTT20->SetLineWidth(2);
  hsim0to1p1EffTFTTBaselineTT20->SetLineWidth(2);
  hsim1p1to2p4EffTFTTBaselineTT20->SetLineColor(1);
  hsim0to1p1EffTFTTBaselineTT20->SetLineColor(1);
  hsim1p1to2p4EffTFTTBaselineTT20->SetMarkerColor(1);
  hsim0to1p1EffTFTTBaselineTT20->SetMarkerColor(1);

  hsim1p1to2p4EffTFTTBaselineTT20->Draw("E1");
  gPad->SetLogx(0);
  hsim0to1p1EffTFTTBaselineTT20->Draw("same e1");

  hsim1p1to2p4EffTFTTBaselineTT20->SetMinimum(0);
  hsim1p1to2p4EffTFTTBaselineTT20->SetMaximum(1.05);

  hsim1p1to2p1EffTFBaselineTF20->SetMarkerStyle(25);
  hsim0to1p1EffTFBaselineTF20->SetMarkerStyle(20);
  hsim1p1to2p1EffTFBaselineTF20->SetMarkerSize(1.2);
  hsim0to1p1EffTFBaselineTF20->SetMarkerSize(1.2);
  hsim1p1to2p1EffTFBaselineTF20->SetLineWidth(2);
  hsim0to1p1EffTFBaselineTF20->SetLineWidth(2);
  hsim1p1to2p1EffTFBaselineTF20->SetLineColor(2);
  hsim0to1p1EffTFBaselineTF20->SetLineColor(2);
  hsim1p1to2p1EffTFBaselineTF20->SetMarkerColor(2);
  hsim0to1p1EffTFBaselineTF20->SetMarkerColor(2);

  hsim1p1to2p1EffTFBaselineTF20->Draw("same e1");
  hsim0to1p1EffTFBaselineTF20->Draw("same e1");

  TLegend* leg = new TLegend(0.5, 0.45, 0.95, 0.7);
  leg->SetFillColor(0); leg->SetBorderSize(0);
  leg->SetTextSize(0.045); leg->SetMargin(0.13);
  leg->AddEntry(hsim0to1p1EffTFBaselineTF20, "L1Muons (qual #geq 4) |#eta| < 1.1");
  leg->AddEntry(hsim0to1p1EffTFTTBaselineTT20, "L1TkMuons             |#eta| < 1.1");
  leg->AddEntry(hsim1p1to2p1EffTFBaselineTF20, "L1Muons (qual #geq 4) 1.1 < |#eta| < 2.1");
  leg->AddEntry(hsim1p1to2p4EffTFTTBaselineTT20, "L1TkMuons             1.1 < |#eta| < 2.4");
  leg->Draw();
  
  gPad->SaveAs("cms/emu/June1462X/hsimEff20_baselineL1MuVL1TkMu_mixh.png");
  gPad->SaveAs("cms/emu/June1462X/hsimEff20_baselineL1MuVL1TkMu_mixh.pdf");
  gPad->SaveAs("cms/emu/June1462X/hsimEff20_baselineL1MuVL1TkMu_mixh.eps");


  // NOW TT20 plots
  cTT20mixh = new TCanvas("cTT20mixh","cTT20mixh",700,450);
  cTT20mixh->Clear();
  gPad->SetLogx(0);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);
  
  TProfile* h = hsim1p1to2p4EffTFTTBaselineTT20;
  h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
  h->GetXaxis()->SetTitle("Simulated muon p_{T}");
  h->GetYaxis()->SetTitle("Efficiency");
  h->SetTitleSize(0.05);
  h->SetTitleOffset(1.1, "X");
  h->SetTitleOffset(0.84, "Y");
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetTitleSize(0.055);
  h->GetYaxis()->SetTitleSize(0.06);
  
  //
  hsim1p1to2p4EffTFTTBaselineTT20->SetMarkerStyle(25);
  hsim0to1p1EffTFTTBaselineTT20->SetMarkerStyle(20);
  hsim1p1to2p4EffTFTTBaselineTT20->SetMarkerSize(1.2);
  hsim0to1p1EffTFTTBaselineTT20->SetMarkerSize(1.2);
  hsim1p1to2p4EffTFTTBaselineTT20->SetLineWidth(2);
  hsim0to1p1EffTFTTBaselineTT20->SetLineWidth(2);
  hsim1p1to2p4EffTFTTBaselineTT20->SetLineColor(1);
  hsim0to1p1EffTFTTBaselineTT20->SetLineColor(1);
  hsim1p1to2p4EffTFTTBaselineTT20->SetMarkerColor(1);
  hsim0to1p1EffTFTTBaselineTT20->SetMarkerColor(1);

  hsim1p1to2p4EffTFTTBaselineTT20->Draw("E1");
  gPad->SetLogx(0);
  hsim0to1p1EffTFTTBaselineTT20->Draw("same e1");

  hsim1p1to2p4EffTFTTBaselineTT20->SetMinimum(0);
  hsim1p1to2p4EffTFTTBaselineTT20->SetMaximum(1.05);

  hsim1p1to2p4EffTFBaselineTF20->SetMarkerStyle(25);
  hsim0to1p1EffTFBaselineTF20->SetMarkerStyle(20);
  hsim1p1to2p4EffTFBaselineTF20->SetMarkerSize(1.2);
  hsim0to1p1EffTFBaselineTF20->SetMarkerSize(1.2);
  hsim1p1to2p4EffTFBaselineTF20->SetLineWidth(2);
  hsim0to1p1EffTFBaselineTF20->SetLineWidth(2);
  hsim1p1to2p4EffTFBaselineTF20->SetLineColor(2);
  hsim0to1p1EffTFBaselineTF20->SetLineColor(2);
  hsim1p1to2p4EffTFBaselineTF20->SetMarkerColor(2);
  hsim0to1p1EffTFBaselineTF20->SetMarkerColor(2);

  hsim1p1to2p4EffTFBaselineTF20->Draw("same e1");
  hsim0to1p1EffTFBaselineTF20->Draw("same e1");

  TLegend* leg = new TLegend(0.5, 0.45, 0.95, 0.7);
  leg->SetFillColor(0); leg->SetBorderSize(0);
  leg->SetTextSize(0.045); leg->SetMargin(0.13);
  leg->AddEntry(hsim0to1p1EffTFBaselineTF20, "L1Muons (qual #geq 4) |#eta| < 1.1");
  leg->AddEntry(hsim0to1p1EffTFTTBaselineTT20, "L1TkMuons             |#eta| < 1.1");
  leg->AddEntry(hsim1p1to2p4EffTFBaselineTF20, "L1Muons (qual #geq 4) 1.1 < |#eta| < 2.4");
  leg->AddEntry(hsim1p1to2p4EffTFTTBaselineTT20, "L1TkMuons             1.1 < |#eta| < 2.4");
  leg->Draw();
  
  gPad->SaveAs("cms/emu/June1462X/hsimEff20_baselineL1MuVL1TkMu_mixh2.png");
  gPad->SaveAs("cms/emu/June1462X/hsimEff20_baselineL1MuVL1TkMu_mixh2.pdf");
  gPad->SaveAs("cms/emu/June1462X/hsimEff20_baselineL1MuVL1TkMu_mixh2.eps");

  // NOW TT20 plots
  c = new TCanvas("c","c",700,450);
  c->Clear();
  gPad->SetLogx(0);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);
  
  TProfile* h = hsim1p1to2p2EffTFTTBaseline;
  h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
  h->GetXaxis()->SetTitle("Simulated muon p_{T}");
  h->GetYaxis()->SetTitle("Efficiency");
  h->SetTitleSize(0.05);
  h->SetTitleOffset(1.1, "X");
  h->SetTitleOffset(0.84, "Y");
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetTitleSize(0.055);
  h->GetYaxis()->SetTitleSize(0.06);
  
  //
  hsim1p1to2p2EffTFTTBaseline->SetMarkerStyle(25);
  hsim0to1p1EffTFTTBaseline->SetMarkerStyle(20);
  hsim1p1to2p2EffTFTTBaseline->SetMarkerSize(1.2);
  hsim0to1p1EffTFTTBaseline->SetMarkerSize(1.2);
  hsim1p1to2p2EffTFTTBaseline->SetLineWidth(2);
  hsim0to1p1EffTFTTBaseline->SetLineWidth(2);
  hsim1p1to2p2EffTFTTBaseline->SetLineColor(1);
  hsim0to1p1EffTFTTBaseline->SetLineColor(1);
  hsim1p1to2p2EffTFTTBaseline->SetMarkerColor(1);
  hsim0to1p1EffTFTTBaseline->SetMarkerColor(1);

  hsim1p1to2p2EffTFTTBaseline->Draw("E1");
  gPad->SetLogx(0);
  hsim0to1p1EffTFTTBaseline->Draw("same e1");

  hsim1p1to2p2EffTFTTBaseline->SetMinimum(0);
  hsim1p1to2p2EffTFTTBaseline->SetMaximum(1.05);

  hsim1p1to2p2EffTFBaseline->SetMarkerStyle(25);
  hsim0to1p1EffTFBaseline->SetMarkerStyle(20);
  hsim1p1to2p2EffTFBaseline->SetMarkerSize(1.2);
  hsim0to1p1EffTFBaseline->SetMarkerSize(1.2);
  hsim1p1to2p2EffTFBaseline->SetLineWidth(2);
  hsim0to1p1EffTFBaseline->SetLineWidth(2);
  hsim1p1to2p2EffTFBaseline->SetLineColor(2);
  hsim0to1p1EffTFBaseline->SetLineColor(2);
  hsim1p1to2p2EffTFBaseline->SetMarkerColor(2);
  hsim0to1p1EffTFBaseline->SetMarkerColor(2);

  hsim1p1to2p2EffTFBaseline->Draw("same e1");
  hsim0to1p1EffTFBaseline->Draw("same e1");

  TLegend* leg = new TLegend(0.5, 0.45, 0.95, 0.7);
  leg->SetFillColor(0); leg->SetBorderSize(0);
  leg->SetTextSize(0.045); leg->SetMargin(0.13);
  leg->AddEntry(hsim0to1p1EffTFBaseline, "L1Muons (qual #geq 4) |#eta| < 1.1");
  leg->AddEntry(hsim0to1p1EffTFTTBaseline, "L1TkMuons             |#eta| < 1.1");
  leg->AddEntry(hsim1p1to2p2EffTFBaseline, "L1Muons (qual #geq 4) 1.1 < |#eta| < 2.2");
  leg->AddEntry(hsim1p1to2p2EffTFTTBaseline, "L1TkMuons             1.1 < |#eta| < 2.2");
  leg->Draw();
  
  gPad->SaveAs("cms/emu/June1462X/hsimEff_baselineL1MuVL1TkMu.png");
  gPad->SaveAs("cms/emu/June1462X/hsimEff_baselineL1MuVL1TkMu.pdf");
  gPad->SaveAs("cms/emu/June1462X/hsimEff_baselineL1MuVL1TkMu.eps");


  // NOW TT20 plots
  cTT20h2p4 = new TCanvas("cTT20h2p4","cTT20h2p4",700,450);
  cTT20h2p4->Clear();
  gPad->SetLogx(0);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);
  
  TProfile* h = hsim1p1to2p4EffTFTTBaselineTT20;
  h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
  h->GetXaxis()->SetTitle("Simulated muon p_{T}");
  h->GetYaxis()->SetTitle("Efficiency");
  h->SetTitleSize(0.05);
  h->SetTitleOffset(1.1, "X");
  h->SetTitleOffset(0.84, "Y");
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetTitleSize(0.055);
  h->GetYaxis()->SetTitleSize(0.06);
  
  //
  hsim1p1to2p4EffTFTTBaselineTT20->SetMarkerStyle(25);
  hsim0to1p1EffTFTTBaselineTT20->SetMarkerStyle(20);
  hsim1p1to2p4EffTFTTBaselineTT20->SetMarkerSize(1.2);
  hsim0to1p1EffTFTTBaselineTT20->SetMarkerSize(1.2);
  hsim1p1to2p4EffTFTTBaselineTT20->SetLineWidth(2);
  hsim0to1p1EffTFTTBaselineTT20->SetLineWidth(2);
  hsim1p1to2p4EffTFTTBaselineTT20->SetLineColor(1);
  hsim0to1p1EffTFTTBaselineTT20->SetLineColor(1);
  hsim1p1to2p4EffTFTTBaselineTT20->SetMarkerColor(1);
  hsim0to1p1EffTFTTBaselineTT20->SetMarkerColor(1);

  hsim1p1to2p4EffTFTTBaselineTT20->Draw("E1");
  gPad->SetLogx(0);
  hsim0to1p1EffTFTTBaselineTT20->Draw("same e1");

  hsim1p1to2p4EffTFTTBaselineTT20->SetMinimum(0);
  hsim1p1to2p4EffTFTTBaselineTT20->SetMaximum(1.05);

  hsim1p1to2p4EffTFBaselineTF20->SetMarkerStyle(25);
  hsim0to1p1EffTFBaselineTF20->SetMarkerStyle(20);
  hsim1p1to2p4EffTFBaselineTF20->SetMarkerSize(1.2);
  hsim0to1p1EffTFBaselineTF20->SetMarkerSize(1.2);
  hsim1p1to2p4EffTFBaselineTF20->SetLineWidth(2);
  hsim0to1p1EffTFBaselineTF20->SetLineWidth(2);
  hsim1p1to2p4EffTFBaselineTF20->SetLineColor(2);
  hsim0to1p1EffTFBaselineTF20->SetLineColor(2);
  hsim1p1to2p4EffTFBaselineTF20->SetMarkerColor(2);
  hsim0to1p1EffTFBaselineTF20->SetMarkerColor(2);

  //  hsim1p1to2p4EffTFBaselineTF20->Draw("same e1");
  //  hsim0to1p1EffTFBaselineTF20->Draw("same e1");

  TLegend* leg = new TLegend(0.5, 0.45, 0.95, 0.7);
  leg->SetFillColor(0); leg->SetBorderSize(0);
  leg->SetTextSize(0.045); leg->SetMargin(0.13);
  //  leg->AddEntry(hsim0to1p1EffTFBaselineTF20, "L1Muons (qual #geq 4) |#eta| < 1.1");
  leg->AddEntry(hsim0to1p1EffTFTTBaselineTT20, "L1TkMuons             |#eta| < 1.1");
  //  leg->AddEntry(hsim1p1to2p4EffTFBaselineTF20, "L1Muons (qual #geq 4) 1.1 < |#eta| < 2.4");
  leg->AddEntry(hsim1p1to2p4EffTFTTBaselineTT20, "L1TkMuons             1.1 < |#eta| < 2.4");
  leg->Draw();
  
  gPad->SaveAs("cms/emu/June1462X/hsimEff20_baselineL1TkMu.png");
  gPad->SaveAs("cms/emu/June1462X/hsimEff20_baselineL1TkMu.pdf");
  gPad->SaveAs("cms/emu/June1462X/hsimEff20_baselineL1TkMu.eps");


  // NOW muCh2 plots
  cmuChOpt2more = new TCanvas("cmuChOpt2more","cmuChOpt2more",700,450);
  cmuChOpt2more->Clear();
  gPad->SetLogx(0);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);
  
  TProfile* h = hsim1p6to2p1EffTFTTBaselineMuCh;
  h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
  h->GetXaxis()->SetTitle("Simulated muon p_{T}");
  h->GetYaxis()->SetTitle("Efficiency");
  h->SetTitleSize(0.05);
  h->SetTitleOffset(1.1, "X");
  h->SetTitleOffset(0.84, "Y");
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetTitleSize(0.055);
  h->GetYaxis()->SetTitleSize(0.06);
 
  
  hsim1p6to2p1EffTFTTBaselineMuCh->Draw("E3");
  hp2sim1p6to2p1EffTFTTBaselineMuCh->Draw("E3 same");
  gPad->SetLogx(0);
  hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDeco->Draw("same e3");

  tp = new TPave(0.45, 0.43, 0.95, 0.5, 0, "NDC"); tp->SetFillColor(0);
  tp->Draw();
  tex1 = new TLatex(.41,.45,"L1TkMu: 1.6<|#eta|<2.1");
  tex1->SetTextFont(42);
  tex1->SetTextSize(0.048);
  tex1->SetNDC();
  tex1->Draw();

  hsim1p6to2p1EffTFTTBaselineMuCh->SetMinimum(0.5);
  hsim1p6to2p1EffTFTTBaselineMuCh->SetMaximum(1.05);

  hsim1p6to2p1EffTFTTBaselineMuCh->SetLineColor(kBlue+1);
  hsim1p6to2p1EffTFTTBaselineMuCh->SetFillColor(kBlue+1);
  hsim1p6to2p1EffTFTTBaselineMuCh->SetLineWidth(1);

  hp2sim1p6to2p1EffTFTTBaselineMuCh->SetLineColor(kCyan);
  hp2sim1p6to2p1EffTFTTBaselineMuCh->SetFillColor(kCyan);
  hp2sim1p6to2p1EffTFTTBaselineMuCh->SetLineWidth(1);

  hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDeco->SetLineColor(kRed+2); 
  hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDeco->SetFillColor(kRed+2); 
  hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDeco->SetLineWidth(1); 

  TLegend* leg = new TLegend(0.45, 0.15, 0.95, 0.4);
  leg->SetFillColor(0); leg->SetBorderSize(0);
  leg->SetTextSize(0.04); leg->SetMargin(0.13);
  leg->AddEntry(hsim1p6to2p1EffTFTTBaselineMuCh, "Phase-1 muon");
  leg->AddEntry(hp2sim1p6to2p1EffTFTTBaselineMuCh, "Phase-2 muon");
  leg->AddEntry(hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDeco, "Phase-2 muon with MS1 and bending cut");
  leg->Draw();
  
  gPad->SaveAs("cms/emu/June1462X/hsimEff_baselineL1TkMu_muChOpt2more.png");
  gPad->SaveAs("cms/emu/June1462X/hsimEff_baselineL1TkMu_muChOpt2more.pdf");
  gPad->SaveAs("cms/emu/June1462X/hsimEff_baselineL1TkMu_muChOpt2more.eps");

  // NOW muCh2 plots
  cmuChOpt2tfpt = new TCanvas("cmuChOpt2tfpt","cmuChOpt2tfpt",700,450);
  cmuChOpt2tfpt->Clear();
  gPad->SetLogx(0);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);
  
  TProfile* h = hsim1p6to2p1EffTFTTBaselineMuCh;
  h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
  h->GetXaxis()->SetTitle("Simulated muon p_{T}");
  h->GetYaxis()->SetTitle("Efficiency");
  h->SetTitleSize(0.05);
  h->SetTitleOffset(1.1, "X");
  h->SetTitleOffset(0.84, "Y");
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetTitleSize(0.055);
  h->GetYaxis()->SetTitleSize(0.06);
 
  
  hsim1p6to2p1EffTFTTBaselineMuCh->Draw("E3");
  gPad->SetLogx(0);
  hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDeco->Draw("same e3");
  hp2sim1p6to2p1EffTFTTBaselineTF0p5PTor20WS1dPhiMuChDeco->Draw("same e3");

  tp = new TPave(0.45, 0.43, 0.95, 0.5, 0, "NDC"); tp->SetFillColor(0);
  tp->Draw();
  tex1 = new TLatex(.41,.45,"L1TkMu: 1.6<|#eta|<2.1");
  tex1->SetTextFont(42);
  tex1->SetTextSize(0.048);
  tex1->SetNDC();
  tex1->Draw();

  hsim1p6to2p1EffTFTTBaselineMuCh->SetMinimum(0.5);
  hsim1p6to2p1EffTFTTBaselineMuCh->SetMaximum(1.05);

  hsim1p6to2p1EffTFTTBaselineMuCh->SetLineColor(kBlue+1);
  hsim1p6to2p1EffTFTTBaselineMuCh->SetFillColor(kBlue+1);
  hsim1p6to2p1EffTFTTBaselineMuCh->SetLineWidth(1);

  hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDeco->SetLineColor(kRed+2); 
  hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDeco->SetFillColor(kRed+2); 
  hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDeco->SetLineWidth(1); 

  hp2sim1p6to2p1EffTFTTBaselineTF0p5PTor20WS1dPhiMuChDeco->SetLineColor(kMagenta); 
  hp2sim1p6to2p1EffTFTTBaselineTF0p5PTor20WS1dPhiMuChDeco->SetFillColor(kMagenta); 
  hp2sim1p6to2p1EffTFTTBaselineTF0p5PTor20WS1dPhiMuChDeco->SetLineWidth(1); 



  TLegend* leg = new TLegend(0.45, 0.15, 0.95, 0.4);
  leg->SetFillColor(0); leg->SetBorderSize(0);
  leg->SetTextSize(0.04); leg->SetMargin(0.13);
  leg->AddEntry(hsim1p6to2p1EffTFTTBaselineMuCh, "Phase-1 muon");
  leg->AddEntry(hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDeco, "Phase-2 muon with MS1 and bending cut");
  leg->AddEntry(hp2sim1p6to2p1EffTFTTBaselineTF0p5PTor20WS1dPhiMuChDeco, "Phase-2 muon with MS1 #Delta#phi and TF p_{T}");
  leg->Draw();
  
  gPad->SaveAs("cms/emu/June1462X/hsimEff_baselineL1TkMu_muChOpt2tfpt.png");
  gPad->SaveAs("cms/emu/June1462X/hsimEff_baselineL1TkMu_muChOpt2tfpt.pdf");
  gPad->SaveAs("cms/emu/June1462X/hsimEff_baselineL1TkMu_muChOpt2tfpt.eps");

  cmuChOpt2 = new TCanvas("cmuChOpt2","cmuChOpt2",700,450);
  cmuChOpt2->Clear();
  gPad->SetLogx(0);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);

  hsim1p6to2p1EffTFTTBaselineMuCh->Draw("E3");
  gPad->SetLogx(0);
  hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDeco->Draw("same e3");

  tp = new TPave(0.45, 0.43, 0.95, 0.5, 0, "NDC"); tp->SetFillColor(0);
  tp->Draw();
  tex1 = new TLatex(.41,.45,"L1TkMu: 1.6<|#eta|<2.1");
  tex1->SetTextFont(42);
  tex1->SetTextSize(0.048);
  tex1->SetNDC();
  tex1->Draw();


  TLegend* leg = new TLegend(0.45, 0.15, 0.95, 0.4);
  leg->SetFillColor(0); leg->SetBorderSize(0);
  leg->SetTextSize(0.04); leg->SetMargin(0.13);
  leg->AddEntry(hsim1p6to2p1EffTFTTBaselineMuCh, "Phase-1 muon");
  leg->AddEntry(hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDeco, "Phase-2 muon with MS1 and bending cut");
  leg->Draw();
  
  gPad->SaveAs("cms/emu/June1462X/hsimEff_baselineL1TkMu_muChOpt2.png");
  gPad->SaveAs("cms/emu/June1462X/hsimEff_baselineL1TkMu_muChOpt2.pdf");
  gPad->SaveAs("cms/emu/June1462X/hsimEff_baselineL1TkMu_muChOpt2.eps");

  cTT20muChOpt2 = new TCanvas("cTT20muChOpt2","cTT20muChOpt2",700,450);
  cTT20muChOpt2->Clear();
  gPad->SetLogx(0);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx(1);
  gPad->SetTicky(1);
  
  TProfile* h = hsim1p6to2p1EffTFTTBaselineMuChTT20;
  h->SetTitle("CMS Phase 2 Simulation Preliminary,  PU = 140 @ 25 nsec");
  h->GetXaxis()->SetTitle("Simulated muon p_{T}");
  h->GetYaxis()->SetTitle("Efficiency");
  h->SetTitleSize(0.05);
  h->SetTitleOffset(1.1, "X");
  h->SetTitleOffset(0.84, "Y");
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetTitleSize(0.055);
  h->GetYaxis()->SetTitleSize(0.06);

  
  
  hsim1p6to2p1EffTFTTBaselineMuChTT20->Draw("E3");
  gPad->SetLogx(0);
  hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDecoTT20->Draw("same e3");

  tp = new TPave(0.45, 0.43, 0.95, 0.5, 0, "NDC"); tp->SetFillColor(0);
  tp->Draw();
  tex1 = new TLatex(.41,.45,"L1TkMu: 1.6<|#eta|<2.1");
  tex1->SetTextFont(42);
  tex1->SetTextSize(0.048);
  tex1->SetNDC();
  tex1->Draw();

  hsim1p6to2p1EffTFTTBaselineMuChTT20->SetMinimum(0.0);
  hsim1p6to2p1EffTFTTBaselineMuChTT20->SetMaximum(1.05);
  

  hsim1p6to2p1EffTFTTBaselineMuChTT20->SetLineColor(kBlue+1);
  hsim1p6to2p1EffTFTTBaselineMuChTT20->SetFillColor(kBlue+1);
  hsim1p6to2p1EffTFTTBaselineMuChTT20->SetLineWidth(1);

  hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDecoTT20->SetLineColor(kRed+2); 
  hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDecoTT20->SetFillColor(kRed+2); 
  hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDecoTT20->SetLineWidth(1); 

  TLegend* leg = new TLegend(0.45, 0.15, 0.95, 0.4);
  leg->SetFillColor(0); leg->SetBorderSize(0);
  leg->SetTextSize(0.04); leg->SetMargin(0.13);
  leg->AddEntry(hsim1p6to2p1EffTFTTBaselineMuChTT20, "Phase-1 muon");
  leg->AddEntry(hp2sim1p6to2p1EffTFTTBaselineWS1dPhiMuChDecoTT20, "Phase-2 muon with MS1 and bending cut");
  leg->Draw();
  
  gPad->SaveAs("cms/emu/June1462X/hsimEff20_baselineL1TkMu_muChOpt2.png");
  gPad->SaveAs("cms/emu/June1462X/hsimEff20_baselineL1TkMu_muChOpt2.pdf");
  gPad->SaveAs("cms/emu/June1462X/hsimEff20_baselineL1TkMu_muChOpt2.eps");
  
}
