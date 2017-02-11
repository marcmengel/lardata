#include "KFTrackState.h"

using namespace trkf;

bool KFTrackState::updateWithHitState(const HitState& hitstate) {
  //if track and hit not on same plane do not update and return false
  if ( (hitstate.plane().position()-fTrackState.plane().position()).Mag2()>10e-6   ) return false;
  if ( (hitstate.plane().direction()-fTrackState.plane().direction()).Mag2()>10e-6 ) return false;
  SMatrixSym55 tmp;
  tmp(0,0) = 1./(hitstate.hitMeasErr2()+fTrackState.covariance()(0,0));
  fTrackState.setParameters( fTrackState.parameters() + fTrackState.covariance()*tmp.Col(0)*(hitstate.hitMeas() - fTrackState.parameters()(0)) );
  fTrackState.setCovariance( (fTrackState.covariance()-ROOT::Math::Similarity(fTrackState.covariance(),tmp)) );
  /*
  SVector6 par6d = fTrackState.plane().Local5DToGlobal6DParameters(fTrackState.parameters(),fTrackState.isTrackAlongPlaneDir());
  fTrackState.setPosition( Point_t(par6d[0],par6d[1],par6d[2]) );
  fTrackState.setMomentum( Vector_t(par6d[3],par6d[4],par6d[5]) );
  */
  return true;
}

bool KFTrackState::combineWithTrackState(const TrackState& trackstate) {
  //if tracks not on same plane do not update and return false
  if ( (trackstate.plane().position()-fTrackState.plane().position()).Mag2()>10e-6   ) return false;
  if ( (trackstate.plane().direction()-fTrackState.plane().direction()).Mag2()>10e-6 ) return false;
  const SVector5& par1 = fTrackState.parameters();
  const SVector5& par2 = trackstate.parameters();
  const SMatrixSym55& cov1 = fTrackState.covariance();
  const SMatrixSym55& cov2 = trackstate.covariance();
  SMatrixSym55&& cov = cov1 + cov2;
  bool success = cov.Invert();
  if (!success) return false;
  const auto K = cov1 * cov;
  fTrackState.setParameters( par1 + K*(par2 - par1) );
  fTrackState.setCovariance( SMatrixSym55(SMatrix55(K*cov2).LowerBlock()) );
  return true;
}

/*

SVector5 KFTrackState::updatedTrackStatePar(double slopeVar) const {
  SVector5 he;
  //std::cout << "hitMeasErr2=" << hitMeasErr2() << " trackStateCov=" << trackStateCov().At(comp_,comp_) << " slopevar=" << slopeVar << std::endl;
  he.At(comp_) = 1./(hitMeasErr2()+trackStateCov().At(comp_,comp_)+slopeVar);
  // std::cout << "temp=" << he << std::endl;
  // std::cout << "terr\n=" << trackStateCov() << std::endl;
  // std::cout << "fRvec=" << (hitMeas() - trackStatePar()[comp_]) << " gain=" << trackStateCov()*he << std::endl;
  return SVector5(trackStatePar() + trackStateCov()*he*(hitMeas() - trackStatePar()[comp_]));
}

SMatrixSym55 KFTrackState::updatedTrackStateCov(double slopeVar) const {
  //slower version for backward compatibility (note that the use of slopeVar is inconsistent, this is to match numerical results from before)
  SMatrix51 he;he.At(comp_,comp_) = 1./(hitMeasErr2()+trackStateCov().At(comp_,comp_)+slopeVar);
  SMatrix51 K = trackStateCov()*he;
  SMatrixSym11 V; V.At(comp_,comp_) = hitMeasErr2();//+slopeVar
  SMatrix55 KH;KH.Place_at(K,comp_,comp_);
  SMatrix55 I = ROOT::Math::SMatrixIdentity();
  //std::cout << "Result1=\n" << ROOT::Math::Similarity(I-KH,fPredTrackStateCov)+ROOT::Math::Similarity(K,V) << std::endl;
  return ROOT::Math::Similarity(I-KH,trackStateCov())+ROOT::Math::Similarity(K,V);
  //this is the right version
  // SMatrixSym55 tmp;
  // tmp.At(comp_,comp_) = 1./(hitMeasErr2()+trackStateCov().At(comp_,comp_));
  // return (trackStateCov()-ROOT::Math::Similarity(trackStateCov(),tmp));
}
*/
