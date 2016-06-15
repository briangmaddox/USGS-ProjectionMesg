// $Id: MeshNode.h,v 1.1.1.1 2002-02-20 17:09:49 cbilderback Exp $
// Last modified by $Author: cbilderback $ on $Date: 2002-02-20 17:09:49 $
// Originally by Christopher Bilderback

// This is the MeshNode class that the Projection Mesh library needs.  It has
// been extracted into it's own header and implementation files.

#ifndef _MESHNODE_H_
#define _MESHNODE_H_

namespace PmeshLib
{

// MeshNode class models a single projected point
class MeshNode
{
 public:
  // Main constructor for Mesh
  MeshNode(double x = 0, double y = 0) throw();
	
  // Modifiers
  void setValid( bool bValid )     throw();
  void setXY( double x, double y ) throw();
    
  // Get functions
  void   getXY(double &x, double &y)  const throw();
  double getX()                       const throw();
  double getY()                       const throw();
  
  // Data validation 
  bool   isValid() const throw();
  
 protected:
  double d_x, d_y; // Projected coordinates
  bool   d_bValid;
};
 

// ***************************************************************************
inline
double MeshNode::getX() const throw()
{
  return d_x;
}

// ***************************************************************************
inline
double MeshNode::getY()const throw()
{
  return d_y;
}

// ***************************************************************************
inline
void MeshNode::getXY(double &x, double &y) const throw()
{
  //give the point
  x = d_x;
  y = d_y;
}

// ***************************************************************************
inline
bool MeshNode::isValid() const throw()
{
  return d_bValid;
}

// ***************************************************************************
inline
void MeshNode::setValid( bool bValid ) throw()
{
  d_bValid = bValid;
}

// ***************************************************************************
inline
void MeshNode::setXY( double x, double y ) throw()
{
  d_x = x;
  d_y = y;
}


} // namespace
 
#endif
