/*!
This file defines the IK solver class.
*/

#ifndef _IK_HPP_
#define _IK_HPP_

#include <cmath>
#include <vector>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif
#include <Eigen/Dense>

using namespace Eigen;

//! This is the class for a IK node
class ik_node_t
{
public:
  //!Degrees of freedom of this node
  int dof;

  //!Values of the degrees of freedom
  VectorXf theta;

  //!Transformation from world to node
  Affine3f T;

  //!Transformation from node to world
  Affine3f invT;

  //!Absolute position of the node in world coordinates
  Vector3f abs_pos;

  //!Default Constructor
  ik_node_t(int _dof, const VectorXf &_theta)
  {
    dof=_dof;
    theta=_theta;
  }
};


//! This solves IK for a single kinematic chain
//! Even though the class can support upto 3 rotational DoFs for each joint, we consider
//! that the root has 3 DoF (ball and socket joint) and all other joints are hinge joints with 1 DoF
//! The link is aligned to the local Y-axis for each node, and the hinge rotates about the +ve Z axis.
//! Lenght of all the link is considered to be the same, though this is easy to change.
class ik_t
{
public:
  
  //! All bones in the chain are of this fixed length
  float bone_length;

  //! Total number of bones in the chain (equal to number of nodes)
  int num_bones;

  //! Total degrees of freedom for the chain
  int num_dofs;
  
  //! List of all nodes in increasing order of hierarchy
  std::vector<ik_node_t> node_list;

  //! Position of the end-effector
  Vector3f end_effector;
  
  //! Root translation - do not use this
  Vector3f root;

  //! Goal or target for the IK solver
  Vector3f target;
  
  //! Recalculate all joint transformations and absolute positions from 
  //! current chain configuration
  void update_all_nodes(void);
   
  //! Return the end-effector position
  const Vector3f& get_end_effector(void);

  //! Solve IK and update the chain
  double update(void);

  //! Draw a marker
  void render_marker(Vector3f p);

  //! Draw a bone
  void render_bone(Vector3f v1, Vector3f v2);

  //! Draw the chain
  void render(void);

  //! Default Constructor
  ik_t()
  {
    num_bones=0; bone_length=10.0; num_dofs=0;
    root=Vector3f(0.0,0.0,0.0);
  }

  //! Add a node to a chain
  void add_node(ik_node_t _node)
  {
    node_list.push_back(_node);
    num_bones++;
    num_dofs += _node.dof;
  }
  
  //! Initialize the chain by computing all joint matrices and positions
  void init(void)
  {
    update_all_nodes();
  }

  //! Reset the chain 
  void reset(void)
  {
    for(int i=0;i<num_bones; i++)
      {
	     VectorXf theta = node_list[i].theta;
	     for (unsigned int j=0;j<theta.size();j++)
	     {
	       node_list[i].theta[j]=10.0;
	     }
      }
    update_all_nodes();
  }
};

#endif
