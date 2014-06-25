/**
 * \file MCBaseException.hh
 *
 * \ingroup MCBase
 * 
 * \brief Class def header for exception classes for MC data products
 *
 * @author kazuhiro
 */

/** \addtogroup MCBase

    @{*/
#ifndef MCBASEEXCEPTION_H
#define MCBASEEXCEPTION_H

#include <string>
#include <exception>

namespace sim {
  /**
     \class MCBaseException
     Simple exception class for MCBase package
  */
  class MCBaseException : public std::exception{

  public:

    MCBaseException(std::string msg="") : std::exception(), _msg(msg)
    {}

    virtual ~MCBaseException() throw(){}
    virtual const char* what() const throw() 
    {return _msg.c_str(); }

  private:

    std::string _msg;
  };

}
#endif
/** @} */ // end of doxygen group 

