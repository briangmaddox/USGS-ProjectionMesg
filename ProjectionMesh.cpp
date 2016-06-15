// ProjectionMesh.cpp - extenstion (reworked copy) of the ProjectionMesh class
//
// Orginally Developed by: Mike Childs
// Modified to use mathlib interpolators by Chris Bilderback

#include "ProjectionMesh.h"
#include <math.h>

using namespace PmeshLib;

// ***************************************************************************
//Main constructor for the Projection mesh class 
//which just inits the class data members
ProjectionMesh::ProjectionMesh() throw()
  : interpolator(NULL), interpolator2(NULL), d_left(0.0), d_top(0.0), 
  d_sourceWidth(0.0), d_sourceHeight(0.0),
  d_horizMeshSpacing(0.0), d_vertMeshSpacing(0.0),
  d_meshWidth(0), d_meshHeight(0), d_pNodes(0),
  d_pFromProj(NULL), d_pToProj(NULL)
{
  //setup the default interpolator
  try
  {
    if (!(interpolator = new (std::nothrow) MathLib::DlgViewerInterpolator))
      throw std::bad_alloc();
      
  }
  catch(...)
  {
    delete interpolator;
  }
}

// ***************************************************************************
// Destructor
ProjectionMesh::~ProjectionMesh()
{
  try
  {
    delete[] d_pNodes;
    delete d_pFromProj;
    delete d_pToProj;
    delete interpolator;
    delete interpolator2;
  }
  catch(...)
  {
    //don't do anything since this is the destructor
  }
}


// ***************************************************************************
void ProjectionMesh::setSourceMeshBounds( double left, double bottom,
                                          double right, double top )  throw()
{
  d_left = left;
  d_top = top;
  d_sourceWidth = right - left;
  d_sourceHeight = top - bottom;
  
  // Compute the horizontal mesh spacing
  if ( 0.0 != d_meshWidth )
  {
    d_horizMeshSpacing = d_sourceWidth / ( d_meshWidth - 1 );
  }

  // Computer the vertical mesh spacing
  if ( 0.0 != d_meshHeight )
  {
    d_vertMeshSpacing = d_sourceHeight / ( d_meshHeight - 1 );
  }
}


// ***************************************************************************
void ProjectionMesh::setMeshSize( long width, long height )
  throw (std::bad_alloc)
{
  d_meshWidth = width;
  d_meshHeight = height;
  
  // Make sure the mesh sizes are at least 3
  if ( d_meshWidth < 3 )
  {
    d_meshWidth = 3;
  }

  if ( d_meshHeight < 3 )
  {
    d_meshHeight = 3;
  }

  // Allocate the mesh
  if (d_pNodes)
    delete [] d_pNodes;
  
  if (!(d_pNodes = new(std::nothrow) MeshNode[width * height]))
    throw std::bad_alloc();
  
  // Compute the horizontal mesh spacing
  if ( 0.0 != d_meshWidth )
  {
    d_horizMeshSpacing = d_sourceWidth / ( d_meshWidth - 1 );
  }
  
  // Computer the vertical mesh spacing
  if ( 0.0 != d_meshHeight )
  {
    d_vertMeshSpacing = d_sourceHeight / ( d_meshHeight - 1 );
  }
}


// ***************************************************************************
//Set interpolator function allows you to set the interpolator to use
//with the projection calculation
void ProjectionMesh::setInterpolator(long int in) throw (std::bad_alloc)
{
  //create the Interpolator
  switch(in)
  {
  case MathLib::LeastSquaresPlane:
    //create the interpolators
    delete interpolator;
    delete interpolator2;

    interpolator = NULL;
    interpolator2 = NULL;

    if (!(interpolator = new (std::nothrow) 
          MathLib::LeastSquaresPlaneInterpolator))
      throw std::bad_alloc();
    if (!(interpolator2 = new (std::nothrow) 
          MathLib::LeastSquaresPlaneInterpolator))
      throw std::bad_alloc();
    break;
          
  case MathLib::BiPolynomial:
    delete interpolator;
    delete interpolator2;
    
    interpolator = NULL;
    interpolator2 = NULL;

    if (!(interpolator = new (std::nothrow) 
          MathLib::BiPolynomialInterpolator))
      throw std::bad_alloc();
    if (!(interpolator2 = new (std::nothrow) 
          MathLib::BiPolynomialInterpolator))
      throw std::bad_alloc();
    break;
      
  case MathLib::DlgViewer:
    delete interpolator;
    delete interpolator2;

    interpolator = NULL;
    interpolator2 = NULL;

    if (!(interpolator = new (std::nothrow) 
          MathLib::DlgViewerInterpolator))
      throw std::bad_alloc();
    break;
  case MathLib::BiLinear:
    delete interpolator;
    delete interpolator2;
    
    interpolator = NULL;
    interpolator2 = NULL;

    if (!(interpolator = new (std::nothrow)
          MathLib::BiLinearInterpolator))
      throw std::bad_alloc();
    
    if (!(interpolator2 = new (std::nothrow)
          MathLib::BiLinearInterpolator))
      throw std::bad_alloc();
    break;
  case MathLib::BiCubic:
    delete interpolator;
    delete interpolator2;
    
    interpolator = NULL;
    interpolator2 = NULL;

    if (!(interpolator = new (std::nothrow)
          MathLib::BiCubicInterpolator))
      throw std::bad_alloc();
    
    if (!(interpolator2 = new (std::nothrow)
          MathLib::BiCubicInterpolator))
      throw std::bad_alloc();
    break;
  
  case MathLib::BiCubicSpline:
    delete interpolator;
    delete interpolator2;
    
    interpolator = NULL;
    interpolator2 = NULL;

    if (!(interpolator = new (std::nothrow)
          MathLib::BiCubicSplineInterpolator))
      throw std::bad_alloc();
    
    if (!(interpolator2 = new (std::nothrow)
          MathLib::BiCubicSplineInterpolator))
      throw std::bad_alloc();
    break;
  }
  return; // to stop spurious compiler warnings
}


// ***************************************************************************
// Projection function
bool ProjectionMesh::projectPoint( double& x, double& y )
  const throw(PmeshException)
{
  float col, row;
  long leftCol, rightCol, topRow, bottomRow;
  MathLib::Point * grid = NULL;
  MathLib::Point temp, temp2;
  int counter;
  
  try
  {
    
    // Determine which mesh grid the point is in
    col = ( x - d_left ) / d_horizMeshSpacing;
    row = ( d_top - y  ) / d_vertMeshSpacing;
      
    // Determine which mesh nodes to get
    leftCol   = static_cast<long>( col );
    rightCol  = leftCol + 1;
    topRow    = static_cast<long>( row );
    bottomRow = topRow + 1;
      
    // Make sure the point is in the mesh
    if ( leftCol < 0 || rightCol  > d_meshWidth ||
         topRow  < 0 || bottomRow > d_meshHeight )
      return false;
  
  
    // Make sure we didn't go off the edge with nodes
    if ( rightCol == d_meshWidth )
    {
      rightCol = leftCol;
    }
      
    if ( bottomRow == d_meshHeight )
    {
      bottomRow = topRow;
    }
  
    // Get the needed mesh nodes
    MeshNode* pULNode = getMeshNode( leftCol, topRow );
    MeshNode* pURNode = getMeshNode( rightCol, topRow );
    MeshNode* pLLNode = getMeshNode( leftCol, bottomRow );
    MeshNode* pLRNode = getMeshNode( rightCol, bottomRow );
  
    // Fail if any of the surrounding nodes are invalid
    if ( !pULNode->isValid() || !pURNode->isValid() ||
         !pLLNode->isValid() || !pLRNode->isValid() )
      return false;
      
    switch(interpolator->getInterpolatorType())
    {
    case MathLib::DlgViewer:
      /*Use the viewer's bilinear interpolation to determine the 
        projected point*/
      if (!(grid = new (std::nothrow) MathLib::Point[4]))
        throw std::bad_alloc();
      // Calculate the source coordinates of the node at <col>, <row>
      grid[0].x = d_left + leftCol * d_horizMeshSpacing;
      grid[0].y = d_top  - topRow  * d_vertMeshSpacing;
      grid[1].x = grid[0].x + d_horizMeshSpacing;
      grid[1].y = grid[0].y;
      grid[2].x = grid[0].x + d_horizMeshSpacing;
      grid[2].y = grid[0].y - d_vertMeshSpacing;
      grid[3].x = grid[0].x;
      grid[3].y = grid[0].y - d_vertMeshSpacing;

      // Do a (modified) bilinear interpolation to get the results
      pULNode->getXY(grid[0].z, grid[0].w);
      pURNode->getXY(grid[1].z, grid[1].w);
      pLRNode->getXY(grid[2].z, grid[2].w);
      pLLNode->getXY(grid[3].z, grid[3].w);
                              
      temp.x = x;
      temp.y = y;
      interpolator->setPoints(grid, 4);
      temp = interpolator->interpolatePoint(temp);
      x = temp.z;
      y = temp.w;
      break;

    case MathLib::LeastSquaresPlane:
      /* use the leastSquaresPlane to find the point */
      //setup the grid
      if (!(grid = new (std::nothrow) MathLib::Point[4]))
        throw std::bad_alloc();
      grid[0].x = d_left + leftCol * d_horizMeshSpacing;
      grid[0].y = d_top  - topRow  * d_vertMeshSpacing;
      grid[1].x = grid[0].x + d_horizMeshSpacing;
      grid[1].y = grid[0].y;
      grid[2].x = grid[0].x;
      grid[2].y = grid[0].y - d_vertMeshSpacing;
      grid[3].x = grid[0].x + d_horizMeshSpacing;
      grid[3].y = grid[0].y - d_vertMeshSpacing;
      
      pULNode->getXY(grid[0].z, grid[0].w);
      pURNode->getXY(grid[1].z, grid[1].w);
      pLLNode->getXY(grid[2].z, grid[2].w);
      pLRNode->getXY(grid[3].z, grid[3].w);

      temp.x = x;
      temp.y = y;
      
      interpolator->setPoints(grid, 4);
      temp = interpolator->interpolatePoint(temp);
      
      for(counter = 0; counter < 4; counter++)
      {
        grid[counter].z = grid[counter].w;
      }
          
      temp2.x = x;
      temp2.y = y;
          
      interpolator2->setPoints(grid, 4);
      temp2 = interpolator2->interpolatePoint(temp2);
      
      x = temp.z;
      y = temp2.z;
      break;
          
    case MathLib::BiPolynomial:
      /* use the biPolynomial to find the point */
      //setup the grid
      if (!(grid = new (std::nothrow) MathLib::Point[4]))
        throw std::bad_alloc();
      grid[0].x = d_left + leftCol * d_horizMeshSpacing;
      grid[0].y = d_top  - topRow  * d_vertMeshSpacing;
      grid[1].x = grid[0].x + d_horizMeshSpacing;
      grid[1].y = grid[0].y;
      grid[2].x = grid[0].x;
      grid[2].y = grid[0].y - d_vertMeshSpacing;
      grid[3].x = grid[0].x + d_horizMeshSpacing;
      grid[3].y = grid[0].y - d_vertMeshSpacing;

      pULNode->getXY(grid[0].z, grid[0].w);
      pURNode->getXY(grid[1].z, grid[1].w);
      pLLNode->getXY(grid[2].z, grid[2].w);
      pLRNode->getXY(grid[3].z, grid[3].w);
      
      temp.x = x;
      temp.y = y;
          
      interpolator->setPoints(grid, 4);
      temp = interpolator->interpolatePoint(temp);
      
      for(counter = 0; counter < 4; counter++)
      {
        grid[counter].z = grid[counter].w;
      }
          
      temp2.x = x;
      temp2.y = y;
          
      interpolator2->setPoints(grid, 4);
      temp2 = interpolator2->interpolatePoint(temp2);
          
      x = temp.z;
      y = temp2.z;
      break;
    case MathLib::BiLinear:
      if (!(grid = new (std::nothrow) MathLib::Point[4]))
        throw std::bad_alloc();
      grid[0].x = d_left + leftCol * d_horizMeshSpacing;
      grid[0].y = d_top  - topRow  * d_vertMeshSpacing;
      grid[1].x = grid[0].x + d_horizMeshSpacing;
      grid[1].y = grid[0].y;
      grid[2].x = grid[0].x;
      grid[2].y = grid[0].y - d_vertMeshSpacing;
      grid[3].x = grid[0].x + d_horizMeshSpacing;
      grid[3].y = grid[0].y - d_vertMeshSpacing;
      
      pULNode->getXY(grid[0].z, grid[0].w);
      pURNode->getXY(grid[1].z, grid[1].w);
      pLLNode->getXY(grid[2].z, grid[2].w);
      pLRNode->getXY(grid[3].z, grid[3].w);
      
      temp.x = x;
      temp.y = y;
          
      interpolator->setPoints(grid, 4);
      temp = interpolator->interpolatePoint(temp);
      
      for(counter = 0; counter < 4; counter++)
      {
        grid[counter].z = grid[counter].w;
      }
          
      temp2.x = x;
      temp2.y = y;
          
      interpolator2->setPoints(grid, 4);
      temp2 = interpolator2->interpolatePoint(temp2);
          
      x = temp.z;
      y = temp2.z;
      break;
      
  
    case MathLib::BiCubic: 
      if (!(grid = new (std::nothrow) MathLib::Point[16]))
        throw std::bad_alloc();
       
      //find the nearest sixteen by sixteen grid
      getGrid(leftCol, topRow,grid, 4);
               
      temp.x = x;
      temp.y = y;
      
      interpolator->setPoints(grid, 16);
      temp = interpolator->interpolatePoint(temp);
      
      for(counter = 0; counter < 16; counter++)
      {
        grid[counter].z = grid[counter].w;
      }
          
      temp2.x = x;
      temp2.y = y;
          
      interpolator2->setPoints(grid, 16);
      temp2 = interpolator2->interpolatePoint(temp2);
          
      x = temp.z;
      y = temp2.z;
      break;
    case MathLib::BiCubicSpline:
      if (!(grid = new (std::nothrow) MathLib::Point[16]))
        throw std::bad_alloc();
       
      //find the nearest 9x9 grid
      getGrid(leftCol, topRow,grid, 4);
      
      temp.x = x;
      temp.y = y;
      //dynamic_cast<MathLib::BiCubicSplineInterpolator *>
						//				(interpolator2)->setParabolic();          
      interpolator->setPoints(grid, 16);
      temp = interpolator->interpolatePoint(temp);
      
      for(counter = 0; counter < 4; counter++)
      {
        grid[counter].z = grid[counter].w;
      }
          
      temp2.x = x;
      temp2.y = y;
      //dynamic_cast<MathLib::BiCubicSplineInterpolator *>
						//				(interpolator2)->setParabolic();          
      interpolator2->setPoints(grid, 16);
      temp2 = interpolator2->interpolatePoint(temp2);
          
      x = temp.z;
      y = temp2.z;
      break;


    }
    delete [] grid;
  }
  catch(...)
  { //something went wrong
    delete [] grid;
    return false;
  }
  return true;
}

//**************************************************************************
void ProjectionMesh::getGrid(int Col, int Row, MathLib::Point * in, int size)
const throw(PmeshException)
{
  int counter, counter1;
  int grow, gcol;
  int index =0;
  try
    {
      if ((size > d_meshHeight) || (size > d_meshWidth))
        throw PmeshException();
      grow =0; gcol = 0;
      for (counter = 0; counter < d_meshHeight-size; counter++)
        if (counter+size-1 > Row)
         { 
           grow = counter;
           break;
         }
      for (counter = 0; counter < d_meshWidth-size; counter++)
        if (counter+size-1 > Col)
          {
            gcol = counter;
            break;
          }
      
      //now should be able to get the grid
      for (counter = grow; counter < grow+size; counter++)
        {
          for (counter1 = gcol; counter1 < gcol+size; counter1++)
            {
               getMeshNode( counter1, counter)->getXY
                 (in[index].z, 
                  in[index].w);
               in[index].x =  d_left + counter1 * d_horizMeshSpacing;
               in[index].y = d_top  -  counter * d_vertMeshSpacing;
               index++;
            }
        }

    }
  catch(...)
    {
      throw PmeshException();
    }
}
                 



// ***************************************************************************
void ProjectionMesh::validateNodes() throw()
{
  int row, col;
  double centerX, centerY;
  MeshNode* pTopNode;
  MeshNode* pLeftNode;
  MeshNode* pRightNode;
  MeshNode* pBottomNode;
  MeshNode* pCenterNode;
  
  for ( row = 0; row < d_meshHeight; row++ )
  {
    for ( col = 0; col < d_meshWidth; col++ )
    {
      pCenterNode = getMeshNode( col, row );
	  
      // Don't check this node if it's already been marked invalid
      if ( !pCenterNode->isValid() )
      {
        continue;
      }
	  
      // Handle top row case
      if ( 0 == row )
      {
        pTopNode    = pCenterNode;
        pBottomNode = getMeshNode( col, row + 1 );
      }
      // Handle bottom row case
      else if ( ( d_meshHeight - 1 ) == row )
      {
        pTopNode    = getMeshNode( col, row - 1 );
        pBottomNode = pCenterNode;
      }
      else
      {
        pTopNode    = getMeshNode( col, row - 1 );
        pBottomNode = getMeshNode( col, row + 1 );
      }
      
      // Handle left column case
      if ( 0 == col )
      {
        pLeftNode  = pCenterNode;
        pRightNode = getMeshNode( col + 1, row );
      }
      // Handle right column case
      else if ( ( d_meshWidth - 1 ) == col )
      {
        pLeftNode  = getMeshNode( col - 1, row );
        pRightNode = pCenterNode;
      }
      else
      {
        pLeftNode  = getMeshNode( col - 1, row );
        pRightNode = getMeshNode( col + 1, row );
      }
	  
      // Determine the validity of the node
      pCenterNode->getXY(centerX, centerY);
      
      if ( ( ( pRightNode->getX() - centerX ) *
             ( centerX - pLeftNode->getX() ) < 0.0 ) ||
           ( ( pTopNode->getY() - centerY ) *
             ( centerY - pBottomNode->getY() ) < 0.0 ) )
      {
        pCenterNode->setValid( false );
      }
      else
      {
        pCenterNode->setValid( true );
      }
    }
  }
  
}


// ***************************************************************************
void ProjectionMesh::getProjectedBoundingRect( double& left, double& bottom,
                                               double& right, double& top ) 
  const throw (PmeshException)
{
  double x, y;                 //projected point
  bool   bFirstPoint = true;   //first point
  long   row, col;             //counters
 
  for ( row = 0; row < getMeshHeight(); row++ )
  {
    for ( col = 0; col < getMeshWidth(); col++ )
    {
      // Get the projected point at this position
      if (!getProjectedCoordinate( col, row, x, y ))
      {
        // Try and get the point the slow way
        getSourceCoordinate( col, row, x, y );
        
        if ( !d_pFromProj->projectToGeo( x, y, y, x ) )
        {
          //This should not, to my knowledge happen
          //continue;
          throw PmeshException(PMESH_ERROR_UNKOWN);
        }
	      
        if ( !d_pToProj->projectFromGeo( y, x, x, y ) )
        {
          //continue;
          //again this should not happen
          throw PmeshException(PMESH_ERROR_UNKOWN);
        }
      }
	  
      // Init the bounds if this is the first point
      if ( bFirstPoint )
      {
        left = right = x;
        top = bottom = y;
        bFirstPoint = false;
      }
      else
      {
        left   = ( left < x ) ? left : x;
        right  = ( right > x ) ? right : x;
        top    = ( top > y ) ? top : y;
        bottom = ( bottom < y ) ? bottom : y;
      }
    }
  }
}


// ***************************************************************************
// This function iterates through the source mesh and projects selective
// points in a grid
void ProjectionMesh::calculateMesh( const ProjLib::Projection& sourceProj,
                                    const ProjLib::Projection& destProj )
  throw (PmeshException)
{
  // Set all the points in the mesh grid
  double x, y;
   

  try
  {
    delete d_pFromProj;
    delete d_pToProj;
      
    d_pFromProj = sourceProj.clone();
    d_pToProj = destProj.clone();

    for ( long row = 0; row < d_meshHeight; row++ )
    {
      for ( long col = 0; col < d_meshWidth; col++ )
      {
        // Get the grs point at this position
        getSourceCoordinate( col, row, x, y );
              
        // Convert the coordinate to geographic
        if ( sourceProj.projectToGeo( x, y, y, x ) )
        {
          // Convert from geographic to the destination coordinates
          if ( destProj.projectFromGeo( y, x, x, y ) )
          {
            // Set the projected coordinates in the mesh
            setMeshPoint( col, row, x, y );
          }
        }
        /*the orginal class had no error handling for this
         *and just marked the node as invalid later*/
        else
          {
            //we be screwed so throw
            throw PmeshException(PMESH_ERROR_UNKOWN);
          }
      }
    }	
    // Validate the projection mesh
    validateNodes();
  }
  catch(PmeshException &e)
  {
    throw e; //catch possible out of bounds or not created
  }
}






