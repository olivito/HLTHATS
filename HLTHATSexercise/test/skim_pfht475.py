import FWCore.ParameterSet.Config as cms

process = cms.Process('SKIM')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

# Input source
process.source = cms.Source("PoolSource",
   fileNames = cms.untracked.vstring(
       '/store/data/Run2016B/JetHT/MINIAOD/PromptReco-v2/000/273/730/00000/04C3D8B6-B821-E611-B245-02163E011979.root',
       '/store/data/Run2016B/JetHT/MINIAOD/PromptReco-v2/000/273/730/00000/06D365BD-B821-E611-9B47-02163E014235.root',
       '/store/data/Run2016B/JetHT/MINIAOD/PromptReco-v2/000/273/730/00000/0CC04CB6-B821-E611-833C-02163E011979.root',
       '/store/data/Run2016B/JetHT/MINIAOD/PromptReco-v2/000/273/730/00000/10B6096D-6421-E611-AEF0-02163E0129FB.root',
       '/store/data/Run2016B/JetHT/MINIAOD/PromptReco-v2/000/273/730/00000/1227EEEC-5021-E611-8B5B-02163E01466B.root',
   ),
)


process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.20 $'),
    annotation = cms.untracked.string('RelVal nevts:100'),
    name = cms.untracked.string('Applications')
)

# JSON

import FWCore.PythonUtilities.LumiList as LumiList
process.source.lumisToProcess = LumiList.LumiList(filename = 'Cert_271036-273730_13TeV_PromptReco_Collisions16_JSON.txt').getVLuminosityBlockRange()

# skim definitions

process.triggerSelection = cms.EDFilter( "TriggerResultsFilter",
 triggerConditions = cms.vstring(
   'HLT_PFHT475_v*',
 ),
 hltResults = cms.InputTag( "TriggerResults", "", "HLT" ),
 l1tResults = cms.InputTag("" ),                                         
 throw = cms.bool( True )
)

process.trigsel_step = cms.Path(process.triggerSelection)


# Output definition

process.Out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string ("skim_pfht475_5files.root"),
                               outputCommands = cms.untracked.vstring('keep *'),
                               SelectEvents = cms.untracked.PSet(
                                   SelectEvents = cms.vstring('trigsel_step')
                               ),
)
  
process.GlobalTag.globaltag = "80X_dataRun2_Prompt_v8"

# Path and EndPath definitions
process.output_step = cms.EndPath(process.Out)

process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 10000
