// $Id: ProjectionMesh.h,v 1.1.1.1 2002-02-20 17:09:49 cbilderback Exp $
// Last modified by $Author: cbilderback $ on $Date: 2002-02-20 17:09:49 $
// Originally by Christopher Bilderback

/*ProjectionMesh.h  Basically a extention on the DLGV32's projection mesh
  which was written by Mike Childs
  Extention includes the ability to select your own interpolator along with
  a little bit different structure and (hopefully) exception handling*/


#ifndef _PROJECTIONMESH_H_
#define _PROJECTIONMESH_H_

#include "ProjectionLib/Projection.h" //include projection headers here
#include "MathLib/DlgViewerInterpolator.h"
#include "MathLib/LeastSquaresPlaneInterpolator.h"
#include "MathLib/BiPolynomialInterpolator.h"
#include "MathLib/BiLinearInterpolator.h"
#include "MathLib/BiCubicInterpolator.h"
#include "MathLib/BiCubicSplineInterpolator.h"
#include "PmeshException.h"
#include "MeshNode.h"

namespace PmeshLib    //namespace
{

class ProjectionMesh
{
 public:
  /* Main constructor for Projection Mesh class */
  ProjectionMesh() throw();
  
  /* Destruction */
  ~ProjectionMesh();

  /* This function projects a Point from the source mesh coordinate space
     into the target mesh (set in calculate mesh) using the 
     interpolator specified in setInterpolator() If no interpolator is set 
     then the original bilinear interpolation from the veiwer is used.*/ 
  bool projectPoint( double& x, double& y ) const throw(PmeshException);
  
  
  /* Projects each source coordinate in the mesh from <sourceProj> to
     <destProj> and validates all the nodes when it's done */ 
  void calculateMesh( const ProjLib::Projection& sourceProj, 
		      const ProjLib::Projection& destProj )  
    throw(PmeshException);
    
  /* This function sets the bounding rectangle for the source mesh*/
  void setSourceMeshBounds( double left, double bottom,
                            double right, double top ) throw();

  /* Set the number of points to place in the mesh */
  void setMeshSize( long width, long height ) throw(std::bad_alloc);
 
  /* Set interpolator function sets the interpolator for use *
   * with projection calculation                             */ 
  void setInterpolator(long int in)  throw(std::bad_alloc);
  
  /* Get the bounding value from the source mesh */
  void getSourceMesh(double & left, double & bottom,
		     double & right, double & top) const throw();
  
  /* Get the width and height */
  void getMeshSize(long &width, long & height) const throw();
 
  /* Get just the width */
  long getMeshWidth() const throw();
  
  /* Get just the height */
  long getMeshHeight() const throw();

  /* Gets the source mesh coordinate for the *
   * grid coordinate <col>, <row>            */ 
  void getSourceCoordinate( long col, long row, double& x, double& y ) 
    const throw();
  
  /*Gets the projected coordinate for the grid coordinate <col>, <row>.
    Returns false if the projected coordinate is invalid*/ 
  bool getProjectedCoordinate( long col, long row, double& x, double& y ) 
    const throw(PmeshException);
 
  /*Iterates through every valid point in the grid and determines the
    minimum bounding rectangle of the projected coordinates*/
  void getProjectedBoundingRect( double& left, double& bottom,
                                 double& right, double& top ) 
    const throw(PmeshException);

  

 private:    
  
  /* Helper functions */
  MeshNode* getMeshNode( long col, long row ) const throw(PmeshException);
  
  /* This gets a sizexsize grid */
  void ProjectionMesh::getGrid(int Col, int Row, MathLib::Point * in, int size)
   const throw(PmeshException);
  
  /*Determines the validity of each node in the mesh.  This should be
    called after setMeshPoint has been called for each point in the mesh*/
  void validateNodes() throw();
  
  /* Set a particular projected point in the mesh */
  void setMeshPoint( long col, long row,
                     double projectedX, double projectedY )
    throw(PmeshException);

  /* Data members */
  MathLib::Interpolator *interpolator;  //the interpolators to use.
                                        //by default this is the 
                                        //dlgveiwer interpolator
  MathLib::Interpolator *interpolator2; //some need extra interpolator
  double    d_left, d_top;
  double    d_sourceWidth, d_sourceHeight;
  double    d_horizMeshSpacing, d_vertMeshSpacing;
  long      d_meshWidth, d_meshHeight;
  MeshNode* d_pNodes;
  ProjLib::Projection* d_pFromProj;
  ProjLib::Projection* d_pToProj;
};


// ***************************************************************************
//Sets a single point in the projection mesh
inline
void ProjectionMesh::setMeshPoint( long col, long row,
                                   double projectedX, double projectedY )
     throw(PmeshException)
{
  int tempindex;    //temporary index
  //check for the existance of the d_pNodes
  if (!d_pNodes)
    throw PmeshException(PMESH_NOT_CREATED_YET);
  
  //now check to see if the pmesh is in the bounding array
  tempindex = row * d_meshWidth + col;

  if ((tempindex < 0) || (tempindex > (d_meshWidth * d_meshHeight)))
    throw PmeshException(PMESH_OUT_OF_BOUNDS);
	  
  d_pNodes[tempindex].setXY( projectedX, projectedY );
  d_pNodes[tempindex].setValid( true );
}


// ***************************************************************************
//Gets the extents of the Mesh in the source projection space
inline
void ProjectionMesh::getSourceMesh(double & left, double & bottom,
                                   double & right, double & top) const throw()
{
  left = d_left;
  right = d_left + d_sourceWidth;
  bottom = d_top - d_sourceHeight;
  top = d_top;
}
 
// ***************************************************************************
//Gets how many projected points are in our mesh
inline
void ProjectionMesh::getMeshSize(long &width, long & height) const throw()
{
  width = d_meshWidth;
  height = d_meshHeight;
}


// ***************************************************************************
// Get just the width
inline
long ProjectionMesh::getMeshWidth() const throw()
{
  return d_meshWidth;
}

  
// ***************************************************************************
// Get just the height
inline
long ProjectionMesh::getMeshHeight() const throw()
{
  return d_meshHeight;
}

// ***************************************************************************
//Gets a coordinate in the source projected space
inline
void ProjectionMesh::getSourceCoordinate( long col, long row,
                                          double& x, double& y ) const
     throw()
{ 
  x = d_left + col * d_horizMeshSpacing;
  y = d_top  - row * d_vertMeshSpacing;
}

// ***************************************************************************
//Gets a coordinate in the Projected coordinate space
inline
bool ProjectionMesh::getProjectedCoordinate( long col, long row,
                                             double& x, double& y ) const
     throw(PmeshException)
{
  int tempindex;    //temporary index
  //check on the existance of d_pNodes
  if (!d_pNodes)
    throw PmeshException(PMESH_NOT_CREATED_YET);
  
  //check the validity of the index
  tempindex = row * d_meshWidth + col;
  
  if ((tempindex < 0) || (tempindex > d_meshWidth*d_meshHeight))
    throw PmeshException(PMESH_OUT_OF_BOUNDS);
  
  //proceed
  MeshNode node = d_pNodes[ row * d_meshWidth + col ];
  node.getXY(x, y);
  
  if (!node.isValid())
    return false;
  return true;
}


// ***************************************************************************
//Gets a node from the mesh by row or col
inline
MeshNode* ProjectionMesh::getMeshNode( long col, long row ) const
     throw (PmeshException)
{
  int tempindex;    //temporary index
  
  //check for the existance of the d_pNodes
  if (!d_pNodes)
    throw PmeshException(PMESH_NOT_CREATED_YET);
  
  tempindex = row * d_meshWidth + col;
  
  if ((tempindex < 0) || (tempindex > d_meshWidth*d_meshHeight))
    throw PmeshException(PMESH_OUT_OF_BOUNDS);
  //proceed
  return &d_pNodes[ row * d_meshWidth + col ];
}


}//namespace
#endif

