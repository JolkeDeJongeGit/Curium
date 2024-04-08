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

		// Check if a point is inside the frustum
		bool isInside(const glm::vec3& min ,const glm::vec3& max) const {
			for (const auto& plane : m_planes) {
				glm::vec3 pVertex = glm::vec3(
					plane.normal.x > 0 ? max.x : min.x,
					plane.normal.y > 0 ? max.y : min.y,
					plane.normal.z > 0 ? max.z : min.z
				);

				if (glm::dot(plane.normal, pVertex) + plane.distance <= 0) {
					return false;
				}
			}
			return true;
		}

		void Update(const glm::mat4& viewProjectionMatrix) {
			// Extract frustum planes from the view-projection matrix

			glm::mat4 vp = viewProjectionMatrix;
			m_planes[0].normal.x = vp[0][3] + vp[0][0];
			m_planes[0].normal.y = vp[1][3] + vp[1][0];
			m_planes[0].normal.z = vp[2][3] + vp[2][0];
			m_planes[0].distance = vp[3][3] + vp[3][0];

			// Right clipping plane
			m_planes[1].normal.x = vp[0][3] - vp[0][0];
			m_planes[1].normal.y = vp[1][3] - vp[1][0];
			m_planes[1].normal.z = vp[2][3] - vp[2][0];
			m_planes[1].distance = vp[3][3] - vp[3][0];

			// Top clipping plane
			m_planes[2].normal.x = vp[0][3] - vp[0][1];
			m_planes[2].normal.y = vp[1][3] - vp[1][1];
			m_planes[2].normal.z = vp[2][3] - vp[2][1];
			m_planes[2].distance = vp[3][3] - vp[3][1];

			// Bottom clipping plane
			m_planes[3].normal.x = vp[0][3] + vp[0][1];
			m_planes[3].normal.y = vp[1][3] + vp[1][1];
			m_planes[3].normal.z = vp[2][3] + vp[2][1];
			m_planes[3].distance = vp[3][3] + vp[3][1];

			// Near clipping plane
			m_planes[4].normal.x = vp[0][3] + vp[0][2];
			m_planes[4].normal.y = vp[1][3] + vp[1][2];
			m_planes[4].normal.z = vp[2][3] + vp[2][2];
			m_planes[4].distance = vp[3][3] + vp[3][2];

			// Far clipping plane
			m_planes[5].normal.x = vp[0][3] - vp[0][2];
			m_planes[5].normal.y = vp[1][3] - vp[1][2];
			m_planes[5].normal.z = vp[2][3] - vp[2][2];
			m_planes[5].distance = vp[3][3] - vp[3][2];


			for (int i = 0; i < 6; i++) {
				float mag = 1.f / glm::length(m_planes[i].normal);
				m_planes[i].normal = m_planes[i].normal * mag;
				m_planes[i].distance = m_planes[i].distance * mag;
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
	    float m_far = 5000000.0f;
	    float m_aspectRatio = 1.f;
		float m_sensitivity = 0.003f;
		float m_movementSpeed = 400000.f;
	};
