#include <iostream>
#include "ik.hpp"

float K = 3.14159265 / 180.0;

void ik_t::update_all_nodes(void)
{
	//! TODO CS775
	//! Compute T, invT, abs_pos for all nodes in the node_list
	//! Assume the theta's for all the nodes have velid values.
	//! Compute and assign the end effector position to end_effector.
	node_list[0].T = AngleAxisf(-K*node_list[0].theta[0], Vector3f::UnitX()) *
	                 AngleAxisf(-K*node_list[0].theta[1], Vector3f::UnitY()) *
	                 AngleAxisf(-K*node_list[0].theta[2], Vector3f::UnitZ());
	node_list[0].invT = AngleAxisf(K*node_list[0].theta[2], Vector3f::UnitZ()) *
	                    AngleAxisf(K*node_list[0].theta[1], Vector3f::UnitY()) *
	                    AngleAxisf(K*node_list[0].theta[0], Vector3f::UnitX());
	node_list[0].abs_pos = Vector3f(0, 0, 0);
	std::cerr << node_list[0].T.matrix() <<std::endl;
	std::cerr << node_list[0].abs_pos.matrix() << std::endl;
	for (int i = 1; i < num_bones; i++) {
		node_list[i].T = AngleAxisf(K*node_list[i].theta[0], Vector3f::UnitZ()) *
		                 Translation3f(bone_length, 0, 0) *
		                 node_list[i-1].T;
		node_list[i].invT = node_list[i-1].invT *
		                    Translation3f(bone_length, 0, 0) *
		                    AngleAxisf(K*node_list[i].theta[0], Vector3f::UnitZ());
		node_list[i].abs_pos = node_list[i].invT * Vector3f(0, 0, 0);
		std::cerr << node_list[i].T.matrix() <<std::endl;
		std::cerr << node_list[i].abs_pos.matrix() << std::endl;
	}
	end_effector = node_list[num_bones - 1].invT * Vector3f(bone_length, 0, 0);
	std::cerr << end_effector <<std::endl;
}

const Vector3f& ik_t::get_end_effector(void)
{
	return end_effector;
}

void ik_t::update(void)
{
	Vector3f tmpTarget = target;
	Vector3f targetDiff = tmpTarget - root;
	float l = targetDiff.norm();
	float maxLen = bone_length*(float)num_bones - 0.01f;
	if (l > maxLen) {
		targetDiff *= maxLen/l;
		l = targetDiff.norm();
		tmpTarget = root + targetDiff;
	}

	Vector3f endPos = end_effector;
	Vector3f diff = tmpTarget - endPos;


	float maxAngle = 360.0f/(float)num_bones;



	for(int loop = 0; loop < 1; loop++) {
		MatrixXf jacobian(diff.rows(), num_bones+2);
		jacobian.setZero();
		float step = 1.0f;
		float steprad = step*M_PI/180.0;
		for (int i = 0; i < num_dofs; i++) {
			//! TODO CS775
			//! Construct the matrix jacobian
			//! Remember there has to be a line in the Jacobian corresponding to each dof in your chain
			
			//AngleAxisf Ti;
			//Ti.setIdentity();
			//for (int j = 0; j < i; j++)
			//Ti = Ti * nodelist[j].T;
			Vector3f Df4f5f6 = Matrix3f(node_list[i].T.linear()) * Vector3f(0,0,1);
			Vector3f Pi(node_list[i].T.translation());
			Vector3f Df1f2f3 = Df4f5f6.cross(end_effector - Pi);

			jacobian(0,i) = Df1f2f3(0);
			jacobian(1,i) = Df1f2f3(1);
			jacobian(2,i) = Df1f2f3(2);
		}

		MatrixXf invJ;
		JacobiSVD<MatrixXf> svd(jacobian, ComputeThinU | ComputeThinV);
		double tolerance = 1e-6 * std::max(jacobian.cols(), jacobian.rows()) * svd.singularValues().array().abs().maxCoeff();

		//! Inverting the Jacobian
		invJ = svd.matrixV() * MatrixXf( (svd.singularValues().array().abs() > tolerance).select(svd.singularValues().array().inverse(), 0) ).asDiagonal() * svd.matrixU().adjoint();


		VectorXf add = invJ*diff;

		//Updating the node thetas
		node_list[0].theta[0] += add[0];
		node_list[0].theta[1] += add[1];
		node_list[0].theta[2] += add[2];
		if (add.size() != num_bones+2) exit(-1);

		for (int i = 3; i < add.size(); i++)
			node_list[i-2].theta[0] += add[i];
	}
	update_all_nodes();
}

void ik_t::render_marker(Vector3f p)
{
	glPushMatrix();

	glBegin(GL_LINES);
	glVertex3f(p[0]-5, p[1], p[2]);
	glVertex3f(p[0]+5, p[1], p[2]);
	glVertex3f(p[0], p[1]-5, p[2]);
	glVertex3f(p[0], p[1]+5, p[2]);
	glVertex3f(p[0], p[1], p[2]-5);
	glVertex3f(p[0], p[1], p[2]+5);
	glEnd();
	glPopMatrix();
}

void ik_t::render_bone(Vector3f v1, Vector3f v2)
{
	glBegin(GL_LINES);
	glVertex3f(v1[0],v1[1],v1[2]);
	glVertex3f(v2[0],v2[1],v2[2]);
	glEnd();
}

void ik_t::render(void)
{
	glPushMatrix();
	for (int i = 0; i < num_bones; i++) {
		if (i < num_bones-1) {
			render_bone(node_list[i].abs_pos, node_list[i+1].abs_pos);
		} else {
			render_bone(node_list[i].abs_pos, end_effector);
		}
	}
	glPopMatrix();

	render_marker(target);
	render_marker(end_effector);
}
