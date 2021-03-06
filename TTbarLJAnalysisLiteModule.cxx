#include <iostream>
#include <memory>
#include <UHH2/core/include/AnalysisModule.h>
#include <UHH2/core/include/Event.h>
#include <UHH2/core/include/Selection.h>
#include <UHH2/common/include/MCWeight.h>
#include <UHH2/common/include/CleaningModules.h>
#include <UHH2/common/include/NSelections.h>
#include <UHH2/common/include/LumiSelection.h>
#include <UHH2/common/include/TriggerSelection.h>
#include <UHH2/common/include/JetCorrections.h>
#include <UHH2/common/include/ObjectIdUtils.h>
#include <UHH2/common/include/MuonIds.h>
#include <UHH2/common/include/ElectronIds.h>
#include <UHH2/common/include/JetIds.h>
#include <UHH2/common/include/TopJetIds.h>
#include <UHH2/common/include/TTbarGen.h>
#include <UHH2/common/include/TTbarReconstruction.h>
#include <UHH2/common/include/ReconstructionHypothesisDiscriminators.h>
#include <UHH2/common/include/Utils.h>
#include <UHH2/common/include/JetHists.h>
#include <UHH2/common/include/TopPtReweight.h>
#include <UHH2/ZprimeSemiLeptonic/include/ModuleBASE.h>
#include <UHH2/ZprimeSemiLeptonic/include/ZprimeSemiLeptonicSelections.h>
#include <UHH2/ZprimeSemiLeptonic/include/ZprimeSemiLeptonicUtils.h>
#include <UHH2/ZprimeSemiLeptonic/include/TTbarLJHists.h>
#include <UHH2/ZprimeSemiLeptonic/include/DileptonHists.h>
#include <UHH2/ZprimeSemiLeptonic/include/EffyTTbarRECOHists.h>
#include <UHH2/ZprimeSemiLeptonic/include/SF_elec.h>
#include <UHH2/ZprimeSemiLeptonic/include/SF_ttagging.h>
#include <UHH2/ZprimeSemiLeptonic/include/SF_WjetsREWGT.h>
#include <UHH2/ZprimeSemiLeptonic/include/jacobi_eigenvalue.h>
#include <TMVA/Tools.h>
#include <TMVA/Reader.h>
#include <TMVA/MethodCuts.h>



class TTbarLJAnalysisLiteModule : public ModuleBASE {
	public:
  		explicit TTbarLJAnalysisLiteModule(uhh2::Context&);
  		virtual bool process(uhh2::Event&) override;
  	protected:
  		enum lepton { muon, elec };
  		lepton channel_;
  		//SELECTIONS
  		std::unique_ptr<uhh2::Selection> lumi_sel;
		std::unique_ptr<uhh2::Selection> jet2_sel;
  		std::unique_ptr<uhh2::Selection> jet1_sel;
  		std::unique_ptr<uhh2::Selection> trigger_sel;
  		std::unique_ptr<uhh2::Selection> trigger2_sel;
  		std::unique_ptr<uhh2::Selection> met_sel;
  		std::unique_ptr<uhh2::Selection> htlep_sel;
  		std::unique_ptr<uhh2::Selection> triangc_sel;
  		std::unique_ptr<uhh2::Selection> ttagevt_sel;
  		std::unique_ptr<uhh2::Selection> topleppt_sel;
  		std::unique_ptr<uhh2::Selection> chi2_sel;
  		//TTBAR RECONSTRUCTION
  		std::unique_ptr<uhh2::AnalysisModule> ttgenprod;
  		std::unique_ptr<uhh2::AnalysisModule> reco_primlep;
		std::unique_ptr<uhh2::AnalysisModule>  ttbar_reco__ttag0;
  		std::unique_ptr<uhh2::AnalysisModule>  ttbar_reco__ttag1;
  		std::unique_ptr<Chi2Discriminator>     ttbar_chi2__ttag0;
  		std::unique_ptr<Chi2DiscriminatorTTAG> ttbar_chi2__ttag1;
  		uhh2::Event::Handle<TTbarGen> h_ttbar_gen;
  		uhh2::Event::Handle<std::vector<ReconstructionHypothesis> > h_ttbar_hyps;
  		//MISCELANEOUS
  		float lep1_pt_;
  		float muo1_pt_max_, muo1_eta_max_;
		JetId btag_ID_;
  		CSVBTag::wp b_working_point;
		bool use_ttagging_;
  		TopJetId ttag_ID_;
  		float    ttag_minDR_jet_;
		bool blind_DATA_;
		bool store_PDF_weights_;
		//// Data/MC scale factors
  		std::unique_ptr<uhh2::AnalysisModule> elecID_SF;
  		std::unique_ptr<uhh2::AnalysisModule> elecGsf_SF;
		std::unique_ptr<uhh2::AnalysisModule> pileupSF;
  		std::unique_ptr<uhh2::AnalysisModule> muonID_SF;
  		std::unique_ptr<uhh2::AnalysisModule> muonHLT_SF;
 		std::unique_ptr<uhh2::AnalysisModule> btagSF;
  		std::unique_ptr<Hists> h_btagMCeffi;
		std::unique_ptr<weightcalc_ttagging> ttagSF_ct;
  		std::unique_ptr<weightcalc_ttagging> ttagSF_upL;
  		std::unique_ptr<weightcalc_ttagging> ttagSF_dnL;
  		std::unique_ptr<weightcalc_ttagging> ttagSF_upT;
  		std::unique_ptr<weightcalc_ttagging> ttagSF_dnT;
		std::unique_ptr<TopPtReweight> topptREWGT;
		//// VARS
		// event
  		Event::Handle<int>   h_run;
  		Event::Handle<int>   h_lumi_block;
  		Event::Handle<int>   h_event;
  		Event::Handle<int>   h_pvN;
  		Event::Handle<int>   h_muoN;
  		Event::Handle<int>   h_eleN;
  		Event::Handle<int>   h_btagN;
  		Event::Handle<int>   h_ttagN;
  		Event::Handle<bool>  h_ttagevt;
  		Event::Handle<int>   h_ttgen;
		// lepton
  		Event::Handle<TLorentzVector> h_lep1;
  		Event::Handle<int>            h_lep1__pdgID;
  		Event::Handle<int>            h_lep1__charge;
  		Event::Handle<float>          h_lep1__minDR_jet;
  		Event::Handle<float>          h_lep1__pTrel_jet;
		// jet
  		Event::Handle<TLorentzVector> h_jet1;
  		Event::Handle<TLorentzVector> h_jet2;
		// MET
  		Event::Handle<TLorentzVector> h_MET;
  		Event::Handle<float>          h_MET__pz;
		// ttbar reco hyp
  		Event::Handle<float>          h_rec_chi2;
  		Event::Handle<TLorentzVector> h_rec_tlep;
  		Event::Handle<TLorentzVector> h_rec_thad;
  		// weight
  		Event::Handle<float> h_wgtMC__GEN;
  		// elec_weights
  		Event::Handle<float> h_wgtMC__elecIDSF_ct;
		Event::Handle<float> h_wgtMC__elecIDSF_up;
		Event::Handle<float> h_wgtMC__elecIDSF_dn;
		Event::Handle<float> h_wgtMC__elecHLTSF_ct;
  		Event::Handle<float> h_wgtMC__elecHLTSF_up;
  		Event::Handle<float> h_wgtMC__elecHLTSF_dn;
		// ttag_weights
		Event::Handle<float> h_wgtMC__ttagSF_ct;
  		Event::Handle<float> h_wgtMC__ttagSF_upL;
  		Event::Handle<float> h_wgtMC__ttagSF_dnL;
  		Event::Handle<float> h_wgtMC__ttagSF_upT;
  		Event::Handle<float> h_wgtMC__ttagSF_dnT;
  		//q2_weights
  		Event::Handle<float> h_wgtMC__muR_ct__muF_up;
  		Event::Handle<float> h_wgtMC__muR_ct__muF_dn;
  		Event::Handle<float> h_wgtMC__muR_up__muF_ct;
  		Event::Handle<float> h_wgtMC__muR_up__muF_up;
  		Event::Handle<float> h_wgtMC__muR_dn__muF_ct;
  		Event::Handle<float> h_wgtMC__muR_dn__muF_dn;
  		//toppt_rewgt
    	Event::Handle<float> h_wgtMC__topptREWGT_ct;
  		Event::Handle<float> h_wgtMC__topptREWGT_up;
  		Event::Handle<float> h_wgtMC__topptREWGT_dn;
  		//PDF wgt
  		Event::Handle<std::vector<float> > h_wgtMC__PDF;
  		Event::Handle<float> h_jet1pt; 
  		Event::Handle<float> h_mttbar;
  		Event::Handle<float> h_jet1pt_chi2;
  		Event::Handle<float> h_mttbar_chi2;
  		//MVA Related 
  		TMVA::Reader *reader_wjets;
  		float WJets_TMVA_response;
  		Event::Handle<float> H_Rec_chi2;
  		Event::Handle<float> wjets_TMVA_response;
    	float mva_DRpt_norm;                                //1
    	float mva_ht_met_lep_norm;                          //2
    	float mva_lep1__minDR_jet;                          //3
    	float mva_ptrel_norm;                               //4
    	float mva_j1CSV;                                    //5
    	float mva_jet1pt_norm;                              //6
    	float mva_jet1m_norm;                               //7
    	float mva_j2CSV;                                    //8
    	float mva_jet2pt_norm;                              //9
    	float mva_jet2m_norm;                               //10
    	float mva_njets;                                    //11
    	float mva_s11;                                      //12
    	float mva_s12;                                      //13
    	float mva_s13;                                      //14
    	float mva_s22;                                      //15
    	float mva_s23;                                      //16
    	float mva_s33;                                      //17
    	float mva_sphericity;                               //18
    	float mva_aplanarity;                               //19
    	int it_num;
    	int rot_num;
    	//Event::Handle<float>    h_jet1_pt;                 //9
    	//Event::Handle<float>    h_jet2_pt;                 //10
  		Event::Handle<float>    h_lep1__pTrel_jet_norm;     //11
  		Event::Handle<float>    h_ht_met_lep_norm;          //12
  		Event::Handle<float>    h_jet1_csv;                //13
  		Event::Handle<float>    h_jet2_csv;                //14
  		Event::Handle<float>    h_DRpt;                     //15
  		Event::Handle<float>    h_njets;                    //16
 		Event::Handle<float>    h_jet1_m;                  //17
  		Event::Handle<float>    h_jet2_m;                  //18
  		Event::Handle<float>    h_lep1__minDR_norm;   //19
  		Event::Handle<float>    h_s33;  
};

TTbarLJAnalysisLiteModule::TTbarLJAnalysisLiteModule(uhh2::Context& ctx){
	//// CONFIGURATION
	const bool isMC = (ctx.get("dataset_type") == "MC");

  	const std::string& channel = ctx.get("channel", "");
  	if     (channel == "muon") channel_ = muon;
  	else if(channel == "elec") channel_ = elec;
  	else {
    	std::string log("TTbarLJAnalysisLiteModule::TTbarLJAnalysisLiteModule -- ");
    	log += "TTbarLJAnalysisLiteModule::TTbarLJAnalysisLiteModule -- invalid argument for 'channel' key in xml file (must be 'muon' or 'elec'): \""+channel+"\"";
   		throw std::runtime_error(log);
  	}
  	use_ttagging_ = true;
	blind_DATA_ = ((ctx.get("dataset_version").find("BLINDED") != std::string::npos) && (ctx.get("dataset_type") == "DATA") && !isMC);
	const std::string& store_PDF_weights = ctx.get("store_PDF_weights", "");
  	if     (store_PDF_weights == "true")  store_PDF_weights_ = true;
  	else if(store_PDF_weights == "false") store_PDF_weights_ = false;
  	else {
    	std::string log("TTbarLJAnalysisLiteModule::TTbarLJAnalysisLiteModule -- ");
    	log += "invalid argument for 'store_PDF_weights' key in xml file (must be 'true' or 'false'): \""+store_PDF_weights+"\"";
    	throw std::runtime_error(log);
  	}
  	const std::string& keyword = ctx.get("keyword");
	float jet1_pt(-1.), jet2_pt(-1.), MET(-1.), HT_lep(-1.);
  	bool triangul_cut(false);
  	bool topleppt_cut(false);
  	if(keyword == "T0_v01" || keyword == "T1_v01"){
		if     (keyword == "T0_v01") use_ttagging_ = false;
    	else if(keyword == "T1_v01") use_ttagging_ = true;
		if(channel_ == muon){
			lep1_pt_ =   55.;
      		jet1_pt  = 150.;
      		//jet1_pt  = 190;
      		jet2_pt  = 50.;
      		MET      =  50.;
      		HT_lep   = 150.;
			triangul_cut = false;
      		topleppt_cut = false;
			muo1_pt_max_ = 500.;
      		muo1_eta_max_ = 1.2;
    	}
    	else if(channel_ == elec){
			lep1_pt_ =   55.;
      		jet1_pt  = 175.;
      		jet2_pt  =  50.;
			MET      = 120.;
      		HT_lep   =   0.;
			triangul_cut = false;
      		topleppt_cut = false;
			muo1_pt_max_ = 0;
     		muo1_eta_max_ = 0;
		}
  	}
  	else throw std::runtime_error("TTbarLJAnalysisLiteModule::TTbarLJAnalysisLiteModule -- undefined \"keyword\" argument in .xml configuration file: "+keyword);
  	//// COMMON MODULES
	/* CMS-certified luminosity sections */
  	if(!isMC) lumi_sel.reset(new LumiSelection(ctx));

  	/**********************************/

  	//// OBJ CLEANING
  	////
	//// EVENT SELECTION
  	jet2_sel.reset(new NJetSelection(2, -1, JetId(PtEtaCut(jet2_pt, 2.4))));
  	jet1_sel.reset(new NJetSelection(1, -1, JetId(PtEtaCut(jet1_pt, 2.4))));
	const std::string& trigger = ctx.get("trigger", "NULL");
  	if(trigger != "NULL" && !isMC) trigger_sel.reset(new TriggerSelection(trigger));
  	else                  trigger_sel.reset(new uhh2::AndSelection(ctx));
  	const std::string& trigger2 = ctx.get("trigger2", "NULL");
  	if(trigger2 != "NULL" && !isMC) trigger2_sel.reset(new TriggerSelection(trigger2));
  	else                  trigger2_sel.reset(new uhh2::AndSelection(ctx));
	met_sel  .reset(new METCut  (MET   , uhh2::infinity));
  	htlep_sel.reset(new HTlepCut(HT_lep, uhh2::infinity));
  	if(triangul_cut){
		if     (channel_ == elec) triangc_sel.reset(new TriangularCuts(M_PI/2., (M_PI/2.)/75.));
    	else if(channel_ == muon) triangc_sel.reset(new uhh2::AndSelection(ctx));
  	}
  	else triangc_sel.reset(new uhh2::AndSelection(ctx));
  ////
	/* b-tagging */
  	const std::string& btag_wp = ctx.get("btag_wp");
	if     (btag_wp == "CSVL") btag_ID_ = CSVBTag(CSVBTag::WP_LOOSE);
  	else if(btag_wp == "CSVM") btag_ID_ = CSVBTag(CSVBTag::WP_MEDIUM);
  	else if(btag_wp == "CSVT") btag_ID_ = CSVBTag(CSVBTag::WP_TIGHT);
	if     (btag_wp == "CSVL") b_working_point = CSVBTag::WP_LOOSE;
  	else if(btag_wp == "CSVM") b_working_point = CSVBTag::WP_MEDIUM;
  	else if(btag_wp == "CSVT") b_working_point = CSVBTag::WP_TIGHT;
  	h_btagMCeffi.reset(new BTagMCEfficiencyHists(ctx,"BTAG",b_working_point));


  	/* t-tagging */
  	const std::string& ttag_wp = ctx.get("ttag_wp");
  	ttag_ID_ = TopTagID_SoftDrop(ttag_wp);
  	ttag_minDR_jet_ = 1.2;
	ttagevt_sel.reset(new TopTagEventSelection(ttag_ID_, ttag_minDR_jet_));
  	/*************/
	//// TTBAR KINEMATICAL RECO
	const std::string ttbar_gen_label ("ttbargen");
  	const std::string ttbar_hyps_label("TTbarReconstruction");
  	const std::string ttbar_chi2_label("Chi2");
	ttgenprod.reset(new TTbarGenProducer(ctx, ttbar_gen_label, false));
  	h_ttbar_gen = ctx.get_handle<TTbarGen>   (ttbar_gen_label);
	reco_primlep.reset(new PrimaryLepton(ctx));
	ttbar_reco__ttag0.reset(new HighMassTTbarReconstruction(ctx, NeutrinoReconstruction, ttbar_hyps_label));
  	ttbar_reco__ttag1.reset(new        TopTagReconstruction(ctx, NeutrinoReconstruction, ttbar_hyps_label, ttag_ID_, ttag_minDR_jet_));
	ttbar_chi2__ttag0.reset(new Chi2Discriminator(ctx, ttbar_hyps_label));
 	ttbar_chi2__ttag0->set_Mtlep_mean (175.);
  	ttbar_chi2__ttag0->set_Mtlep_sigma( 19.);
  	ttbar_chi2__ttag0->set_Mthad_mean (177.);
  	ttbar_chi2__ttag0->set_Mthad_sigma( 16.);
	ttbar_chi2__ttag1.reset(new Chi2DiscriminatorTTAG(ctx, ttbar_hyps_label));
  	ttbar_chi2__ttag1->set_Mtlep_mean (175.);
  	ttbar_chi2__ttag1->set_Mtlep_sigma( 19.);
  	ttbar_chi2__ttag1->set_Mthad_mean (173.);
  	ttbar_chi2__ttag1->set_Mthad_sigma( 15.);
  	ttbar_chi2__ttag1->use_subjet_mass(false);
	h_ttbar_hyps = ctx.get_handle<std::vector<ReconstructionHypothesis> >(ttbar_hyps_label);
  ////
	if(topleppt_cut){
		if     (channel_ == elec) topleppt_sel.reset(new LeptonicTopPtCut(ctx, 140., uhh2::infinity, ttbar_hyps_label, ttbar_chi2_label));
    	else if(channel_ == muon) topleppt_sel.reset(new uhh2::AndSelection(ctx));
  	}
  	else topleppt_sel.reset(new uhh2::AndSelection(ctx));
	chi2_sel.reset(new HypothesisDiscriminatorCut(ctx,  0., 30., ttbar_hyps_label, ttbar_chi2_label));

	std::vector<std::string> htags_2({
		"kine",
   		"kine__t0",
   		"kine__t1",

    	"antichi2",
    	"antichi2__t0",
    	"antichi2__t1",

	    "chi2_antibdt",
    	"chi2_antibdt__t0",
    	"chi2_antibdt__t1",

    	"chi2",
    	"chi2__t0",
    	"chi2__t1",
 	
    	"chi2_bdt",
    	"chi2_bdt__t0",
    	"chi2_bdt__t1",
      
    	"antichi2_antibdt",
    	"antichi2_antibdt__t0",
    	"antichi2_antibdt__t1",
   
    	"antichi2_bdt",
    	"antichi2_bdt__t0",
    	"antichi2_bdt__t1",
  	});

  	for(const auto& tag : htags_2){
		book_HFolder(tag          , new TTbarLJHists      (ctx, tag          , ttag_ID_, ttag_minDR_jet_));
    	book_HFolder(tag+"__ttbar", new EffyTTbarRECOHists(ctx, tag+"__ttbar", ttbar_gen_label, ttbar_hyps_label, ttbar_chi2_label));
  	}

	std::vector<std::string> htags_3({
		"chi2_L2",
    	"chi2_L2__t0",
    	"chi2_L2__t1",
  	});

	for(const auto& tag : htags_3){
		book_HFolder(tag          , new TTbarLJHists      (ctx, tag          , ttag_ID_, ttag_minDR_jet_));
    	book_HFolder(tag+"__dilep", new DileptonHists     (ctx, tag+"__dilep"));
    	book_HFolder(tag+"__ttbar", new EffyTTbarRECOHists(ctx, tag+"__ttbar", ttbar_gen_label, ttbar_hyps_label, ttbar_chi2_label));
  	}
  //
  	std::vector<std::string> htags_4({
    	"Final",
  	});

  	for(const auto& tag : htags_4){
		book_HFolder(tag          , new TTbarLJHists      (ctx, tag          , ttag_ID_, ttag_minDR_jet_));
    	book_HFolder(tag+"__ttbar", new EffyTTbarRECOHists(ctx, tag+"__ttbar", ttbar_gen_label, ttbar_hyps_label, ttbar_chi2_label));
  	}
  	//// Data/MC scale factors

	// muon-ID
  	const std::string& muonID_SFac    = ctx.get("muonID_SF_file");
  	const std::string& muonID_directory    = ctx.get("muonID_SF_directory");

  	// muon-HLT
  	const std::string& muonHLT_SFac   = ctx.get("muonHLT_SF_file");
  	const std::string& muonHLT_directory   = ctx.get("muonHLT_SF_directory");

  	//muon-Trk
  	//const std::string& muonTRK_SFac   = ctx.get("muonTRK_SF_file");

  	// elec-ID
    const std::string& elecID_SFac    = ctx.get("elecID_SF_file");
    //    const std::string& elecID_SFac_Data_part1    = ctx.get("elecID_SF_file_Data_part1");
    //    elecID_SF_Data_part1.reset(new MCElecScaleFactor(ctx, elecID_SFac_Data_part1, 0.0, "ID"));//must be here to avoid double weight vars in the TTree
  	//  const std::string& elecID_hist    = ctx.get("elecID_SF_hist");
  	//  elecIDSF.reset(new weightcalc_elecID(ctx, "electrons", elecID_SFac, elecID_hist, 0.00));
	// elec-ID
  	const std::string& elecGsf_SFac    = ctx.get("elecGsf_SF_file");
	//
	//!!  // elec-HLT
	//!!  const std::string& elecHLT_SFac   = ctx.get("elecHLT_SF_file");
	//!!  const std::string& elecHLT_hist   = ctx.get("elecHLT_SF_hist");
	//!!
	//!!  elecHLTSF.reset(new weightcalc_elecHLT(ctx, "electrons", elecHLT_SFac, elecHLT_hist, 0.005));
	//!!  //
  	// t-tagging
  	const std::string& ttag_SFac_file = ctx.get("ttag_SFs");
  	const std::string& ttag_effy_file = ctx.get("ttag_eff");
  	const std::string& ttag_effyL     = ctx.get("ttag_eff__jetL");
  	const std::string& ttag_effyT     = ctx.get("ttag_eff__jetT");
	ttagSF_ct .reset(new weightcalc_ttagging(ttag_SFac_file, ttag_wp, "comb", "comb", "CT", "CT", ttag_effy_file, ttag_effyL, ttag_effyT));
	ttagSF_upL.reset(new weightcalc_ttagging(ttag_SFac_file, ttag_wp, "comb", "comb", "UP", "CT", ttag_effy_file, ttag_effyL, ttag_effyT));
  	ttagSF_dnL.reset(new weightcalc_ttagging(ttag_SFac_file, ttag_wp, "comb", "comb", "DN", "CT", ttag_effy_file, ttag_effyL, ttag_effyT));
	ttagSF_upT.reset(new weightcalc_ttagging(ttag_SFac_file, ttag_wp, "comb", "comb", "CT", "UP", ttag_effy_file, ttag_effyL, ttag_effyT));
  	ttagSF_dnT.reset(new weightcalc_ttagging(ttag_SFac_file, ttag_wp, "comb", "comb", "CT", "DN", ttag_effy_file, ttag_effyL, ttag_effyT));

	////top-pt reweighting
   	if(ctx.get("dataset_version").find("TTbar") != std::string::npos || ctx.get("dataset_version").find("TTbar") != std::string::npos){
  		topptREWGT.reset(new TopPtReweight(ctx, 0.0615, -0.0005, ttbar_gen_label, "wgtMC__topptREWGT_ct"));
   	}
	ctx.undeclare_all_event_output();

  	//pileup (define it after undeclaring all other variables to keep the weights in the output)
  	pileupSF.reset(new MCPileupReweight(ctx));

  	// //muon ID scale factors
  	muonID_SF.reset(new MCMuonScaleFactor(ctx, muonID_SFac, muonID_directory, 1., "ID")); //hypot(1.0%,0.5%) //used 1.12
  	muonHLT_SF.reset(new MCMuonScaleFactor(ctx, muonHLT_SFac, muonHLT_directory, 5.0, "HLT",true));//TEST
	//muonTRK_SF.reset(new MCMuonTrkScaleFactor(ctx, muonTRK_SFac, 0.0, "TRK"));
	// // elec-ID
  	elecID_SF.reset(new MCElecScaleFactor(ctx, elecID_SFac, 0.0, "ID"));
  	// elecID_SF.reset(new MCElecScaleFactor2(ctx, elecID_SFac_Data_part1, elecID_SFac, lumi1, lumi2, 0.0, "ID"));
	// // elec-ID
  	elecGsf_SF.reset(new MCElecScaleFactor(ctx, elecGsf_SFac, 0.0, "Gsf"));
	// //b-tagging scale factors
  	btagSF.reset(new MCBTagScaleFactor(ctx, b_working_point,"jets","central","comb","incl","MCBtagEfficiencies"));
 	// event
  	h_run             = ctx.declare_event_output<int>("run");
  	h_lumi_block      = ctx.declare_event_output<int>("lumi_block");
  	h_event           = ctx.declare_event_output<int>("event");
	h_pvN             = ctx.declare_event_output<int>("pvN");
	h_muoN            = ctx.declare_event_output<int>("muoN");
  	h_eleN            = ctx.declare_event_output<int>("eleN");
	h_btagN           = ctx.declare_event_output<int>("btagN");
  	h_ttagN           = ctx.declare_event_output<int>("ttagN");
	h_ttagevt         = ctx.declare_event_output<bool>("ttagevt");
	h_ttgen           = ctx.declare_event_output<int>("ttgen");
	//lepton
  	h_lep1            = ctx.declare_event_output<TLorentzVector>("lep1");
  	h_lep1__pdgID     = ctx.declare_event_output<int>           ("lep1__pdgID");
  	h_lep1__charge    = ctx.declare_event_output<int>           ("lep1__charge");
  	h_lep1__minDR_jet = ctx.declare_event_output<float>         ("lep1__minDR_jet");
  	h_lep1__pTrel_jet = ctx.declare_event_output<float>         ("lep1__pTrel_jet");
	// jet
  	h_jet1            = ctx.declare_event_output<TLorentzVector>("jet1");
  	h_jet2            = ctx.declare_event_output<TLorentzVector>("jet2");
  	//
	// MET
  	h_MET             = ctx.declare_event_output<TLorentzVector>("MET");
  	h_MET__pz         = ctx.declare_event_output<float>         ("MET__pz");
  	//
	// ttbar reco hyp
  	h_rec_chi2        = ctx.declare_event_output<float>         ("rec_chi2");
  	h_rec_tlep        = ctx.declare_event_output<TLorentzVector>("rec_tlep");
  	h_rec_thad        = ctx.declare_event_output<TLorentzVector>("rec_thad");
  	//
	// weight
  	h_wgtMC__GEN            = ctx.declare_event_output<float>("wgtMC__GEN");
	// h_wgtMC__elecIDSF_ct    = ctx.declare_event_output<float>("wgtMC__elecIDSF_ct");
  	// h_wgtMC__elecIDSF_up    = ctx.declare_event_output<float>("wgtMC__elecIDSF_up");
  	// h_wgtMC__elecIDSF_dn    = ctx.declare_event_output<float>("wgtMC__elecIDSF_dn");
	h_wgtMC__elecHLTSF_ct   = ctx.declare_event_output<float>("wgtMC__elecHLTSF_ct");
  	h_wgtMC__elecHLTSF_up   = ctx.declare_event_output<float>("wgtMC__elecHLTSF_up");
  	h_wgtMC__elecHLTSF_dn   = ctx.declare_event_output<float>("wgtMC__elecHLTSF_dn");

	h_wgtMC__ttagSF_ct      = ctx.declare_event_output<float>("wgtMC__ttagSF_ct");
  	h_wgtMC__ttagSF_upL     = ctx.declare_event_output<float>("wgtMC__ttagSF_upL");
  	h_wgtMC__ttagSF_dnL     = ctx.declare_event_output<float>("wgtMC__ttagSF_dnL");
  	h_wgtMC__ttagSF_upT     = ctx.declare_event_output<float>("wgtMC__ttagSF_upT");
  	h_wgtMC__ttagSF_dnT     = ctx.declare_event_output<float>("wgtMC__ttagSF_dnT");

  	h_wgtMC__muR_ct__muF_up = ctx.declare_event_output<float>("wgtMC__muR_ct__muF_up");
  	h_wgtMC__muR_ct__muF_dn = ctx.declare_event_output<float>("wgtMC__muR_ct__muF_dn");
  	h_wgtMC__muR_up__muF_ct = ctx.declare_event_output<float>("wgtMC__muR_up__muF_ct");
  	h_wgtMC__muR_up__muF_up = ctx.declare_event_output<float>("wgtMC__muR_up__muF_up");
  	h_wgtMC__muR_dn__muF_ct = ctx.declare_event_output<float>("wgtMC__muR_dn__muF_ct");
  	h_wgtMC__muR_dn__muF_dn = ctx.declare_event_output<float>("wgtMC__muR_dn__muF_dn");

  	h_wgtMC__topptREWGT_ct  = ctx.declare_event_output<float>("wgtMC__topptREWGT_ct");
  	h_wgtMC__topptREWGT_up  = ctx.declare_event_output<float>("wgtMC__topptREWGT_up");
  	h_wgtMC__topptREWGT_dn  = ctx.declare_event_output<float>("wgtMC__topptREWGT_dn");

  	h_wgtMC__PDF            = ctx.declare_event_output<std::vector<float> >("wgtMC__PDF");
  	h_s33             = ctx.declare_event_output<float>("s33");
	//h_jet2_pt          = ctx.declare_event_output<float>          ("jet2_pt");
 	h_ht_met_lep_norm   = ctx.declare_event_output<float>          ("ht_met_lep_norm");
  	h_jet1_csv         = ctx.declare_event_output<float>           ("jet1_csv");
  	h_jet2_csv         = ctx.declare_event_output<float>           ("jet2_csv");
  	h_njets             = ctx.declare_event_output<float>             ("njets");
  	h_DRpt              = ctx.declare_event_output<float>           ("DRpt");
  	h_lep1__pTrel_jet_norm = ctx.declare_event_output<float>         ("lep1__pTrel_jet_norm"); 
  	h_jet1_m         = ctx.declare_event_output<float>("jet1_m");
  	h_lep1__minDR_norm = ctx.declare_event_output<float>("lep1__minDR_jet");
  	h_jet2_m         = ctx.declare_event_output<float>("jet2_m");
  	h_jet1pt =  ctx.declare_event_output<float>("h_jet1pt");
  	h_mttbar =  ctx.declare_event_output<float>("h_mttbar");
  	h_jet1pt_chi2 = ctx.declare_event_output<float>("h_jet1pt_chi2");
  	h_mttbar_chi2 = ctx.declare_event_output<float>("h_mttbar_chi2");

    WJets_TMVA_response = -100;
    wjets_TMVA_response = ctx.declare_event_output<float>("WJets_TMVA_response");
    H_Rec_chi2 = ctx.declare_event_output<float>("H_Rec_chi2");
    //h_wjets_bdtresponse = ctx.declare_event_output<float>("wjets_bdtresponse");
    TMVA::Tools::Instance();
    reader_wjets = new TMVA::Reader();
    reader_wjets->AddVariable("DRpt", &mva_DRpt_norm);
    reader_wjets->AddVariable("ht_met_lep_norm", &mva_ht_met_lep_norm);
    reader_wjets->AddVariable("lep1__minDR_jet", &mva_lep1__minDR_jet);
    reader_wjets->AddVariable("lep1__pTrel_jet_norm", &mva_ptrel_norm);
    reader_wjets->AddVariable("jet1_csv", &mva_j1CSV);
    //reader_wjets->AddVariable("jet1_pt", &mva_jet1pt_norm);
    reader_wjets->AddVariable("jet1_m", &mva_jet1m_norm);
    reader_wjets->AddVariable("jet2_csv", &mva_j2CSV);
    //reader_wjets->AddVariable("jet2_pt",&mva_jet2pt_norm);
    reader_wjets->AddVariable("jet2_m", &mva_jet2m_norm);
    reader_wjets->AddVariable("njets", &mva_njets);
    reader_wjets->AddVariable("s33", &mva_s33);
    reader_wjets->BookMVA("BDT method", "../TMVA_weights/80X/weights_80Xv3BDT_OPTIMIZED_50-001-10-0p2/TMVAClassification_BDT.weights.xml");
}

bool TTbarLJAnalysisLiteModule::process(uhh2::Event& event){
	event.set(wjets_TMVA_response,0);//set some dummy initial value
  	//
  	event.set(H_Rec_chi2,0);
  	//event.set(h_jet1_pt,0);
  	event.set(h_jet1_m,0);
  	event.set(h_jet1_csv,0);
  	event.set(h_jet2_m,0);   
  	//event.set(h_lep1__minDR_norm,0); 
  	event.set(h_jet2_csv,0);  
  	event.set(h_njets,1);
  	event.set(h_ht_met_lep_norm,0);
  	event.set(h_lep1__pTrel_jet_norm,0);
  	event.set(h_lep1__minDR_norm,0);
  	event.set(h_s33,0);
  	event.set(h_DRpt,0);
  	event.set(h_jet1pt_chi2,0);
  	event.set(h_mttbar_chi2,0);
 //// COMMON MODULES
  	float w_GEN(1.);
  	if(!event.isRealData)
    	w_GEN = event.weight;
	if(!event.isRealData){
    	ttgenprod->process(event);
  	}
	/* CMS-certified luminosity sections */
  	if(event.isRealData){
		if(!lumi_sel->passes(event)) return false;
 	}
  	//  std::cout<<"Pile up next..."<<std::endl;
  	////
  	//// Data/MC scale factors
  	//  float w_elecIDSF_ct(1.) , w_elecIDSF_up(1.) , w_elecIDSF_dn(1.);
  	float w_elecHLTSF_ct(1.), w_elecHLTSF_up(1.), w_elecHLTSF_dn(1.);
  	float w_ttagSF_ct(1.), w_ttagSF_upL(1.), w_ttagSF_dnL(1.), w_ttagSF_upT(1.), w_ttagSF_dnT(1.);
  	float w_muR_ct__muF_up(1.), w_muR_ct__muF_dn(1.), w_muR_up__muF_ct(1.), w_muR_up__muF_up(1.), w_muR_dn__muF_ct(1.), w_muR_dn__muF_dn(1.);
  	float w_topptREWGT_up(1.), w_topptREWGT_dn(1.),w_topptREWGT_ct(1.);
  	//float w_wjetsREWGT_ct(1.);
  	std::vector<float> w_PDF;
  	w_PDF.clear();

  	//pileup
  	pileupSF->process(event);
  	btagSF->process(event);
  	muonID_SF->process(event);
  	muonHLT_SF->process(event);
  	//  muon-Trk
  	//muonTRK_SF->process(event);
  	//  std::cout<<event.weight<<std::endl;	
  	// // elec-ID
  	//std::cout<<event.weight<<std::endl;
  	elecID_SF->process(event);
  	//std::cout<<event.weight<<std::endl;
  	// elec-Gsf
  	elecGsf_SF->process(event);
  	if(!event.isRealData){
    	//std::cout<<event.weight<<std::endl;
    	//std::cout<<""<<std::endl;
    
    	// w_elecIDSF_ct  = elecIDSF->weight(event, "CT");
    	// w_elecIDSF_up  = elecIDSF->weight(event, "UP");
    	// w_elecIDSF_dn  = elecIDSF->weight(event, "DN");
    	// //
   
    	// elec-HLT
    	w_elecHLTSF_ct = 0.9598;//!!elecHLTSF->weight(event, "CT");
    	w_elecHLTSF_up = 0.9665;//!!elecHLTSF->weight(event, "UP");
    	w_elecHLTSF_dn = 0.9531;//!!elecHLTSF->weight(event, "DN");
    	//
    	// t-tagging
    	 w_ttagSF_ct    = ttagSF_ct ->weight(event);
		w_ttagSF_upL   = ttagSF_upL->weight(event);
    	//if(fabs(w_ttagSF_upL)>10.)
     	// std::cout<<"!!!!!!!!!!!!!!!! w_ttagSF_upL = "<<w_ttagSF_upL<<std::endl;
    	w_ttagSF_dnL   = ttagSF_dnL->weight(event);
    	w_ttagSF_upT   = ttagSF_upT->weight(event);
    	//if(fabs(w_ttagSF_upT)>10.)
    	//std::cout<<"!!!!!!!!!!!!!! w_ttagSF_upT = "<<w_ttagSF_upT<<std::endl;
    	w_ttagSF_dnT   = ttagSF_dnT->weight(event);
    	// Renormalization/Factorization scales
    	if(event.genInfo){
			if(event.genInfo->systweights().size() >= 9){
				if(event.genInfo->originalXWGTUP()){
					w_muR_ct__muF_up = (event.genInfo->systweights().at(1) / event.genInfo->originalXWGTUP());
          			w_muR_ct__muF_dn = (event.genInfo->systweights().at(2) / event.genInfo->originalXWGTUP());
          			w_muR_up__muF_ct = (event.genInfo->systweights().at(3) / event.genInfo->originalXWGTUP());
          			w_muR_up__muF_up = (event.genInfo->systweights().at(4) / event.genInfo->originalXWGTUP());
          			w_muR_dn__muF_ct = (event.genInfo->systweights().at(6) / event.genInfo->originalXWGTUP());
          			w_muR_dn__muF_dn = (event.genInfo->systweights().at(8) / event.genInfo->originalXWGTUP());
          			if(store_PDF_weights_){
						for(unsigned int i=9; i<event.genInfo->systweights().size(); ++i){
							const float pdf_w(event.genInfo->systweights().at(i) / event.genInfo->originalXWGTUP());
              				w_PDF.push_back(pdf_w);
            			}
            		}
            	}
            }
        }
        if(topptREWGT.get()){
        	////    std::cout<<"APPLYING TOP PT REWEIGHT!"<<std::endl;
      		topptREWGT->process(event);
      		float w_topptREWGT_ct = event.get(h_wgtMC__topptREWGT_ct);
       		///// std::cout<<w_topptREWGT_ct<<std::endl;
      		//apply twice the shift as uncertainty
      		w_topptREWGT_dn = w_topptREWGT_ct*w_topptREWGT_ct;
      		w_topptREWGT_up = 1;
      		event.weight *= w_topptREWGT_ct;	
    	}
    	//event.weight *= w_topptREWGT_ct;
	}//end of only for not data
	
	bool pass_lepV(0);
  	if     (channel_ == muon) pass_lepV = ((event.electrons->size() == 0));
  	else if(channel_ == elec) pass_lepV = ((event.muons    ->size() == 0));
  	if(!pass_lepV) return false;
  	//due to unknown efficiency in 2016 data, skip events with: muon pt>500 GeV in the endcap
  	bool good_muon(0);
  	if (channel_ == muon){
    	const Particle* lep1 = leading_lepton(event);
    	good_muon = (!(lep1->pt()>muo1_pt_max_ && lep1->eta()>muo1_eta_max_));
  	}
  	if(!good_muon && channel_ == muon) return false;
  	int lepN(-1);
  	if     (channel_ == muon) lepN = int(event.muons    ->size());
  	else if(channel_ == elec) lepN = int(event.electrons->size());
  	if(!(lepN >= 1)) return false;
  	// pt-leading lepton selection
  	const Particle* lep1 = leading_lepton(event);
  	if(!(lep1->pt() > lep1_pt_)) return false; 
 
  	//// JET selection
	/* 2nd AK4 jet selection */
  	const bool pass_jet2 = jet2_sel->passes(event);
  	if(!pass_jet2) return false;
	/* 1st AK4 jet selection */
  	const bool pass_jet1 = jet1_sel->passes(event);
  	if(!pass_jet1) return false;
	////Wpt selection
  	//const float wpt = (event.met->v4() + lep1->v4()).Pt();
  	//if (!(wpt > 200.)) return false;
  	//// HLT selection
  	const bool pass_trigger = trigger_sel->passes(event);
  	bool pass_trigger2 = false;
  	//pass_trigger2 = trigger2_sel->passes(event);
    //comment out if for MC
    if(event.isRealData){
    	if(event.run>274953 && channel_ == muon)
    	pass_trigger2 = trigger2_sel->passes(event);	
    }
  	else pass_trigger2 = trigger2_sel->passes(event);
	if(channel_ == elec) event.weight *= w_elecHLTSF_ct;
  	//// MET selection
  	const bool pass_met = met_sel->passes(event);
  	if(!pass_met) return false;
	//// HT_lep selection
  	const bool pass_htlep = htlep_sel->passes(event);
  	if(!pass_htlep) return false;
	//// TRIANGULAR-CUTS selection
  	const bool pass_triangc = triangc_sel->passes(event);
  	if(!pass_triangc) return false;
 	//// TTBAR KIN RECO

  	/* TOPTAG-EVENT boolean */
  	const bool pass_ttagevt = ttagevt_sel->passes(event) && use_ttagging_;
  	const std::string ttag_posx = (pass_ttagevt ? "t1" : "t0");
	reco_primlep->process(event);
	if(!pass_ttagevt){ ttbar_reco__ttag0->process(event); ttbar_chi2__ttag0->process(event); }
  	else             { ttbar_reco__ttag1->process(event); ttbar_chi2__ttag1->process(event); }
	std::vector<ReconstructionHypothesis>& ttbar_hyps = event.get(h_ttbar_hyps);
	////
	//// LEPTONIC-TOP pt selection
  	const bool pass_topleppt = topleppt_sel->passes(event);
  	if(!pass_topleppt) return false;
  	//// FINAL selection
	 /* ttagN counters */
  	int ttagN(0);
  	for(const auto& tj : *event.topjets) if(ttag_ID_(tj, event)) ++ttagN;
  	/* btagN counters */
  	int jetbtagN(0), subjbtagN(0);
	if(!pass_ttagevt){
		for(const auto& j : *event.jets) if(btag_ID_(j, event)) ++jetbtagN;
  	}
  	else {
		const ReconstructionHypothesis* ttbar_hyp = get_best_hypothesis(ttbar_hyps, "Chi2");
		const TopJet* thad = ttbar_hyp->tophad_topjet_ptr();
		for(const auto& j : thad->subjets()) if(btag_ID_(j, event)) ++subjbtagN;
    	for(const auto& j : *event.jets){
			if(!(uhh2::deltaR(*thad, j) > ttag_minDR_jet_)) continue;
      		if(btag_ID_(j, event)) ++jetbtagN;
    	}
  	}
	const int btagN = jetbtagN + subjbtagN;
	////const std::string btag_posx =  ( pass_ttagevt ? "b" :(btagN == 0 ? "b0": "b1"));
  	//const std::string btag_posx = (btagN >= 2 ? "b2" : (btagN >= 1 ? "b1" : "b0"));
  	/******************/
	/* CHI2 selection */
  	const bool pass_chi2 = chi2_sel->passes(event);
	const std::string chi2_posx = pass_chi2 ? "chi2" : "antichi2";
  	/******************/
	/* DATA blinding */
  	//if(blind_DATA_ && event.isRealData ){
		//if(lepN == 1 && pass_chi2){

      //const ReconstructionHypothesis* rec_ttbar = get_best_hypothesis(ttbar_hyps, "Chi2");

      //const float rec_ttbar_M((rec_ttbar->top_v4()+rec_ttbar->antitop_v4()).M());

      //if(!( rec_ttbar_M < 2000. )) return false;
    	//}
 	// }
 	/*****************/
   	const ReconstructionHypothesis* rec_ttbar = get_best_hypothesis(ttbar_hyps, "Chi2");
   	//const float rec_ttbar_M((rec_ttbar->top_v4()+rec_ttbar->antitop_v4()).M());
   	//if (!(rec_ttbar_M < 2000.)) return false;
   	/* KINE plots */
  	if(lepN == 1){
		HFolder("kine")       ->fill(event);
    	HFolder("kine__ttbar")->fill(event);
		if(!pass_ttagevt){
      		//HFolder("kine__"+ttag_posx+btag_posx)          ->fill(event);
     		// HFolder("kine__"+ttag_posx+btag_posx+"__ttbar")->fill(event);
       		HFolder("kine__"+ttag_posx)          ->fill(event);
       		HFolder("kine__"+ttag_posx+"__ttbar")          ->fill(event);
     	}
    	else{
       		//HFolder("kine__"+ttag_posx+btag_posx)          ->fill(event);
       		HFolder("kine__"+ttag_posx)          ->fill(event); 
       		HFolder("kine__"+ttag_posx+"__ttbar")->fill(event);
       		//HFolder("kine__"+ttag_posx+btag_posx+"__ttbar")->fill(event);
    	}
  	}
  	if(lepN == 2){
		bool l2_ossf(false);
    	if     (channel_ == muon) l2_ossf = ((event.muons    ->at(0).charge() * event.muons    ->at(1).charge()) == -1);
    	else if(channel_ == elec) l2_ossf = ((event.electrons->at(0).charge() * event.electrons->at(1).charge()) == -1);
		if(l2_ossf && chi2_posx == "chi2"){
			HFolder(chi2_posx+"_L2")          ->fill(event);
      		HFolder(chi2_posx+"_L2"+"__dilep")->fill(event);
      		HFolder(chi2_posx+"_L2"+"__ttbar")->fill(event);
      		if(!pass_ttagevt){
			//HFolder(chi2_posx+"_L2"+"__"+ttag_posx+btag_posx)          ->fill(event);
			//HFolder(chi2_posx+"_L2"+"__"+ttag_posx+btag_posx+"__dilep")->fill(event);
			//HFolder(chi2_posx+"_L2"+"__"+ttag_posx+btag_posx+"__ttbar")->fill(event);
        		HFolder(chi2_posx+"_L2"+"__"+ttag_posx)          ->fill(event);
        		HFolder(chi2_posx+"_L2"+"__"+ttag_posx+"__dilep")->fill(event);
        		HFolder(chi2_posx+"_L2"+"__"+ttag_posx+"__ttbar")->fill(event);
      		}
      		else{
         	// HFolder(chi2_posx+"_L2"+"__"+ttag_posx+btag_posx)          ->fill(event);
          	//HFolder(chi2_posx+"_L2"+"__"+ttag_posx+btag_posx+"__dilep")->fill(event);
          	//HFolder(chi2_posx+"_L2"+"__"+ttag_posx+btag_posx+"__ttbar")->fill(event);
          		HFolder(chi2_posx+"_L2"+"__"+ttag_posx)          ->fill(event);
          		HFolder(chi2_posx+"_L2"+"__"+ttag_posx+"__dilep")->fill(event);
          		HFolder(chi2_posx+"_L2"+"__"+ttag_posx+"__ttbar")->fill(event);
      		}
    	}
  	}
  	//// VARS
	// event
  	event.set(h_run       , event.run);
  	event.set(h_lumi_block, event.luminosityBlock);
  	event.set(h_event     , event.event);
	event.set(h_pvN  , event.pvs->size());
	event.set(h_muoN , event.muons    ->size());
  	event.set(h_eleN , event.electrons->size());
	event.set(h_btagN, btagN);
  	event.set(h_ttagN, ttagN);
	event.set(h_ttagevt, pass_ttagevt);
	int ttgen_decay(-1);
  	if(!event.isRealData){
		const auto& ttgen = event.get(h_ttbar_gen);
		if     (ttgen.DecayChannel() == TTbarGen::e_notfound) ttgen_decay = -1;
    	else if(ttgen.DecayChannel() == TTbarGen::e_had     ) ttgen_decay = 00;
    	else if(ttgen.DecayChannel() == TTbarGen::e_ehad    ) ttgen_decay = 01;
    	else if(ttgen.DecayChannel() == TTbarGen::e_muhad   ) ttgen_decay = 02;
    	else if(ttgen.DecayChannel() == TTbarGen::e_tauhad  ) ttgen_decay = 03;
    	else if(ttgen.DecayChannel() == TTbarGen::e_ee      ) ttgen_decay = 11;
    	else if(ttgen.DecayChannel() == TTbarGen::e_emu     ) ttgen_decay = 12;
    	else if(ttgen.DecayChannel() == TTbarGen::e_etau    ) ttgen_decay = 13;
    	else if(ttgen.DecayChannel() == TTbarGen::e_mumu    ) ttgen_decay = 22;
    	else if(ttgen.DecayChannel() == TTbarGen::e_mutau   ) ttgen_decay = 23;
    	else if(ttgen.DecayChannel() == TTbarGen::e_tautau  ) ttgen_decay = 33;
  	}
	event.set(h_ttgen, ttgen_decay);
	// jet
  	if(!(event.jets->size() >= 2)) throw std::runtime_error("TTbarLJAnalysisLiteModule::process -- logic error: jet multiplicity < 2 ("+std::to_string(event.jets->size())+")");
	TLorentzVector jet1__p4(event.jets->at(0).v4().Px(), event.jets->at(0).v4().Py(), event.jets->at(0).v4().Pz(), event.jets->at(0).v4().E());
  	TLorentzVector jet2__p4(event.jets->at(1).v4().Px(), event.jets->at(1).v4().Py(), event.jets->at(1).v4().Pz(), event.jets->at(1).v4().E());
	event.set(h_jet1           , jet1__p4);
  	event.set(h_jet2           , jet2__p4);
	const int jet_n = event.jets->size();
    const Jet* jet1 = &event.jets->at(0);
    ljet_eta = jet1->eta();
    ljet_CSV = jet1->btag_combinedSecondaryVertexMVA();
   	if(jet_n>1){
        const Jet* jet2 =  &event.jets->at(1);
        fjet2_pt = jet2->pt(); fjet2_eta = jet2->eta(); //fjet2_phi = jet2->phi();
        //jet2_M = jet2->v4().M();
        jet2_CSV = jet2->btag_combinedSecondaryVertexMVA();
        //std::cout<<"jet2_CSV = "<<jet2_CSV<<std::endl;
    }
   	// MET
    event.set(h_MET, TLorentzVector(event.met->v4().Px(), event.met->v4().Py(), event.met->v4().Pz(), event.met->v4().P()));
    met_pt = event.met->pt();
   	TLorentzVector lep1__p4(0.,0.,0.,0.);
  	int   lep1__pdgID(0), lep1__charge(0); 
  	float lep1__minDR_jet(-1.), lep1__pTrel_jet(-1.);
  	if(channel_ == muon){
		lep1__pdgID     = lep1->charge() * -13;
    	lep1__charge    = lep1->charge();
    	lep1__minDR_jet = ((Muon*) lep1)->get_tag(Muon::twodcut_dRmin);
    	lep1__pTrel_jet = ((Muon*) lep1)->get_tag(Muon::twodcut_pTrel);
    	lep1__p4        = TLorentzVector(lep1->v4().Px(), lep1->v4().Py(), lep1->v4().Pz(), lep1->v4().E()); 
  	}
  	else if(channel_ == elec){
		lep1__pdgID     = lep1->charge() * -11;
    	lep1__charge    = lep1->charge();
    	lep1__minDR_jet = ((Electron*) lep1)->get_tag(Electron::twodcut_dRmin);
    	lep1__pTrel_jet = ((Electron*) lep1)->get_tag(Electron::twodcut_pTrel);
  	  	lep1__p4        = TLorentzVector(lep1->v4().Px(), lep1->v4().Py(), lep1->v4().Pz(), lep1->v4().E());
  	}

	event.set(h_lep1           , lep1__p4);
    event.set(h_lep1__pdgID    , lep1__pdgID);
    event.set(h_lep1__charge   , lep1__charge);
    event.set(h_lep1__minDR_jet, lep1__minDR_jet);
    event.set(h_lep1__pTrel_jet, lep1__pTrel_jet);
    
    // ttbar reco hyp
  	//const ReconstructionHypothesis* rec_ttbar = get_best_hypothesis(ttbar_hyps, "Chi2");
  	if(!rec_ttbar) throw std::runtime_error("TTbarLJAnalysisLiteModule::process -- logic error: ttbar reconstruction hypothesis (\"get_best_hypothesis\", discr=Chi2) not found");
  	const float MET__pz = rec_ttbar->neutrino_v4().Pz();
  	event.set(h_MET__pz, MET__pz);
	const float rec_chi2 = rec_ttbar->discriminator("Chi2");
  	const LorentzVector tlep(rec_ttbar->toplep_v4());
  	const LorentzVector thad(rec_ttbar->tophad_v4());
	event.set(h_rec_tlep, TLorentzVector(tlep.Px(), tlep.Py(), tlep.Pz(), tlep.E()));
  	event.set(h_rec_thad, TLorentzVector(thad.Px(), thad.Py(), thad.Pz(), thad.E()));
  	// weight
 	event.set(h_wgtMC__GEN         , w_GEN);
	// event.set(h_wgtMC__elecIDSF_ct , w_elecIDSF_ct);
  	// event.set(h_wgtMC__elecIDSF_up , w_elecIDSF_up);
  	// event.set(h_wgtMC__elecIDSF_dn , w_elecIDSF_dn);
	event.set(h_wgtMC__elecHLTSF_ct, w_elecHLTSF_ct);
  	event.set(h_wgtMC__elecHLTSF_up, w_elecHLTSF_up);
  	event.set(h_wgtMC__elecHLTSF_dn, w_elecHLTSF_dn);
  	event.set(h_wgtMC__ttagSF_ct  , w_ttagSF_ct );
  	event.set(h_wgtMC__ttagSF_upL , w_ttagSF_upL);
  	event.set(h_wgtMC__ttagSF_dnL , w_ttagSF_dnL);
  	event.set(h_wgtMC__ttagSF_upT , w_ttagSF_upT);
  	event.set(h_wgtMC__ttagSF_dnT , w_ttagSF_dnT);
	event.set(h_wgtMC__muR_ct__muF_up, w_muR_ct__muF_up);
  	event.set(h_wgtMC__muR_ct__muF_dn, w_muR_ct__muF_dn);
  	event.set(h_wgtMC__muR_up__muF_ct, w_muR_up__muF_ct);
  	event.set(h_wgtMC__muR_up__muF_up, w_muR_up__muF_up);
  	event.set(h_wgtMC__muR_dn__muF_ct, w_muR_dn__muF_ct);
  	event.set(h_wgtMC__muR_dn__muF_dn, w_muR_dn__muF_dn);

	if(!topptREWGT.get())
  	event.set(h_wgtMC__topptREWGT_ct , 1.);
	event.set(h_wgtMC__topptREWGT_up , w_topptREWGT_up);
  	event.set(h_wgtMC__topptREWGT_dn , w_topptREWGT_dn);
  	event.set(h_wgtMC__wjetsREWGT_ct , w_wjetsREWGT_ct);
  	event.set(h_wgtMC__PDF, std::move(w_PDF));

	const ReconstructionHypothesis* rec_ttbar_ = get_best_hypothesis(ttbar_hyps, "Chi2");
  	rec_ttbar_M_ = ((rec_ttbar_->top_v4()+rec_ttbar_->antitop_v4()).M());
  	event.set(h_mttbar, rec_ttbar_M_);
  	event.set(h_jet1pt,jet1__p4.Pt());
  	event.set(h_rec_chi2, rec_chi2);
   	if (rec_chi2 < 30){
       event.set(h_mttbar_chi2, rec_ttbar_M_);
       event.set(h_jet1pt_chi2, jet1__p4.Pt()); 
   	}
   	//FILL HERE THE BDT WJETS VARIABLES
    float s11 = 0.0;
    float s12 = 0.0;
    float s13 = 0.0;
    float s22 = 0.0;
    float s23 = 0.0;
    float s33 = 0.0;
    float s11d = 0.0;
    float s12d = 0.0;
    float s13d = 0.0;
    float s22d = 0.0;
    float s23d = 0.0;
    float s33d =0.0;
    float mag = 0.0;
    float ht=0.0;
    for (auto jet : *event.jets){
        if (jet.pt()>30.0){
            ht+=jet.pt();
            s11d += jet.v4().Px()*jet.v4().Px();
            mag  += (jet.v4().Px()*jet.v4().Px()+jet.v4().Py()*jet.v4().Py()+jet.v4().Pz()*jet.v4().Pz());
            s22d += jet.v4().Py()*jet.v4().Py();
            s12d += jet.v4().Px()*jet.v4().Py();
            s13d += jet.v4().Px()*jet.v4().Pz();
            s23d += jet.v4().Py()*jet.v4().Pz();
            s33d += jet.v4().Pz()*jet.v4().Pz();
        }
    }
    s11 =  s11d/mag;
    s12 =  s12d/mag;
    s13 =  s13d/mag;
    s22 =  s22d/mag;
    s23 =  s23d/mag;
    s33 =  s33d/mag;
    const float j1CSV    = event.jets->at(0).btag_combinedSecondaryVertex();
    const float j2CSV    = event.jets->at(1).btag_combinedSecondaryVertex();
    const float njets    = event.jets->size();
    const float j1M      = jet1__p4.M();
    const float j2M      = jet2__p4.M();
    
    const float jet1pt_norm = jet1__p4.Pt()/((rec_ttbar->top_v4()+rec_ttbar->antitop_v4()).M());
    const float jet2pt_norm = jet2__p4.Pt()/((rec_ttbar->top_v4()+rec_ttbar->antitop_v4()).M());
    const float ptrel_norm =  lep1__pTrel_jet/((rec_ttbar->top_v4()+rec_ttbar->antitop_v4()).M());
    const float htmetlep_norm = (ht+met_pt+lep1__p4.Pt())/((rec_ttbar->top_v4()+rec_ttbar->antitop_v4()).M());
    const float DRpt_norm = lep1__minDR_jet*jet1__p4.Pt()/((rec_ttbar->top_v4()+rec_ttbar->antitop_v4()).M());
    
    /*event.set(h_DRpt, DRpt_norm);//1
     event.set(h_ht_met_lep_norm, htmetlep_norm);//2
     event.set(h_lep1__minDR_norm, lep1__minDR_jet);//3
     event.set(h_lep1__pTrel_jet_norm, ptrel_norm);//4
     event.set(h_jet1_pt,jet1pt_norm);//5
     event.set(h_jet2_pt,jet2pt_norm);//6
     event.set(h_njets, njets); //9
     event.set(h_jet1_m,  j1M/((rec_ttbar->top_v4()+rec_ttbar->antitop_v4()).M())); //10
     event.set(h_jet2_m,  j2M/((rec_ttbar->top_v4()+rec_ttbar->antitop_v4()).M())); //11
     */
    # define N 3
    double d[N];
    double sph_mat[N*N] = {
        s11, s12, s13,
        s12, s22, s23,
        s13, s23, s33
    };
    double v[N*N];
    int n = 3;
    int it_max = 100;
    jacobi_eigenvalue( n, sph_mat, it_max, v, d, it_num, rot_num);
    const float sphericity = 1.5*(d[1]+d[2]);
    const float aplanarity = 1.5*d[0];
    
    mva_jet1pt_norm = jet1pt_norm;
    mva_jet2pt_norm = jet2pt_norm;
    mva_ptrel_norm =  ptrel_norm;
    mva_ht_met_lep_norm = htmetlep_norm;
    mva_jet1m_norm = j1M/((rec_ttbar->top_v4()+rec_ttbar->antitop_v4()).M());
    mva_jet2m_norm = j2M/((rec_ttbar->top_v4()+rec_ttbar->antitop_v4()).M());
    mva_DRpt_norm = DRpt_norm;
    mva_lep1__minDR_jet = lep1__minDR_jet;
    mva_j1CSV = j1CSV;
    mva_j2CSV = j2CSV;
    mva_njets = njets;
    mva_s11 = s11;
    mva_s12 = s12;
    mva_s13 = s13;
    mva_s22 = s22;
    mva_s23 = s23;
    mva_s33 = s33;
    mva_aplanarity= aplanarity;
    mva_sphericity = sphericity;
    WJets_TMVA_response = reader_wjets->EvaluateMVA("BDT method");
    event.set(wjets_TMVA_response, WJets_TMVA_response);
    event.set(H_Rec_chi2, rec_chi2);
    event.set(h_DRpt, DRpt_norm);//1
    event.set(h_ht_met_lep_norm, htmetlep_norm);//2
    event.set(h_lep1__minDR_norm, lep1__minDR_jet);//3
    event.set(h_lep1__pTrel_jet_norm, ptrel_norm);//4
    //event.set(h_jet1_pt,jet1pt_norm);//5
   // event.set(h_jet2_pt,jet2pt_norm);//6
    event.set(h_jet1_csv, j1CSV );//7
    event.set(h_jet2_csv, j2CSV );//8
    event.set(h_njets, njets); //9
    event.set(h_jet1_m,  j1M/((rec_ttbar->top_v4()+rec_ttbar->antitop_v4()).M())); //10
    event.set(h_jet2_m,  j2M/((rec_ttbar->top_v4()+rec_ttbar->antitop_v4()).M())); //11
    event.set(h_s33, s33);
    
    //if(lepN == 1 && btagN>=1){
    if (lepN ==1){
    	HFolder(chi2_posx)          ->fill(event);
      	HFolder(chi2_posx+"__ttbar")->fill(event);
      	if(!pass_ttagevt){
      		//HFolder(chi2_posx+"__"+ttag_posx+btag_posx)          ->fill(event);
      		//      //HFolder(chi2_posx+"__"+ttag_posx+btag_posx+"__ttbar")->fill(event);
        	HFolder(chi2_posx+"__"+ttag_posx)          ->fill(event);
        	HFolder(chi2_posx+"__"+ttag_posx+"__ttbar")->fill(event);
      	}
      	else{
      		//HFolder(chi2_posx+"__"+ttag_posx+btag_posx)          ->fill(event);
      		//HFolder(chi2_posx+"__"+ttag_posx+btag_posx+"__ttbar")->fill(event);
        	HFolder(chi2_posx+"__"+ttag_posx)          ->fill(event);
        	HFolder(chi2_posx+"__"+ttag_posx+"__ttbar")->fill(event);
      	}
      //    if(pass_chi2 && !event.isRealData){
      	if(!event.isRealData){
        	h_btagMCeffi->fill(event);
      	}
    }
    if (pass_chi2 && lepN==1 && WJets_TMVA_response > 0.5){
        //if (pass_chi2 && lepN==1){
        HFolder("chi2_bdt")          ->fill(event);
        HFolder("chi2_bdt__ttbar")->fill(event);
        //event.set(wjets_TMVA_response, WJets_TMVA_response);
        HFolder("chi2_bdt__"+ttag_posx          )->fill(event);
        HFolder("chi2_bdt__"+ttag_posx+"__ttbar")->fill(event);
        //HFolder("chi2_bdt__"+ttag_posx+btag_posx)          ->fill(event);
        //HFolder("chi2_bdt__"+ttag_posx+btag_posx+"__ttbar")->fill(event);          
    }
    if (pass_chi2 && lepN==1 && WJets_TMVA_response < 0.5){
        //if (pass_chi2 && lepN==1){
        HFolder("chi2_antibdt")          ->fill(event);
        HFolder("chi2_antibdt__ttbar")->fill(event);
        //                        //event.set(wjets_TMVA_response, WJets_TMVA_response);
       // HFolder("chi2_antibdt__"+ttag_posx+btag_posx)         ->fill(event);
       // HFolder("chi2_antibdt__"+ttag_posx+btag_posx+"__ttbar")->fill(event);
        HFolder("chi2_antibdt__"+ttag_posx)         ->fill(event);
        HFolder("chi2_antibdt__"+ttag_posx+"__ttbar")->fill(event);
    }
    if (!pass_chi2 && lepN==1 && WJets_TMVA_response > 0.5){
       HFolder("antichi2_bdt")          ->fill(event);
       HFolder("antichi2_bdt__ttbar")->fill(event);
       //HFolder("antichi2_bdt__"+ttag_posx+btag_posx)          ->fill(event);
       //HFolder("antichi2_bdt__"+ttag_posx+btag_posx+"__ttbar")->fill(event);
       HFolder("antichi2_bdt__"+ttag_posx)          ->fill(event);
       HFolder("antichi2_bdt__"+ttag_posx+"__ttbar")->fill(event);  
    }
    if (!pass_chi2 && lepN==1 && WJets_TMVA_response < 0.5){
      HFolder("antichi2_antibdt")          ->fill(event);
      HFolder("antichi2_antibdt__ttbar")->fill(event);
      HFolder("antichi2_antibdt__"+ttag_posx)          ->fill(event);
      HFolder("antichi2_antibdt__"+ttag_posx+"__ttbar")->fill(event);
      //HFolder("antichi2_antibdt__"+ttag_posx+btag_posx)          ->fill(event);
      //HFolder("antichi2_antibdt__"+ttag_posx+btag_posx+"__ttbar")->fill(event);   
    }
  	HFolder("Final")->fill(event);
  	HFolder("Final__ttbar")->fill(event);
  	//  std::cout<<event.weight<<std::endl;
  	return true;
}  	
UHH2_REGISTER_ANALYSIS_MODULE(TTbarLJAnalysisLiteModule)

 






