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

	struct Plane {
		glm::vec3 normal;
		float distance;
	};

	class Frustum {
	private:
		std::vector<Plane> m_planes;

	public:
		Frustum() {
			m_planes.resize(6); // Frustum typically has 6 planes

			// Initialize frustum planes
			for (int i = 0; i < 6; ++i) {
				m_planes[i] = { glm::vec3(0.0f), 0.0f };
			}
		}

		void Update(const glm::mat4& viewProjectionMatrix) {
			// Extract frustum planes from the view-projection matrix

			glm::mat4 viewProjTranspose = glm::transpose(viewProjectionMatrix);

			// Right plane
			m_planes[0] = {
				glm::vec3(viewProjTranspose[3][0] - viewProjTranspose[0][0],
						  viewProjTranspose[3][1] - viewProjTranspose[0][1],
						  viewProjTranspose[3][2] - viewProjTranspose[0][2]),
				viewProjTranspose[3][3] - viewProjTranspose[0][3]
				};

			// Left plane
			m_planes[1] = {
				glm::vec3(viewProjTranspose[3][0] + viewProjTranspose[0][0],
						  viewProjTranspose[3][1] + viewProjTranspose[0][1],
						  viewProjTranspose[3][2] + viewProjTranspose[0][2]),
				viewProjTranspose[3][3] + viewProjTranspose[0][3]
				};

			// Bottom plane
			m_planes[2] = {
				glm::vec3(viewProjTranspose[3][0] + viewProjTranspose[1][0],
						  viewProjTranspose[3][1] + viewProjTranspose[1][1],
						  viewProjTranspose[3][2] + viewProjTranspose[1][2]),
				viewProjTranspose[3][3] + viewProjTranspose[1][3]
				};

			// Top plane
			m_planes[3] = {
				glm::vec3(viewProjTranspose[3][0] - viewProjTranspose[1][0],
						  viewProjTranspose[3][1] - viewProjTranspose[1][1],
						  viewProjTranspose[3][2] - viewProjTranspose[1][2]),
				viewProjTranspose[3][3] - viewProjTranspose[1][3]
				};

			// Far plane
			m_planes[4] = {
				glm::vec3(viewProjTranspose[3][0] - viewProjTranspose[2][0],
						  viewProjTranspose[3][1] - viewProjTranspose[2][1],
						  viewProjTranspose[3][2] - viewProjTranspose[2][2]),
				viewProjTranspose[3][3] - viewProjTranspose[2][3]
				};

			// Near plane
			m_planes[5] = {
				glm::vec3(viewProjTranspose[3][0] + viewProjTranspose[2][0],
						  viewProjTranspose[3][1] + viewProjTranspose[2][1],
						  viewProjTranspose[3][2] + viewProjTranspose[2][2]),
				viewProjTranspose[3][3] + viewProjTranspose[2][3]
				};

			// Normalize the planes
			for (auto& plane : m_planes) {
				float length = 1.f / glm::length(plane.normal);
				plane.normal *= length;
				plane.distance *= length;
			}
		}

		const std::vector<Plane>& GetPlanes() const {
			return m_planes;
		}
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

		Frustum GetFrustum() const { return m_frustum; }
		inline void SetTransform(const Frustum& inFrustrum) { m_frustum = inFrustrum; }

	    float GetFovDegrees() const { return m_fovDegrees; }
	    inline void SetFovDegrees(const float inFov) { m_fovDegrees = inFov; }

	    float GetNearPlane() const { return m_near; }
	    inline void SetNearPlane(const float inNear) { m_near = inNear; }

	    float GetFarPlane() const { return m_far; }
		inline void SetFarPlane(const float inFar) { m_far = inFar; }

		float& GetSensitivity() { return m_sensitivity; }

		float& GetMovementSpeed() { return m_movementSpeed; }
		void SetMovementSpeed(const float inSpeed) { m_movementSpeed = inSpeed > 8.5f ? inSpeed : 8.5f; }

		inline void SetAspect(const float inAspect) { m_aspectRatio = inAspect; }
		inline float GetAspectRatio() const { return m_aspectRatio; };
		
		void ProcessMouseMovement(float inOffsetX, float inOffsetY);
		void ProcessKeyMovement(Direction inDirection, const float inDeltaTime);

		bool m_updateFrustum = false;
	private:
	    float m_yaw = 0.0f;
	    float m_pitch = 0.0f;

	    glm::mat4 m_projection;
	    glm::mat4 m_view;

		Frustum m_frustum;

	    Transform m_objectTransform;

	    float m_fovDegrees = 80.f;
	    float m_near = 0.1f;
	    float m_far = 500000.0f;
	    float m_aspectRatio = 1.f;
		float m_sensitivity = 0.003f;
		float m_movementSpeed = 15000.f;
	};
