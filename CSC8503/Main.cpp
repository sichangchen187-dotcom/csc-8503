//#include "Window.h"
//#include "../NCLCoreClasses/Vector.h"
//#include "Debug.h"
//#include "Keyboard.h"
//#include "StateMachine.h"
//#include "StateTransition.h"
//#include "State.h"
//#include <cstdlib>   
//#include "GameServer.h"
//#include "GameClient.h"
//
//#include "NavigationGrid.h"
//#include "NavigationMesh.h"
//#include "PushdownState.h"
//#include "PushdownMachine.h"
//#include "TutorialGame.h"
//#include "NetworkedGame.h"
//
//#include "PushdownMachine.h"
//
//#include "PushdownState.h"
//
//#include "BehaviourNode.h"
//#include "BehaviourSelector.h"
//#include "BehaviourSequence.h"
//#include "BehaviourAction.h"
//
//#include "PhysicsSystem.h"
//
//#ifdef USEOPENGL
//#include "GameTechRenderer.h"
//#define CAN_COMPILE
//#endif
//#ifdef USEVULKAN
//#include "GameTechVulkanRenderer.h"
//#define CAN_COMPILE
//#endif
//
//using namespace NCL;
//using namespace CSC8503;
//
//#include <chrono>
//#include <thread>
//#include <sstream>
//
//void TestStateMachine() {
//	StateMachine* testMachine = new StateMachine();
//	int data = 0;
//	State* A = new State([&](float dt) -> void {
//		std::cout << "Running State A!\n";
//		data++;
//		});
//
//	State* B = new State([&](float dt) -> void {
//		std::cout << "Running State B!\n";
//		data--;
//		});
//	StateTransition* stateAB = new StateTransition(A, B, [&]() -> bool {
//		return data > 10;
//		});
//
//	StateTransition* stateBA = new StateTransition(B, A, [&]() -> bool {
//		return data < 0;
//		});
//	testMachine->AddState(A);
//	testMachine->AddState(B);
//	testMachine->AddTransition(stateAB);
//	testMachine->AddTransition(stateBA);
//
//	for (int i = 0; i < 100; ++i) {
//		testMachine->Update(1.0f);
//	}
//}
//
//std::vector<Vector3> testNodes;
//
//void TestPathfinding() {
//	NavigationGrid grid("TestGrid1.txt");
//	NavigationPath outPath;
//
//	Vector3 startPos(20, 0, 20);
//	Vector3 endPos(160, 0, 160);
//
//	bool found = grid.FindPath(startPos, endPos, outPath);
//	std::cout << "FindPath = " << found << std::endl;
//
//	testNodes.clear();
//
//	int c = 0;
//	Vector3 pos;
//	while (outPath.PopWaypoint(pos)) {
//		testNodes.push_back(pos);
//		std::cout << "Waypoint " << c
//			<< ": (" << pos.x << ", " << pos.y << ", " << pos.z << ")\n";
//		c++;
//	}
//	std::cout << "Total waypoints = " << c << std::endl;
//}
//
//void DisplayPathfinding() {
//	for (int i = 1; i < testNodes.size(); ++i) {
//		Vector3 a = testNodes[i - 1];
//		Vector3 b = testNodes[i];
//
//		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
//	}
//}
//void TestPathfinding();
//void DisplayPathfinding();
//
//
//void TestBehaviourTree() {
//    float behaviourTimer;
//    float distanceToTarget;
//
//    // 1. 寻找钥匙
//    BehaviourAction* findKey = new BehaviourAction("FindKey",
//        [&](float dt, BehaviourState state)->BehaviourState {
//            if (state == Initialise) {
//                std::cout << "Looking for a key!\n";
//                behaviourTimer = rand() % 100;
//                state = Ongoing;
//            }
//            else if (state == Ongoing) {
//                behaviourTimer -= dt;
//                if (behaviourTimer <= 0.0f) {
//                    std::cout << "Found a key!\n";
//                    return Success;
//                }
//            }
//            return state; // 在成功之前将是'ongoing'
//        }
//    );
//
//    // 2. 前往房间
//    BehaviourAction* goToRoom = new BehaviourAction("GoToRoom",
//        [&](float dt, BehaviourState state)->BehaviourState {
//            if (state == Initialise) {
//                std::cout << "Going to the loot room\n";
//                state = Ongoing;
//            }
//            else if (state == Ongoing) {
//                distanceToTarget -= dt;
//                if (distanceToTarget <= 0.0f) {
//                    std::cout << "Reached room\n";
//                    return Success;
//                }
//            }
//            return state; // 在成功之前将是'ongoing'
//        }
//    );
//
//    // 3. 开门
//    BehaviourAction* openDoor = new BehaviourAction("OpenDoor",
//        [&](float dt, BehaviourState state)->BehaviourState {
//            if (state == Initialise) {
//                std::cout << "Opening Door!\n";
//                return Success;
//            }
//            return state;
//        }
//    );
//
//    // 4. 寻找宝藏
//    BehaviourAction* lookForTreasure = new BehaviourAction(
//        "LookForTreasure",
//        [&](float dt, BehaviourState state)->BehaviourState {
//            if (state == Initialise) {
//                std::cout << "Looking for treasure!\n";
//                return Ongoing;
//            }
//            else if (state == Ongoing) {
//                bool found = rand() % 2;
//                if (found) {
//                    std::cout << "I found some treasure!\n";
//                    return Success;
//                }
//                std::cout << "No treasure in here...\n";
//                return Failure;
//            }
//            return state;
//        }
//    );
//
//    // 5. 寻找物品
//    BehaviourAction* lookForItems = new BehaviourAction(
//        "LookForItems",
//        [&](float dt, BehaviourState state)->BehaviourState {
//            if (state == Initialise) {
//                std::cout << "Looking for items!\n";
//                return Ongoing;
//            }
//            else if (state == Ongoing) {
//                bool found = rand() % 2;
//                if (found) {
//                    std::cout << "I found some items!\n";
//                    return Success;
//                }
//                std::cout << "No items in here...\n";
//                return Failure;
//            }
//            return state;
//        }
//    );
//
//    // 构建行为树结构
//    BehaviourSequence* sequence = new BehaviourSequence("Room Sequence");
//    sequence->AddChild(findKey);
//    sequence->AddChild(goToRoom);
//    sequence->AddChild(openDoor);
//
//    BehaviourSelector* selection = new BehaviourSelector("Loot Selection");
//    selection->AddChild(lookForTreasure);
//    selection->AddChild(lookForItems);
//
//    BehaviourSequence* rootSequence = new BehaviourSequence("Root Sequence");
//    rootSequence->AddChild(sequence);
//    rootSequence->AddChild(selection);
//
//    // 运行行为树5次
//    for (int i = 0; i < 5; ++i) {
//        rootSequence->Reset();
//        behaviourTimer = 0.0f;
//        distanceToTarget = rand() % 250;
//        BehaviourState state = Ongoing;
//        std::cout << "We're going on an adventure!\n";
//        while (state == Ongoing) {
//            state = rootSequence->Execute(1.0f); // 假的时间步长
//        }
//        if (state == Success) {
//            std::cout << "What a successful adventure!\n";
//        }
//        else if (state == Failure) {
//            std::cout << "What a waste of time!\n";
//        }
//    }
//    std::cout << "All done!\n";
//}
//
//class PauseScreen : public PushdownState {
//public:
//    PushdownResult OnUpdate(float dt, PushdownState** newState) override {
//        if (Window::GetKeyboard()->KeyPressed(KeyCodes::U)) {
//            return PushdownResult::Pop;
//        }
//        return PushdownResult::NoChange;
//    }
//
//    void OnAwake() override {
//        std::cout << "Press U to unpause game!\n";
//    }
//};
//
//class GameScreen : public PushdownState {
//public:
//    PushdownResult OnUpdate(float dt, PushdownState** newState) override {
//        pauseReminder -= dt;
//        if (pauseReminder < 0) {
//            std::cout << "Coins mined: " << coinsMined << "\n";
//            std::cout << "Press P to pause game, or F1 to return to main menu!\n";
//            pauseReminder += 1.0f;
//        }
//
//        if (Window::GetKeyboard()->KeyDown(KeyCodes::P)) {
//            *newState = new PauseScreen();
//            return PushdownResult::Push;
//        }
//
//        if (Window::GetKeyboard()->KeyDown(KeyCodes::F1)) {
//            std::cout << "Returning to main menu!\n";
//            return PushdownResult::Pop;
//        }
//
//        if (rand() % 7 == 0) {
//            coinsMined++;
//        }
//
//        return PushdownResult::NoChange;
//    }
//
//    void OnAwake() override {
//        std::cout << "Preparing to mine coins!\n";
//    }
//
//protected:
//    int coinsMined = 0;
//    float pauseReminder = 1.0f;
//};
//
//class IntroScreen : public PushdownState {
//public:
//    PushdownResult OnUpdate(float dt, PushdownState** newState) override {
//        if (Window::GetKeyboard()->KeyPressed(KeyCodes::SPACE)) {
//            *newState = new GameScreen();
//            return PushdownResult::Push;
//        }
//        if (Window::GetKeyboard()->KeyPressed(KeyCodes::ESCAPE)) {
//            return PushdownResult::Pop;
//        }
//        return PushdownResult::NoChange;
//    }
//
//    void OnAwake() override {
//        std::cout << "Welcome to a really awesome game!\n";
//        std::cout << "Press Space To Begin or Escape to quit!\n";
//    }
//};
//
//void TestPushdownAutomata(Window* w) {
//    PushdownMachine machine(new IntroScreen());
//
//    while (w->UpdateWindow()) {
//        float dt = w->GetTimer().GetTimeDeltaSeconds();
//        if (!machine.Update(dt)) {
//            return;
//        }
//    }
//}
//int main() {
//	WindowInitialisation initInfo;
//	initInfo.width		= 1280;
//	initInfo.height		= 720;
//	initInfo.windowTitle = "CSC8503 Game technology!";
//
//	Window*w = Window::CreateGameWindow(initInfo);
//
//	if (!w->HasInitialised()) {
//		return -1;
//	}	
//
//	w->ShowOSPointer(false);
//	w->LockMouseToWindow(true);
//
//	GameWorld* world = new GameWorld();
//	PhysicsSystem* physics = new PhysicsSystem(*world);
//
//#ifdef USEVULKAN
//	GameTechVulkanRenderer* renderer = new GameTechVulkanRenderer(*world);
//#elif USEOPENGL
//	GameTechRenderer* renderer = new GameTechRenderer(*world);
//#endif
//
//	TutorialGame* g = new TutorialGame(*world, *renderer, *physics);
//	TestPathfinding();
//	w->GetTimer().GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
//	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyCodes::ESCAPE)) {
//		float dt = w->GetTimer().GetTimeDeltaSeconds();
//		if (dt > 0.1f) {
//			std::cout << "Skipping large time delta" << std::endl;
//			continue; //must have hit a breakpoint or something to have a 1 second frame time!
//		}
//		if (Window::GetKeyboard()->KeyPressed(KeyCodes::PRIOR)) {
//			w->ShowConsole(true);
//		}
//		if (Window::GetKeyboard()->KeyPressed(KeyCodes::NEXT)) {
//			w->ShowConsole(false);
//		}
//
//		if (Window::GetKeyboard()->KeyPressed(KeyCodes::T)) {
//			w->SetWindowPosition(0, 0);
//		}
//
//		w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));
//
//		g->UpdateGame(dt);
//		TestBehaviourTree();
//		world->UpdateWorld(dt);
//		physics->Update(dt);
//		DisplayPathfinding();
//		renderer->Update(dt);	
//		renderer->Render();
//		
//		Debug::UpdateRenderables(dt);
//	}
//	Window::DestroyGameWindow();
//}
#include "Window.h"
#include "../NCLCoreClasses/Vector.h"
#include "Debug.h"
#include "Keyboard.h"
#include "StateMachine.h"
#include "StateTransition.h"
#include "State.h"
#include <cstdlib>   
#include "GameServer.h"
#include "GameClient.h"

#include "NavigationGrid.h"
#include "NavigationMesh.h"
#include "PushdownState.h"
#include "PushdownMachine.h"
#include "TutorialGame.h"
#include "NetworkedGame.h"

#include "PushdownMachine.h"

#include "PushdownState.h"

#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"

#include "PhysicsSystem.h"

#ifdef USEOPENGL
#include "GameTechRenderer.h"
#define CAN_COMPILE
#endif
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#define CAN_COMPILE
#endif

using namespace NCL;
using namespace CSC8503;
bool gGameStarted = false;
bool gIsPaused = false;
#include <chrono>
#include <thread>
#include <sstream>

void TestStateMachine() {
    StateMachine* testMachine = new StateMachine();
    int data = 0;
    State* A = new State([&](float dt) -> void {
        std::cout << "Running State A!\n";
        data++;
        });

    State* B = new State([&](float dt) -> void {
        std::cout << "Running State B!\n";
        data--;
        });
    StateTransition* stateAB = new StateTransition(A, B, [&]() -> bool {
        return data > 10;
        });

    StateTransition* stateBA = new StateTransition(B, A, [&]() -> bool {
        return data < 0;
        });
    testMachine->AddState(A);
    testMachine->AddState(B);
    testMachine->AddTransition(stateAB);
    testMachine->AddTransition(stateBA);

    for (int i = 0; i < 100; ++i) {
        testMachine->Update(1.0f);
    }
}

std::vector<Vector3> testNodes;

void TestPathfinding() {
    NavigationGrid grid("TestGrid1.txt");
    NavigationPath outPath;

    Vector3 startPos(20, 0, 20);
    Vector3 endPos(160, 0, 160);

    bool found = grid.FindPath(startPos, endPos, outPath);
    std::cout << "FindPath = " << found << std::endl;

    testNodes.clear();

    int c = 0;
    Vector3 pos;
    while (outPath.PopWaypoint(pos)) {
        testNodes.push_back(pos);
        std::cout << "Waypoint " << c
            << ": (" << pos.x << ", " << pos.y << ", " << pos.z << ")\n";
        c++;
    }
    std::cout << "Total waypoints = " << c << std::endl;
}

void DisplayPathfinding() {
    for (int i = 1; i < testNodes.size(); ++i) {
        Vector3 a = testNodes[i - 1];
        Vector3 b = testNodes[i];

        Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
    }
}
void TestPathfinding();
void DisplayPathfinding();


void TestBehaviourTree() {
    float behaviourTimer;
    float distanceToTarget;

    // 1. 寻找钥匙
    BehaviourAction* findKey = new BehaviourAction("FindKey",
        [&](float dt, BehaviourState state)->BehaviourState {
            if (state == Initialise) {
                std::cout << "Looking for a key!\n";
                behaviourTimer = rand() % 100;
                state = Ongoing;
            }
            else if (state == Ongoing) {
                behaviourTimer -= dt;
                if (behaviourTimer <= 0.0f) {
                    std::cout << "Found a key!\n";
                    return Success;
                }
            }
            return state; // 在成功之前将是'ongoing'
        }
    );

    // 2. 前往房间
    BehaviourAction* goToRoom = new BehaviourAction("GoToRoom",
        [&](float dt, BehaviourState state)->BehaviourState {
            if (state == Initialise) {
                std::cout << "Going to the loot room\n";
                state = Ongoing;
            }
            else if (state == Ongoing) {
                distanceToTarget -= dt;
                if (distanceToTarget <= 0.0f) {
                    std::cout << "Reached room\n";
                    return Success;
                }
            }
            return state; // 在成功之前将是'ongoing'
        }
    );

    // 3. 开门
    BehaviourAction* openDoor = new BehaviourAction("OpenDoor",
        [&](float dt, BehaviourState state)->BehaviourState {
            if (state == Initialise) {
                std::cout << "Opening Door!\n";
                return Success;
            }
            return state;
        }
    );

    // 4. 寻找宝藏
    BehaviourAction* lookForTreasure = new BehaviourAction(
        "LookForTreasure",
        [&](float dt, BehaviourState state)->BehaviourState {
            if (state == Initialise) {
                std::cout << "Looking for treasure!\n";
                return Ongoing;
            }
            else if (state == Ongoing) {
                bool found = rand() % 2;
                if (found) {
                    std::cout << "I found some treasure!\n";
                    return Success;
                }
                std::cout << "No treasure in here...\n";
                return Failure;
            }
            return state;
        }
    );

    // 5. 寻找物品
    BehaviourAction* lookForItems = new BehaviourAction(
        "LookForItems",
        [&](float dt, BehaviourState state)->BehaviourState {
            if (state == Initialise) {
                std::cout << "Looking for items!\n";
                return Ongoing;
            }
            else if (state == Ongoing) {
                bool found = rand() % 2;
                if (found) {
                    std::cout << "I found some items!\n";
                    return Success;
                }
                std::cout << "No items in here...\n";
                return Failure;
            }
            return state;
        }
    );

    // 构建行为树结构
    BehaviourSequence* sequence = new BehaviourSequence("Room Sequence");
    sequence->AddChild(findKey);
    sequence->AddChild(goToRoom);
    sequence->AddChild(openDoor);

    BehaviourSelector* selection = new BehaviourSelector("Loot Selection");
    selection->AddChild(lookForTreasure);
    selection->AddChild(lookForItems);

    BehaviourSequence* rootSequence = new BehaviourSequence("Root Sequence");
    rootSequence->AddChild(sequence);
    rootSequence->AddChild(selection);

    // 运行行为树5次
    for (int i = 0; i < 5; ++i) {
        rootSequence->Reset();
        behaviourTimer = 0.0f;
        distanceToTarget = rand() % 250;
        BehaviourState state = Ongoing;
        std::cout << "We're going on an adventure!\n";
        while (state == Ongoing) {
            state = rootSequence->Execute(1.0f); // 假的时间步长
        }
        if (state == Success) {
            std::cout << "What a successful adventure!\n";
        }
        else if (state == Failure) {
            std::cout << "What a waste of time!\n";
        }
    }
    std::cout << "All done!\n";
}

class PauseScreen : public PushdownState {
public:
    PushdownResult OnUpdate(float dt, PushdownState** newState) override {
        if (Window::GetKeyboard()->KeyPressed(KeyCodes::U)) {
            // 按 U 解除暂停，Pop 回到 GameScreen
            std::cout << "Unpausing game!\n";
            return PushdownResult::Pop;
        }
        return PushdownResult::NoChange;
    }

    void OnAwake() override {
        gIsPaused = true; // 进入暂停状态
        std::cout << "==== GAME PAUSED ====\n";
        std::cout << "Press U to unpause game!\n";
    }
};

class GameScreen : public PushdownState {
public:
    PushdownResult OnUpdate(float dt, PushdownState** newState) override {
        pauseReminder -= dt;
        if (pauseReminder < 0.0f) {
            std::cout << "Coins mined: " << coinsMined << "\n";
            std::cout << "Press P to pause game, or F1 to return to main menu!\n";
            pauseReminder += 1.0f;
        }

        // P：进入暂停界面
        if (Window::GetKeyboard()->KeyDown(KeyCodes::P)) {
            *newState = new PauseScreen();
            return PushdownResult::Push;
        }

        // F1：回到主菜单（Pop 掉 GameScreen），回 IntroScreen
        if (Window::GetKeyboard()->KeyDown(KeyCodes::F1)) {
            std::cout << "Returning to main menu!\n";
            return PushdownResult::Pop;
        }

        if (rand() % 7 == 0) {
            coinsMined++;
        }

        return PushdownResult::NoChange;
    }

    void OnAwake() override {
        gGameStarted = true;   // 进入游戏
        gIsPaused = false;  // 确保不是暂停
        std::cout << "Preparing to mine coins!\n";
    }

protected:
    int   coinsMined = 0;
    float pauseReminder = 1.0f;
};

class IntroScreen : public PushdownState {
public:
    PushdownResult OnUpdate(float dt, PushdownState** newState) override {
        if (Window::GetKeyboard()->KeyPressed(KeyCodes::SPACE)) {
            *newState = new GameScreen();
            return PushdownResult::Push;  // 进入 GameScreen
        }
        if (Window::GetKeyboard()->KeyPressed(KeyCodes::ESCAPE)) {
            // Pop 掉 IntroScreen，本次 Update 返回 false，游戏主循环就会退出
            return PushdownResult::Pop;
        }
        return PushdownResult::NoChange;
    }

    void OnAwake() override {
        gGameStarted = false;  // 回到主菜单，游戏不再更新
        gIsPaused = false;
        std::cout << "Welcome to a really awesome game!\n";
        std::cout << "Press Space To Begin or Escape to quit!\n";
    }
};


int main() {
    WindowInitialisation initInfo;
    initInfo.width = 1280;
    initInfo.height = 720;
    initInfo.windowTitle = "CSC8503 Game technology!";

    Window* w = Window::CreateGameWindow(initInfo);

    if (!w->HasInitialised()) {
        return -1;
    }

    w->ShowOSPointer(false);
    w->LockMouseToWindow(true);

    GameWorld* world = new GameWorld();
    PhysicsSystem* physics = new PhysicsSystem(*world);

#ifdef USEVULKAN
    GameTechVulkanRenderer* renderer = new GameTechVulkanRenderer(*world);
#elif defined(USEOPENGL)
    GameTechRenderer* renderer = new GameTechRenderer(*world);
#endif

    TutorialGame* g = new TutorialGame(*world, *renderer, *physics);

    // ---- 状态机：从 IntroScreen 开始 ----
    PushdownMachine stateMachine(new IntroScreen());

    TestPathfinding();
    w->GetTimer().GetTimeDeltaSeconds(); // Clear the timer so we don't get a large first dt!

    bool keepRunning = true;

    while (w->UpdateWindow() && keepRunning && !Window::GetKeyboard()->KeyDown(KeyCodes::ESCAPE)) {
        float dt = w->GetTimer().GetTimeDeltaSeconds();

        if (dt > 0.1f) {
            std::cout << "Skipping large time delta" << std::endl;
            continue; // must have hit a breakpoint or something to have a 1 second frame time!
        }

        // 更新状态机（Intro / Game / Pause）
        if (!stateMachine.Update(dt)) {
            // 状态栈为空（比如在 Intro 屏按了 ESC），退出循环
            keepRunning = false;
            break;
        }

        if (Window::GetKeyboard()->KeyPressed(KeyCodes::PRIOR)) {
            w->ShowConsole(true);
        }
        if (Window::GetKeyboard()->KeyPressed(KeyCodes::NEXT)) {
            w->ShowConsole(false);
        }

        if (Window::GetKeyboard()->KeyPressed(KeyCodes::T)) {
            w->SetWindowPosition(0, 0);
        }

        w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

        // ======= 这里是真正的游戏逻辑 =======
        // 只有在 GameScreen 且没有暂停时才更新游戏 / 物理 / 寻路 / 行为树
        if (gGameStarted && !gIsPaused) {
            g->UpdateGame(dt);
            TestBehaviourTree();
            world->UpdateWorld(dt);
            physics->Update(dt);
            DisplayPathfinding();
        }

        // 渲染可以即使在暂停时也更新，让画面保持（一般 renderer 里只读状态，会很安全）
        renderer->Update(dt);
        renderer->Render();

        Debug::UpdateRenderables(dt);
    }

    Window::DestroyGameWindow();
    return 0;
}