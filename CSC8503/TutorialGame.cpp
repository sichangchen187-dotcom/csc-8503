#include "TutorialGame.h"
#include "GameWorld.h"
#include "PhysicsSystem.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"

#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "StateGameObject.h"

#include "Window.h"
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"

#include "Debug.h"

#include "KeyboardMouseController.h"

#include "GameTechRendererInterface.h"

#include "Ray.h"
#define PI 3.14159265358979323846f
using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame(GameWorld& inWorld, GameTechRendererInterface& inRenderer, PhysicsSystem& inPhysics)
	:	world(inWorld),
		renderer(inRenderer),
		physics(inPhysics)
{

	forceMagnitude	= 10.0f;
	useGravity		= false;
	inSelectionMode = false;

	controller = new KeyboardMouseController(*Window::GetWindow()->GetKeyboard(), *Window::GetWindow()->GetMouse());

	world.GetMainCamera().SetController(*controller);

	world.SetSunPosition({ -200.0f, 60.0f, -200.0f });
	world.SetSunColour({ 0.8f, 0.8f, 0.5f });

	controller->MapAxis(0, "Sidestep");
	controller->MapAxis(1, "UpDown");
	controller->MapAxis(2, "Forward");

	controller->MapAxis(3, "XLook");
	controller->MapAxis(4, "YLook");

	cubeMesh	= renderer.LoadMesh("cube.msh");
	sphereMesh	= renderer.LoadMesh("sphere.msh");
	catMesh		= renderer.LoadMesh("ORIGAMI_Chat.msh");
	kittenMesh	= renderer.LoadMesh("Kitten.msh");

	enemyMesh	= renderer.LoadMesh("Keeper.msh");

	bonusMesh	= renderer.LoadMesh("19463_Kitten_Head_v1.msh");
	capsuleMesh = renderer.LoadMesh("capsule.msh");

	defaultTex	= renderer.LoadTexture("Default.png");
	checkerTex	= renderer.LoadTexture("checkerboard.png");
	glassTex	= renderer.LoadTexture("stainedglass.tga");

	checkerMaterial.type		= MaterialType::Opaque;
	checkerMaterial.diffuseTex	= checkerTex;

	glassMaterial.type			= MaterialType::Transparent;
	glassMaterial.diffuseTex	= glassTex;

	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame()	{
}

void TutorialGame::UpdateGame(float dt) {
	
	if (testStateObject) {
		testStateObject->Update(dt);
	}
	if (!inSelectionMode) {
		world.GetMainCamera().UpdateCamera(dt);
	}
	if (playerObject) {
		const float moveForce = 30.0f;   // 可以自己调大小

		Camera& cam = world.GetMainCamera();

		// 从相机矩阵里取出 forward / right
		Matrix4 view = cam.BuildViewMatrix();
		Matrix4 camWorld = Matrix::Inverse(view);

		Vector3 forward = -Vector3(camWorld.GetColumn(2)); // 相机看向的方向
		Vector3 right = Vector3(camWorld.GetColumn(0)); // 相机右边的方向

		// 只在地面平面移动
		forward.y = 0.0f;
		right.y = 0.0f;

		forward = Vector::Normalise(forward);
		right = Vector::Normalise(right);

		Vector3 moveDir(0, 0, 0);

		// 👇 这里就是“相机相对”的 WSAD 了
		if (Window::GetKeyboard()->KeyDown(KeyCodes::W)) {
			moveDir += forward;   // 朝相机正前方
		}
		if (Window::GetKeyboard()->KeyDown(KeyCodes::S)) {
			moveDir -= forward;   // 朝相机后方
		}
		if (Window::GetKeyboard()->KeyDown(KeyCodes::A)) {
			moveDir -= right;     // 向相机左侧
		}
		if (Window::GetKeyboard()->KeyDown(KeyCodes::D)) {
			moveDir += right;     // 向相机右侧
		}


		if (Vector::LengthSquared(moveDir) > 0.0f) {
			moveDir = Vector::Normalise(moveDir);

			// 推猫
			playerObject->GetPhysicsObject()->AddForce(moveDir * moveForce);

			//// 让猫朝着自己的运动方向转头
			//float yawRad = atan2(moveDir.x, moveDir.z);     // 视情况 +/- 号微调
			//float yawDeg = yawRad * 180.0f / PI;

			//Quaternion catRot = Quaternion::EulerAnglesToQuaternion(0.0f, yawDeg, 0.0f);
			//playerObject->GetTransform().SetOrientation(catRot);
		}
		// ====== 这里开始：猫头跟着鼠标（相机 yaw）转 ======
		float camYaw = cam.GetYaw();

		// 视模型而定，之前你是“背对屏幕才对”，所以加 180 度：
		float catYaw = camYaw + 180.0f;   // 如果反了就去掉 / 改成 -180.0f 试试

		Quaternion catRot = Quaternion::EulerAnglesToQuaternion(0.0f, catYaw, 0.0f);
		playerObject->GetTransform().SetOrientation(catRot);
		// ====== 猫头控制结束 ======
	}
	if (lockedObject != nullptr) {
		// 先让相机控制器（鼠标）更新 yaw / pitch
	// （上面已经在 !inSelectionMode 时调用了 UpdateCamera(dt)）

		Camera& cam = world.GetMainCamera();

		// 根据当前相机的 yaw / pitch 算出“朝向”向量
		Matrix4 view = cam.BuildViewMatrix();
		Matrix4 camWorld = Matrix::Inverse(view);
		Vector3 forward = -Vector3(camWorld.GetColumn(2)); // 相机朝向（摄像机看向的方向）

		Vector3 objPos = lockedObject->GetTransform().GetPosition();

		// 你可以用 lockedOffset 里面的 y / z 定义高度和距离，
		// 或者直接写死一个你觉得舒服的值：
		float height = 3.0f;   // 类似你原来 lockedOffset.y
		float dist = 12.0f;   // 类似 lockedOffset.z 的绝对值

		// 把摄像机放在“猫的后方 dist 距离，再往上抬 height”
		Vector3 camPos = objPos - Vector::Normalise(forward) * dist + Vector3(0, height, 0);

		cam.SetPosition(camPos);
		// ⚠️ 注意：不再改 pitch / yaw，完全交给鼠标控制
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics.UseGravity(useGravity);
	}
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F9)) {
		world.ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F10)) {
		world.ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F7)) {
		world.ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F8)) {
		world.ShuffleObjects(false);
	}

	if (lockedObject) {
		LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}

	RayCollision closestCollision;
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::K) && selectionObject) {
		Vector3 rayPos;
		Vector3 rayDir;

		rayDir = selectionObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);

		rayPos = selectionObject->GetTransform().GetPosition();

		Ray r = Ray(rayPos, rayDir);

		if (world.Raycast(r, closestCollision, true, selectionObject)) {
			if (objClosest) {
				objClosest->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
			}
			objClosest = (GameObject*)closestCollision.node;

			objClosest->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
		}
	}

	//This year we can draw debug textures as well!
	Debug::DrawTex(*defaultTex, Vector2(10, 10), Vector2(5, 5), Debug::WHITE);
	Debug::DrawLine(Vector3(), Vector3(0, 100, 0), Vector4(1, 0, 0, 1));
	if (useGravity) {
		Debug::Print("(G)ravity on", Vector2(5, 95), Debug::RED);
	}
	else {
		Debug::Print("(G)ravity off", Vector2(5, 95), Debug::RED);
	}

	SelectObject();
	MoveSelectedObject();	
	
	world.OperateOnContents(
		[dt](GameObject* o) {
			o->Update(dt);
		}
	);
}

void TutorialGame::InitCamera() {
	world.GetMainCamera().SetNearPlane(0.1f);
	world.GetMainCamera().SetFarPlane(500.0f);
	world.GetMainCamera().SetPitch(-15.0f);
	world.GetMainCamera().SetYaw(315.0f);
	world.GetMainCamera().SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld() {
	world.ClearAndErase();
	physics.Clear();

	CreatedMixedGrid(15, 15, 3.5f, 3.5f);

	InitGameExamples();

	AddFloorToWorld(Vector3(0, -20, 0));
	BridgeConstraintTest();
	testStateObject = AddStateObjectToWorld(Vector3(0, 10, 0));
}

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject();

	Vector3 floorSize = Vector3(200, 2, 200);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume(volume);
	floor->GetTransform()
		.SetScale(floorSize * 2.0f)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(floor->GetTransform(), cubeMesh, checkerMaterial));
	floor->SetPhysicsObject(new PhysicsObject(floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world.AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(0.5, 0.5, 0.5);
	SphereVolume* volume = new SphereVolume(0.5f);
	sphere->SetBoundingVolume(volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(sphere->GetTransform(), sphereMesh, checkerMaterial));

	sphere->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));   // 红色 RGBA

	sphere->SetPhysicsObject(new PhysicsObject(sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world.AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume(volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 3.0f);

	cube->SetRenderObject(new RenderObject(cube->GetTransform(), cubeMesh, checkerMaterial));
	cube->SetPhysicsObject(new PhysicsObject(cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world.AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize		= 2.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject();
	SphereVolume* volume  = new SphereVolume(2.0f);

	character->SetBoundingVolume(volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(character->GetTransform(), catMesh, notexMaterial));
	character->SetPhysicsObject(new PhysicsObject(character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	
	

	world.AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize		= 3.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject();

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume(volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(character->GetTransform(), enemyMesh, notexMaterial));
	character->SetPhysicsObject(new PhysicsObject(character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world.AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {
	GameObject* apple = new GameObject();

	SphereVolume* volume = new SphereVolume(0.5f);
	apple->SetBoundingVolume(volume);
	apple->GetTransform()
		.SetScale(Vector3(0.5, 0.5, 0.5))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(apple->GetTransform(), bonusMesh, glassMaterial));
	apple->SetPhysicsObject(new PhysicsObject(apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world.AddGameObject(apple);

	return apple;
}

StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position) {
	auto* apple = new StateGameObject();

	SphereVolume* volume = new SphereVolume(0.5f);
	apple->SetBoundingVolume(volume);
	apple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(apple->GetTransform(), bonusMesh, glassMaterial));
	apple->SetPhysicsObject(new PhysicsObject(apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world.AddGameObject(apple);

	return apple;
}

void TutorialGame::InitGameExamples() {
	playerObject = AddPlayerToWorld(Vector3(0, 5, 0));
	lockedObject = playerObject;
	lockedOffset = Vector3(0, 2, 0.5f);
	AddEnemyToWorld(Vector3(5, 5, 0));
	AddBonusToWorld(Vector3(10, 5, 0));
}

void TutorialGame::CreateSphereGrid(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::CreatedMixedGrid(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims);
			}
			else {
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
}

void TutorialGame::CreateAABBGrid(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
}

/*
Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		Debug::Print("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::Left)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(world.GetMainCamera());

			RayCollision closestCollision;
			if (world.Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;

				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else {
				return false;
			}
		}
		if (Window::GetKeyboard()->KeyPressed(NCL::KeyCodes::L)) {
			if (selectionObject) {
				if (lockedObject == selectionObject) {
					lockedObject = nullptr;
				}
				else {
					lockedObject = selectionObject;
				}
			}
		}
	}
	else {
		Debug::Print("Press Q to change to select mode!", Vector2(5, 85));
	}
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void TutorialGame::MoveSelectedObject() {
	Debug::Print("Click Force: " + std::to_string(forceMagnitude), Vector2(5, 90));
	//renderer->DrawString("Click Force: " + std::to_string(forceMagnitude), Vector2(10, 20));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;//we haven't selected anything!
	}
	//Push the selected object!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::Right)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(world.GetMainCamera());

		RayCollision closestCollision;
		if (world.Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(
					ray.GetDirection() * forceMagnitude,
					closestCollision.collidedAt);
			}
		}
	}
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view = world.GetMainCamera().BuildViewMatrix();
	Matrix4 camWorld = Matrix::Inverse(view);

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis = Vector::Normalise(fwdAxis);

	if (Window::GetKeyboard()->KeyDown(KeyCodes::UP)) {
		selectionObject->GetPhysicsObject()->AddForce(fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyCodes::DOWN)) {
		selectionObject->GetPhysicsObject()->AddForce(-fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyCodes::NEXT)) {
		selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
	}
}

void TutorialGame::DebugObjectMovement() {
	//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyCodes::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}
}


void TutorialGame::BridgeConstraintTest() {
	Vector3 cubeSize = Vector3(2, 2, 2);    // 方块半尺寸（实际大小 4x4x4）

	float invCubeMass = 1.0f;              // 中间节点的“逆质量” (1/mass)
	int   numLinks = 10;                // 中间方块数量
	float cubeDistance = 10.0f;             // 相邻方块之间的距离
	float maxDistance = 12.0f;             // 约束允许的最大距离（略大于 cubeDistance）

	// 把桥放在原点附近，略微高于地板（地板 y = -20）
	Vector3 startPos = Vector3(-50, 30, 0);  // 从左往右拉一条链子

	// 左端固定块（inverse mass = 0）
	GameObject* start = AddCubeToWorld(
		startPos,
		cubeSize,
		0.0f
	);

	// 右端固定块
	GameObject* end = AddCubeToWorld(
		startPos + Vector3((numLinks + 1) * cubeDistance, 0, 0),
		cubeSize,
		0.0f
	);

	GameObject* previous = start;

	// 中间的活动方块 + 每段之间的 PositionConstraint
	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(
			startPos + Vector3((i + 1) * cubeDistance, 0, 0),
			cubeSize,
			invCubeMass        // 有质量的中间节
		);

		PositionConstraint* constraint =
			new PositionConstraint(previous, block, maxDistance);

		// 注意这里用的是 world .AddConstraint（不是 ->）
		world.AddConstraint(constraint);

		previous = block;
	}

	// 最后一节与右端固定块的约束
	PositionConstraint* finalConstraint =
		new PositionConstraint(previous, end, maxDistance);

	world.AddConstraint(finalConstraint);
}
