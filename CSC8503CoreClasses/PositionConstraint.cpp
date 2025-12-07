//#include "PositionConstraint.h"
//#include "GameObject.h"
//#include "PhysicsObject.h"
//
//using namespace NCL;
//using namespace Maths;
//using namespace CSC8503;
//
//PositionConstraint::PositionConstraint(GameObject* a, GameObject* b, float d)
//{
//	objectA		= a;
//	objectB		= b;
//	distance	= d;
//}
//
////a simple constraint that stops objects from being more than <distance> away
////from each other...this would be all we need to simulate a rope, or a ragdoll
//void PositionConstraint::UpdateConstraint(float dt) {
//    Vector3 relativePos =
//        objectA->GetTransform().GetPosition() -
//        objectB->GetTransform().GetPosition();
//
//    float currentDistance = Vector::Length(relativePos);
//    float offset = distance - currentDistance;
//
//    if (abs(offset) > 0.0f) {
//        Vector3 offsetDir = Vector::Normalise(relativePos);
//
//        PhysicsObject* physA = objectA->GetPhysicsObject();
//        PhysicsObject* physB = objectB->GetPhysicsObject();
//
//        Vector3 relativeVelocity =
//            physA->GetLinearVelocity() -
//            physB->GetLinearVelocity();
//
//        float constraintMass =
//            physA->GetInverseMass() +
//            physB->GetInverseMass();
//
//        if (constraintMass > 0.0f) {
//            float velocityDot = Vector::Dot(relativeVelocity, offsetDir);
//
//            float biasFactor = 0.01f;
//            float bias = -(biasFactor / dt) * offset;
//
//            float lambda = -(velocityDot + bias) / constraintMass;
//
//            Vector3 aImpulse = offsetDir * lambda;
//            Vector3 bImpulse = -offsetDir * lambda;
//
//            physA->ApplyLinearImpulse(aImpulse);   // multiplied by mass internally
//            physB->ApplyLinearImpulse(bImpulse);   // multiplied by mass internally
//        }
//    }
//}
//
#include "PositionConstraint.h"
#include "GameObject.h"
#include "PhysicsObject.h"
#include "Transform.h"

using namespace NCL;
using namespace NCL::Maths;
using namespace NCL::CSC8503;

PositionConstraint::PositionConstraint(GameObject* a, GameObject* b, float d) {
	objectA = a;
	objectB = b;
	distance = d;
}

// a simple constraint that stops objects from being more than <distance> away
// from each other...this would be all we need to simulate a rope, or a ragdoll
void PositionConstraint::UpdateConstraint(float dt) {
	if (!objectA || !objectB) {
		return;
	}

	Vector3 relativePos =
		objectA->GetTransform().GetPosition() -
		objectB->GetTransform().GetPosition();

	// 当前两物体之间的距离
	float currentDistance = Vector::Length(relativePos);
	float offset = distance - currentDistance;

	// 如果已经在“理想距离”上，就不用管了
	if (std::abs(offset) > 0.0f) {
		Vector3 offsetDir = Vector::Normalise(relativePos);

		PhysicsObject* physA = objectA->GetPhysicsObject();
		PhysicsObject* physB = objectB->GetPhysicsObject();

		if (!physA || !physB) {
			return;
		}

		Vector3 relativeVelocity =
			physA->GetLinearVelocity() -
			physB->GetLinearVelocity();

		float constraintMass =
			physA->GetInverseMass() +
			physB->GetInverseMass();

		if (constraintMass > 0.0f) {
			// 速度在约束方向上的分量
			float velocityDot = Vector::Dot(relativeVelocity, offsetDir);

			// Baumgarte bias，防止约束积累误差
			float biasFactor = 0.01f;
			float bias = -(biasFactor / dt) * offset;

			// 求解冲量标量 lambda
			float lambda = -(velocityDot + bias) / constraintMass;

			Vector3 aImpulse = offsetDir * lambda;
			Vector3 bImpulse = -offsetDir * lambda;

			physA->ApplyLinearImpulse(aImpulse);
			physB->ApplyLinearImpulse(bImpulse);
		}
	}
}

