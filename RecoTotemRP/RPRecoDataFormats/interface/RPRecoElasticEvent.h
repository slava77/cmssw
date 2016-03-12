/****************************************************************************
*
* This is a part of the TOTEM offline software.
* Authors: 
*	Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#ifndef RecoTotemRP_RPRecoDataFormats_RPRecoElasticEvent_h
#define RecoTotemRP_RPRecoDataFormats_RPRecoElasticEvent_h

#include <ostream>
#include <vector>


///\defgroup ElasticReconstruction
/**
 * \ingroup ElasticReconstruction
 * \brief Result of reconstruction of elastic events.
 *
 * Contains the full report of reconstruction process. It is important to check isValid flag.
 **/
class RPRecoElasticEvent
{
	public:
		///\brief structure used in hit selection (step 1)
		struct road_type {
			double sumx, sumy;
            double minx, maxx, miny, maxy;
			std::vector<unsigned short> members;

			road_type() : sumx(0.), sumy(0.), minx(+1E10), maxx(-1E10), miny(+1E10), maxy(-1E10) {}

			double centerX() const { return (members.size()) ? sumx / members.size() : 0.; }
			double centerY() const { return (members.size()) ? sumy / members.size() : 0.; }

            double SizeX() const { return maxx - minx; }
            double SizeY() const { return maxy - miny; }
		};

		///\brief structure holding track fit results
		struct fit_type {
			double th_x, th_y, x, y;				///< IP parameters (angles in rad, vertex position in m)
			double si_th_x, si_th_y, si_x, si_y;	///< parameter errors
			double s2min_x, s2min_y;				///< residual sum of squares
			short ndf_x, ndf_y;						///< numbers of degrees of freedom

			double s2minPerDf_x() const { return (ndf_x == 0) ? 0. : s2min_x / ndf_x; }
			double s2minPerDf_y() const { return (ndf_y == 0) ? 0. : s2min_y / ndf_y; }

			fit_type() { th_x = th_y = x = y = si_th_x = si_th_y = si_x = si_y = s2min_x = s2min_y = 0.; ndf_x = ndf_y = 0; }
		};

		std::vector<road_type> roads;
		signed int preferredRoad;
		fit_type leftFit, rightFit, globalFit, result;

		/// status of the event
		enum status_type {sDefault, sOK, sNoRoad, sNoGoodRoad, sRejected} status;

		/// rejection reason (if status = sRejected)
		enum reject_type {rNone = 0, rVertexX = 1, rVertexY = 2, rAngleX = 4, rAngleY = 8};
		unsigned int rejectReason;

		/// returns true if the event is fully reconstructed
		bool isValid() const { return (status == sOK); }

		RPRecoElasticEvent() : preferredRoad(-1), status(sDefault), rejectReason(rNone) {}
		virtual ~RPRecoElasticEvent() {}
};



std::ostream &operator << (std::ostream &out, const RPRecoElasticEvent &prot);

#endif
