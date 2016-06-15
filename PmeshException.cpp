

#ifndef PMESH_EXCEPTION_CC
#define PMESH_EXCEPTION_CC

#include "PmeshException.h"


using namespace PmeshLib;


PmeshException::PmeshException() throw()
  : exception(PMESH_ERROR_UNKOWN)
{
}

PmeshException::~PmeshException()
{
}

PmeshException::PmeshException(short int inexception) throw()
  : exception(PMESH_ERROR_UNKOWN)
{
}

bool PmeshException::setException(short int inexception) throw()
{
  exception = inexception;
  return true;
}

//Returns the interger form of the error
void PmeshException::getException(short int &inexception) const throw()
{
  inexception = exception;
}

//Returns the string form of the error
void PmeshException::getString(std::string &instring) const throw()
{
  switch (exception)//Get the appropriate error message and return it
    {
    case PMESH_OUT_OF_BOUNDS:
      instring = "PMESH: Out of Bounds";
      break;
    case PMESH_NOT_CREATED_YET:
      instring = "PMESH: Not created yet";
      break;
    default:
      instring = "PMESH: Unkown error";
    }
}

#endif
