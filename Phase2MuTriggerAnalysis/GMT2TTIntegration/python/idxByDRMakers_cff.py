import FWCore.ParameterSet.Config as cms

sim2gmtDRS2Maker = cms.EDProducer(
    "IdxByDRMaker",
    aliasPrefix = cms.string("sims_gmt_drs2"),
    pt1InputTag = cms.InputTag("simtMaker:simspt"),
    eta1InputTag = cms.InputTag("simtMaker:simstfs2eta"),
    phi1InputTag = cms.InputTag("simtMaker:simstfs2phi"),
    pt2InputTag = cms.InputTag("gmtMaker:gmtspt"),
    eta2InputTag = cms.InputTag("gmtMaker:gmtseta"),
    phi2InputTag = cms.InputTag("gmtMaker:gmtsphi"),

    useDr = cms.bool(True),

    drParConst = cms.double(0.5),
    drParPt1In = cms.double(0),
    drParPt2In = cms.double(0)
    )

sim2gmtDEPS2Maker = cms.EDProducer(
    "IdxByDRMaker",
    aliasPrefix = cms.string("sims_gmt_deps2"),
    pt1InputTag = cms.InputTag("simtMaker:simspt"),
    eta1InputTag = cms.InputTag("simtMaker:simstfs2eta"),
    phi1InputTag = cms.InputTag("simtMaker:simstfs2phi"),
    pt2InputTag = cms.InputTag("gmtMaker:gmtspt"),
    eta2InputTag = cms.InputTag("gmtMaker:gmtseta"),
    phi2InputTag = cms.InputTag("gmtMaker:gmtsphi"),

    useDr = cms.bool(False),

    phiParConst = cms.double(0.0504),
    phiParPt1In = cms.double(0.424),
    phiParPt2In = cms.double(0),

    etaParConst = cms.double(0.0432),
    etaParPt1In = cms.double(0.400),
    etaParPt2In = cms.double(0),
)

sim2csctfDRS2Maker = cms.EDProducer(
    "IdxByDRMaker",
    aliasPrefix = cms.string("sims_csctf_drs2"),
    pt1InputTag = cms.InputTag("simtMaker:simspt"),
    eta1InputTag = cms.InputTag("simtMaker:simstfs2eta"),
    phi1InputTag = cms.InputTag("simtMaker:simstfs2phi"),
    pt2InputTag = cms.InputTag("csctfMaker:csctfspt"),
    eta2InputTag = cms.InputTag("csctfMaker:csctfseta"),
    phi2InputTag = cms.InputTag("csctfMaker:csctfsphi"),

    useDr = cms.bool(True),

    drParConst = cms.double(0.5),
    drParPt1In = cms.double(0),
    drParPt2In = cms.double(0)
    )

sim2csctfDEPS2Maker = cms.EDProducer(
    "IdxByDRMaker",
    aliasPrefix = cms.string("sims_csctf_deps2"),
    pt1InputTag = cms.InputTag("simtMaker:simspt"),
    eta1InputTag = cms.InputTag("simtMaker:simstfs2eta"),
    phi1InputTag = cms.InputTag("simtMaker:simstfs2phi"),
    pt2InputTag = cms.InputTag("csctfMaker:csctfspt"),
    eta2InputTag = cms.InputTag("csctfMaker:csctfseta"),
    phi2InputTag = cms.InputTag("csctfMaker:csctfsphi"),

    useDr = cms.bool(False),

    phiParConst = cms.double(0.0504),
    phiParPt1In = cms.double(0.424),
    phiParPt2In = cms.double(0),

    etaParConst = cms.double(0.0432),
    etaParPt1In = cms.double(0.400),
    etaParPt2In = cms.double(0),
)

sim2ttDRMaker = cms.EDProducer(
    "IdxByDRMaker",
    aliasPrefix = cms.string("sims_tt_dr"),
    pt1InputTag = cms.InputTag("simtMaker:simspt"),
    eta1InputTag = cms.InputTag("simtMaker:simseta"),
    phi1InputTag = cms.InputTag("simtMaker:simsphi"),
    pt2InputTag = cms.InputTag("ttMaker:ttspt"),
    eta2InputTag = cms.InputTag("ttMaker:ttseta"),
    phi2InputTag = cms.InputTag("ttMaker:ttsphi"),

    useDr = cms.bool(True),

    drParConst = cms.double(0.03),
    drParPt1In = cms.double(0),
    drParPt2In = cms.double(0)
    )

sim2l1tkmuDRMaker = cms.EDProducer(
    "IdxByDRMaker",
    aliasPrefix = cms.string("sims_l1tkmu_dr"),
    pt1InputTag = cms.InputTag("simtMaker:simspt"),
    eta1InputTag = cms.InputTag("simtMaker:simseta"),
    phi1InputTag = cms.InputTag("simtMaker:simsphi"),
    pt2InputTag = cms.InputTag("l1tkmusCandMaker:l1tkmuspt"),
    eta2InputTag = cms.InputTag("l1tkmusCandMaker:l1tkmuseta"),
    phi2InputTag = cms.InputTag("l1tkmusCandMaker:l1tkmusphi"),

    useDr = cms.bool(True),

    drParConst = cms.double(0.03),
    drParPt1In = cms.double(0),
    drParPt2In = cms.double(0)
    )

sim2l1tkmucsctfDRMaker = cms.EDProducer(
    "IdxByDRMaker",
    aliasPrefix = cms.string("sims_l1tkmucsctf_dr"),
    pt1InputTag = cms.InputTag("simtMaker:simspt"),
    eta1InputTag = cms.InputTag("simtMaker:simseta"),
    phi1InputTag = cms.InputTag("simtMaker:simsphi"),
    pt2InputTag = cms.InputTag("l1tkmucsctfsCandMaker:l1tkmucsctfspt"),
    eta2InputTag = cms.InputTag("l1tkmucsctfsCandMaker:l1tkmucsctfseta"),
    phi2InputTag = cms.InputTag("l1tkmucsctfsCandMaker:l1tkmucsctfsphi"),

    useDr = cms.bool(True),

    drParConst = cms.double(0.03),
    drParPt1In = cms.double(0),
    drParPt2In = cms.double(0)
    )

sim2l1tkmugmtDRMaker = cms.EDProducer(
    "IdxByDRMaker",
    aliasPrefix = cms.string("sims_l1tkmugmt_dr"),
    pt1InputTag = cms.InputTag("simtMaker:simspt"),
    eta1InputTag = cms.InputTag("simtMaker:simseta"),
    phi1InputTag = cms.InputTag("simtMaker:simsphi"),
    pt2InputTag = cms.InputTag("l1tkmugmtsCandMaker:l1tkmugmtspt"),
    eta2InputTag = cms.InputTag("l1tkmugmtsCandMaker:l1tkmugmtseta"),
    phi2InputTag = cms.InputTag("l1tkmugmtsCandMaker:l1tkmugmtsphi"),

    useDr = cms.bool(True),

    drParConst = cms.double(0.03),
    drParPt1In = cms.double(0),
    drParPt2In = cms.double(0)
    )

gmt2csctfDRS2Maker = cms.EDProducer(
    "IdxByDRMaker",
    aliasPrefix = cms.string("gmts_csctf_dr"),
    pt1InputTag = cms.InputTag("gmtMaker:gmtspt"),
    eta1InputTag = cms.InputTag("gmtMaker:gmtseta"),
    phi1InputTag = cms.InputTag("gmtMaker:gmtsphi"),
    pt2InputTag = cms.InputTag("csctfMaker:csctfspt"),
    eta2InputTag = cms.InputTag("csctfMaker:csctfseta"),
    phi2InputTag = cms.InputTag("csctfMaker:csctfsphi"),

    useDr = cms.bool(True),

    drParConst = cms.double(0.01),
    drParPt1In = cms.double(0),
    drParPt2In = cms.double(0)
    )

csctf2gmtDRS2Maker = cms.EDProducer(
    "IdxByDRMaker",
    aliasPrefix = cms.string("csctfs_gmt_dr"),
    pt1InputTag = cms.InputTag("csctfMaker:csctfspt"),
    eta1InputTag = cms.InputTag("csctfMaker:csctfseta"),
    phi1InputTag = cms.InputTag("csctfMaker:csctfsphi"),
    pt2InputTag = cms.InputTag("gmtMaker:gmtspt"),
    eta2InputTag = cms.InputTag("gmtMaker:gmtseta"),
    phi2InputTag = cms.InputTag("gmtMaker:gmtsphi"),

    useDr = cms.bool(True),

    drParConst = cms.double(0.01),
    drParPt1In = cms.double(0),
    drParPt2In = cms.double(0)
    )


tt2gmtDRS2Maker = cms.EDProducer(
    "IdxByDRMaker",
    aliasPrefix = cms.string("tts_gmt_drs2"),
    pt1InputTag = cms.InputTag("ttMaker:ttspt"),
    eta1InputTag = cms.InputTag("ttMaker:ttstfs2eta"),
    phi1InputTag = cms.InputTag("ttMaker:ttstfs2phi"),
    pt2InputTag = cms.InputTag("gmtMaker:gmtspt"),
    eta2InputTag = cms.InputTag("gmtMaker:gmtseta"),
    phi2InputTag = cms.InputTag("gmtMaker:gmtsphi"),

    useDr = cms.bool(True),

    drParConst = cms.double(0.5),
    drParPt1In = cms.double(0),
    drParPt2In = cms.double(0)
    )

tt2gmtDEPS2Maker = cms.EDProducer(
    "IdxByDRMaker",
    aliasPrefix = cms.string("tts_gmt_deps2"),
    pt1InputTag = cms.InputTag("ttMaker:ttspt"),
    eta1InputTag = cms.InputTag("ttMaker:ttstfs2eta"),
    phi1InputTag = cms.InputTag("ttMaker:ttstfs2phi"),
    pt2InputTag = cms.InputTag("gmtMaker:gmtspt"),
    eta2InputTag = cms.InputTag("gmtMaker:gmtseta"),
    phi2InputTag = cms.InputTag("gmtMaker:gmtsphi"),

    useDr = cms.bool(False),

    phiParConst = cms.double(0.0504),
    phiParPt1In = cms.double(0.424),
    phiParPt2In = cms.double(0),

    etaParConst = cms.double(0.0432),
    etaParPt1In = cms.double(0.400),
    etaParPt2In = cms.double(0),
)

tt2csctfDRS2Maker = cms.EDProducer(
    "IdxByDRMaker",
    aliasPrefix = cms.string("tts_csctf_drs2"),
    pt1InputTag = cms.InputTag("ttMaker:ttspt"),
    eta1InputTag = cms.InputTag("ttMaker:ttstfs2eta"),
    phi1InputTag = cms.InputTag("ttMaker:ttstfs2phi"),
    pt2InputTag = cms.InputTag("csctfMaker:csctfspt"),
    eta2InputTag = cms.InputTag("csctfMaker:csctfseta"),
    phi2InputTag = cms.InputTag("csctfMaker:csctfsphi"),

    useDr = cms.bool(True),

    drParConst = cms.double(0.5),
    drParPt1In = cms.double(0),
    drParPt2In = cms.double(0)
    )

tt2csctfDEPS2Maker = cms.EDProducer(
    "IdxByDRMaker",
    aliasPrefix = cms.string("tts_csctf_deps2"),
    pt1InputTag = cms.InputTag("ttMaker:ttspt"),
    eta1InputTag = cms.InputTag("ttMaker:ttstfs2eta"),
    phi1InputTag = cms.InputTag("ttMaker:ttstfs2phi"),
    pt2InputTag = cms.InputTag("csctfMaker:csctfspt"),
    eta2InputTag = cms.InputTag("csctfMaker:csctfseta"),
    phi2InputTag = cms.InputTag("csctfMaker:csctfsphi"),

    useDr = cms.bool(False),

    phiParConst = cms.double(0.0504),
    phiParPt1In = cms.double(0.424),
    phiParPt2In = cms.double(0),

    etaParConst = cms.double(0.0432),
    etaParPt1In = cms.double(0.400),
    etaParPt2In = cms.double(0),
)


gmt2ttDRS2Maker = cms.EDProducer(
    "IdxByDRMaker",
    aliasPrefix = cms.string("gmts_tt_drs2"),
    pt1InputTag = cms.InputTag("gmtMaker:gmtspt"),
    eta1InputTag = cms.InputTag("gmtMaker:gmtseta"),
    phi1InputTag = cms.InputTag("gmtMaker:gmtsphi"),
    pt2InputTag = cms.InputTag("ttMaker:ttspt"),
    eta2InputTag = cms.InputTag("ttMaker:ttstfs2eta"),
    phi2InputTag = cms.InputTag("ttMaker:ttstfs2phi"),

    useDr = cms.bool(True),

    drParConst = cms.double(0.5),
    drParPt1In = cms.double(0),
    drParPt2In = cms.double(0)
    )

gmt2ttDEPS2Maker = cms.EDProducer(
    "IdxByDRMaker",
    aliasPrefix = cms.string("gmts_tt_deps2"),
    pt1InputTag = cms.InputTag("gmtMaker:gmtspt"),
    eta1InputTag = cms.InputTag("gmtMaker:gmtseta"),
    phi1InputTag = cms.InputTag("gmtMaker:gmtsphi"),
    pt2InputTag = cms.InputTag("ttMaker:ttspt"),
    eta2InputTag = cms.InputTag("ttMaker:ttstfs2eta"),
    phi2InputTag = cms.InputTag("ttMaker:ttstfs2phi"),

    useDr = cms.bool(False),

    phiParConst = cms.double(0.0504),
    phiParPt1In = cms.double(0),
    phiParPt2In = cms.double(0.424),

    etaParConst = cms.double(0.0432),
    etaParPt1In = cms.double(0),
    etaParPt2In = cms.double(0.400),
)

csctf2ttDRS2Maker = cms.EDProducer(
    "IdxByDRMaker",
    aliasPrefix = cms.string("csctfs_tt_drs2"),
    pt1InputTag = cms.InputTag("csctfMaker:csctfspt"),
    eta1InputTag = cms.InputTag("csctfMaker:csctfseta"),
    phi1InputTag = cms.InputTag("csctfMaker:csctfsphi"),
    pt2InputTag = cms.InputTag("ttMaker:ttspt"),
    eta2InputTag = cms.InputTag("ttMaker:ttstfs2eta"),
    phi2InputTag = cms.InputTag("ttMaker:ttstfs2phi"),

    useDr = cms.bool(True),

    drParConst = cms.double(0.5),
    drParPt1In = cms.double(0),
    drParPt2In = cms.double(0)
    )

csctf2ttDEPS2Maker = cms.EDProducer(
    "IdxByDRMaker",
    aliasPrefix = cms.string("csctfs_tt_deps2"),
    pt1InputTag = cms.InputTag("csctfMaker:csctfspt"),
    eta1InputTag = cms.InputTag("csctfMaker:csctfseta"),
    phi1InputTag = cms.InputTag("csctfMaker:csctfsphi"),
    pt2InputTag = cms.InputTag("ttMaker:ttspt"),
    eta2InputTag = cms.InputTag("ttMaker:ttstfs2eta"),
    phi2InputTag = cms.InputTag("ttMaker:ttstfs2phi"),

    useDr = cms.bool(False),

    phiParConst = cms.double(0.0504),
    phiParPt1In = cms.double(0),
    phiParPt2In = cms.double(0.424),

    etaParConst = cms.double(0.0432),
    etaParPt1In = cms.double(0),
    etaParPt2In = cms.double(0.400),
)

idxByDRMakers = cms.Sequence(
    sim2gmtDRS2Maker
    + sim2gmtDEPS2Maker
    + sim2csctfDRS2Maker
    + sim2csctfDEPS2Maker
    + sim2ttDRMaker
    + sim2l1tkmuDRMaker
    + sim2l1tkmucsctfDRMaker
    + sim2l1tkmugmtDRMaker
    + gmt2csctfDRS2Maker
    + csctf2gmtDRS2Maker
    + tt2gmtDRS2Maker
    + tt2gmtDEPS2Maker
    + tt2csctfDRS2Maker
    + tt2csctfDEPS2Maker
    + gmt2ttDRS2Maker
    + gmt2ttDEPS2Maker
    + csctf2ttDRS2Maker
    + csctf2ttDEPS2Maker
    )
