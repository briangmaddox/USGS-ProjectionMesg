/* Exception class for the projection mesh class */


#ifndef PMESHEXECPTION_H
#define PMESHEXECPTION_H


#include <exception>
#include <string>
#include <new>

namespace PmeshLib    //use namespace to identify errors
{
//The execptions thrown by Projection Mesh
#define PMESH_OUT_OF_BOUNDS   0
#define PMESH_NOT_CREATED_YET 1
#define PMESH_ERROR_UNKOWN    255

class PmeshException
{
 public:
  //Constructors and Destructors
  PmeshException(short int inexception)             throw();
  PmeshException()                                  throw();
  virtual ~PmeshException();
  
  //member functions
  bool setException(short int inexception)          throw();
  void getException(short int& inexception)   const throw();
  void getString(std::string& instring)       const throw();
  
 protected:
  short int exception;    //the exception thrown
};

} //namespace
#endif

