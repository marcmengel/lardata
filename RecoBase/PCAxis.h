////////////////////////////////////////////////////////////////////////////
// \version $Id: PCAxis.h,v 1.0 2010/02/15 20:32:46 usher Exp $
//
// \brief Definition of a Principal Component Axis object for LArSoft
//
// \author usher@slac.stanford.edu
//
////////////////////////////////////////////////////////////////////////////

#ifndef PCAxis_H
#define PCAxis_H

#include <vector>

#ifndef __GCCXML__
#include <iosfwd>
#endif

namespace recob
{

class PCAxis
{
public:
    
    typedef std::vector<std::vector<double> > EigenVectors;
    
    PCAxis();
    
private:
    
    bool         fSvdOK;            /// SVD Decomposition was successful
    int          fNumHitsUsed;      /// Number of hits in the decomposition
    double       fEigenValues[3];   /// Eigen values from SVD decomposition
    EigenVectors fEigenVectors;     /// The three principle axes
    double       fAvePosition[3];   /// Average position of hits fed to PCA
    double       fAveHitDoca;       /// Average doca of hits used in PCA
    size_t       fID;               ///< axis ID
    
#ifndef __GCCXML__
public:
    
    PCAxis(bool ok, int nHits, const double* eigenValues, const EigenVectors& eigenVecs, const double* avePos, const double aveHitDoca = 9999., size_t id=0);
    
    bool                getSvdOK()        const;
    int                 getNumHitsUsed()  const;
    const double*       getEigenValues()  const;
    const EigenVectors& getEigenVectors() const;
    const double*       getAvePosition()  const;
    const double        getAveHitDoca()   const;
    size_t              getID()           const;
    
    friend std::ostream&  operator << (std::ostream & o, const PCAxis& a);
    friend bool operator < (const PCAxis& a, const PCAxis& b);
    
#endif
};
    
}

#ifndef __GCCXML__

inline bool                               recob::PCAxis::getSvdOK()        const {return fSvdOK;}
inline int                                recob::PCAxis::getNumHitsUsed()  const {return fNumHitsUsed;}
inline const double*                      recob::PCAxis::getEigenValues()  const {return fEigenValues;}
inline const recob::PCAxis::EigenVectors& recob::PCAxis::getEigenVectors() const {return fEigenVectors;}
inline const double*                      recob::PCAxis::getAvePosition()  const {return fAvePosition;}
inline const double                       recob::PCAxis::getAveHitDoca()   const {return fAveHitDoca;}
inline size_t                             recob::PCAxis::getID()           const {return fID;}


#endif

#endif // PCAxis_H
