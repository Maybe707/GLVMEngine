#ifndef ENGINE
#define ENGINE

#include "Components/DirectionalLightComponent.hpp"
#include "Components/SpotLightComponent.hpp"
#include "GLPointer.h"
#include "Systems/GUISystem.hpp"
#include "IChrono.hpp"
#include "IWindow.hpp"
#include "ISoundEngine.hpp"
#include "Systems/CameraSystem.hpp"
#include "Systems/PhysicsSystem.hpp"
#include "Systems/ProjectileSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "ShaderProgram.hpp"
#include "Stack.hpp"
#include "Event.hpp"
#include "VertexData.hpp"
#include "WindowCreator.hpp"
#include "TimerCreator.hpp"
#include "Components/MaterialComponent.hpp"
#include "VectorContainer.hpp"
#include "Systems/CollisionSystem.hpp"
#include "EntityManager.hpp"
#include "ComponentManager.hpp"
#include "Components/ColliderComponent.hpp"
#include "IWindow.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "VertexData.hpp"
#include "Systems/MovementSystem.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/AnimationMoveComponent.hpp"
#include "SystemManager.hpp"
#include "Components/ViewComponent.hpp"
#include "Components/EventComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Components/AnimationMoveComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "ComponentManager.hpp"
#include "Components/CrosshairComponent.hpp"
#include "Event.hpp"
#include "Systems/CollisionSystem.hpp"
#include "Systems/GUISystem.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/PointLightComponent.hpp"
#include "VectorContainer.hpp"
#include "Components/VertexComponent.hpp"
#include "IContainer.hpp"
#include <GL/gl.h>
#include <GL/glext.h>
#include "Constants.hpp"
#include <mutex>
#include "Systems/RenderSystem.hpp"
#include "TextureManager.hpp"

namespace GLVM::ECS::components
{    
	typedef transform        transform;
	typedef vertex           vertex;
	typedef move             movement;
	typedef collider         collider;
	typedef SAnimationMoveComponent    animation;
	typedef beholder             camera;
	typedef rigidBody        rigidBody;
	typedef event            event;
	typedef material         material;
	typedef SDirectionalLightComponent directionalLight;
	typedef pointLight       pointLight;
	typedef spotLight        spotLight;
}

using Entity = unsigned int;

namespace GLVM::Core
{
	class CEngine
	{
        static CEngine*    pInstance_;
        static std::mutex  Mutex_;
        
		Time::IChrono       * Chrono_;
        Sound::ISoundEngine * Sound_Engine_;

		float              fDelta_Time_;
		CStack             Input_Stack_;
        ECS::CRenderSystem * Render_System_Interface_;
        
        ECS::CCollisionSystem  * Collision_System;
		ECS::CMovementSystem   * Movement_System;
        ECS::CPhysicsSystem    * Physics_System_;
        ECS::CProjectileSystem * pProjectile_System_;
        ECS::CGUISystem        * GUI_System;

		/// For FPS counting
		unsigned int fpsCounter = 0;
		double fpsAccumulator   = 0;
		
        CEngine();
        
	public:

        
        ~CEngine();
        
        CEngine(CEngine& _engine) = delete;                   ///< Dont need to make cope because of singleton property.
        void operator=(const CEngine& _engine) = delete;      ///< Dont need assignment operator because of singleton property.
        static CEngine* GetInstance();                        ///< It possibly to get only one instance of this class whith this method
        
		void GameLoop();
		void FPScounter();
		void GameKill();
//        void PlaybackSound(Core::CSoundEngine& _sound_Engine);
	};
		
}

#endif
