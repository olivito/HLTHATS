import FWCore.ParameterSet.Config as cms

process = cms.Process('HLTANALYZER')

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
#   fileNames = cms.untracked.vstring('/store/data/Run2016B/JetHT/MINIAOD/PromptReco-v2/000/273/730/00000/04C3D8B6-B821-E611-B245-02163E011979.root'),
   fileNames = cms.untracked.vstring('file:/eos/uscms/store/user/cmsdas/2016/SHORT_EXERCISES/Trigger/HATS/skim_JetHT_pfht475_15k.root'),
)


process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.20 $'),
    annotation = cms.untracked.string('RelVal nevts:100'),
    name = cms.untracked.string('Applications')
)

# Output definition

process.TFileService = cms.Service("TFileService",
                                       fileName = cms.string('histos_HTTrigAnalyzer.root')
                                   )

### analyzer configuration

process.htTrigAnalyzerMiniAOD = cms.EDAnalyzer("HTTrigAnalyzerMiniAOD")
process.htTrigAnalyzerMiniAOD.refTriggerName = cms.untracked.string("HLT_PFHT475_v2")
process.htTrigAnalyzerMiniAOD.sigTriggerName = cms.untracked.string("HLT_PFHT800_v2")
process.htTrigAnalyzerMiniAOD.verbose = cms.untracked.bool(False)

process.GlobalTag.globaltag = "80X_dataRun2_Prompt_v8"

# Path and EndPath definitions
process.HLTanalyzers = cms.Path(process.htTrigAnalyzerMiniAOD)
