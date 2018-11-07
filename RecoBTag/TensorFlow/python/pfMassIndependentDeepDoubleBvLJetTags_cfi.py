import FWCore.ParameterSet.Config as cms

pfMassIndependentDeepDoubleBvLJetTags = cms.EDProducer('DeepDoubleXTFJetTagsProducer',
  src = cms.InputTag('pfDeepDoubleXTagInfos'),
  input_names = cms.vstring(
    'input_1',
    'input_2',
    'input_3'
  ),
  graph_path = cms.FileInPath('RecoBTag/Combined/data/DeepDoubleX/94X/V01/DDB_mass_independent.pb'),
  lp_names = cms.vstring('db_input_batchnorm/keras_learning_phase'),
  output_names = cms.vstring('ID_pred/Softmax'),
  flavor = cms.string('BvL'),
  flav_table = cms.PSet(
    probQCD = cms.vuint32(0),
    probHbb = cms.vuint32(1)
  ),
  batch_eval = cms.bool(False),
  nThreads = cms.uint32(1),
  singleThreadPool = cms.string('no_threads')
)
