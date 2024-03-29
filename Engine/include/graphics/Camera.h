	#include "Components/Transform.h"
	enum class Direction
	{
		FORWARD,
		BACKWARDS,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	struct CameraData
	{
		CameraData() = default;
		CameraData(glm::mat4 const& view, glm::vec3 const& position);
		float ViewProjectMatrix[4][4];
		float Eye[4];
	};

	class Camera
	{
	public:
	    Camera() = default;
	    Camera(const Transform& inTransform, float inAspectRatio, float inFov);

	    glm::mat4 GetView() const { return m_view; }
	    glm::mat4 GetProjection() const { return m_projection; }
	    glm::mat4 GetViewProjection() const { return m_projection * m_view; }

	    void UpdateView();
	    void UpdateProjection(float inAspectRatio);

	    glm::vec3 GetUp() const { return m_objectTransform.GetUpVector(); }
	    glm::vec3 GetForward() const { return m_objectTransform.GetForwardVector(); }
	    glm::vec3 GetRight() const { return m_objectTransform.GetRightVector(); }

	    void UpdateDirections();

	    Transform GetTransform() const { return m_objectTransform; }
	    inline void SetTransform(const Transform& inTransform) { m_objectTransform = inTransform; }

	    float GetFovDegrees() const { return m_fovDegrees; }
	    inline void SetFovDegrees(const float inFov) { m_fovDegrees = inFov; }

	    float GetNearPlane() const { return m_near; }
	    inline void SetNearPlane(const float inNear) { m_near = inNear; }

	    float GetFarPlane() const { return m_far; }
		inline void SetFarPlane(const float inFar) { m_far = inFar; }

		float& GetSensitivity() { return m_sensitivity; }
		float& GetMovementSpeed() { return m_movementSpeed; }

		inline void SetAspect(const float inAspect) { m_aspectRatio = inAspect; }
		inline float GetAspectRatio() const { return m_aspectRatio; };
		
		void ProcessMouseMovement(float inOffsetX, float inOffsetY);
		void ProcessKeyMovement(Direction inDirection, const float inDeltaTime);
	private:
	    float m_yaw = 0.0f;
	    float m_pitch = 0.0f;

	    glm::mat4 m_projection;
	    glm::mat4 m_view;

	    Transform m_objectTransform;

	    float m_fovDegrees = 80.f;
	    float m_near = 0.1f;
	    float m_far = 100.0f;
	    float m_aspectRatio = 1.f;
		float m_sensitivity = 0.003f;
		float m_movementSpeed = 8.5f;
	};
