#pragma once
#include "MeshSphere.h"
#include "MeshBox.h"
class Collision
{
public :
	//呼びやすいようstaticで作る  玉と玉の衝突
	static bool CheckBoundingSphere(MeshSphere* s1, MeshSphere* s2);
	//AABB同士の衝突判定 と　X,Y,Zの最小値と最大値を持つ箱同士の衝突判定
	static bool CheckAABB(MeshBox* b1, MeshBox* b2);
	//球とAABBの衝突判定
	static bool CheckBallAndBox(MeshSphere* s1, MeshBox* b1);
	//玉とBOXの衝突判定（グリーン）
	static void Refrection(MeshSphere* sphere, MeshBox* box);
	//玉とBoxの衝突判定(ラフ)
	static void Refrection2(MeshSphere* sphere, MeshBox* box);
	//玉とBoxの衝突判定(バンカー)
	static void Refrection3(MeshSphere* sphere, MeshBox* box);
	//玉と玉の衝突判定
	static void RefrectionSphere(MeshSphere* sphere, MeshSphere* sphere2);
	//おまけ:線分とABBの衝突判定 めちゃくちゃスピードが速い物体の衝突判定に使える
	static bool CheckSegmentAndAABB(XMFLOAT3 strat, XMFLOAT3 end, MeshBox* box, float& outT);
};

