// $Id: MeshNode.cpp,v 1.1.1.1 2002-02-20 17:09:49 cbilderback Exp $
// Last modified by $Author: cbilderback $ on $Date: 2002-02-20 17:09:49 $
// Originally by Christopher Bilderback

// Implementation of the MeshNode class

#include "MeshNode.h"

using namespace PmeshLib;


// ***************************************************************************
MeshNode::MeshNode(double x, double y) throw()
  : d_x(x), d_y(y), d_bValid(false)
{
}

