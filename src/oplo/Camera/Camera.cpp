#include "Camera.h"

#include "Rendering/oploGL.h"

namespace oplo
{

	Camera::Camera()
	{

	}

	Camera::Camera(const Camera& rhs)
	{

	}

	Camera::ClippingResult Camera::ClipSphere(Vec3D const& worldPosition, double worldRadius) const
	{
		double d = 0.0;
		ClippingResult r = INSIDE;

		for (unsigned i = 0; i < NUM_PLANES; ++i)
		{
			d = m_cullingPlanes[i].distance(worldPosition);

			if (d < -worldRadius)
			{
				return OUTSIDE;
			}
			else if (d < worldRadius)
			{
				r = INTERSECTS;
			}
		}

		return r;
	}

	void Camera::SetViewingPosition(Vec3D const& worldPosition)
	{
		m_viewingPositionWorld = worldPosition;
		m_viewDirty = true;
	}

	void Camera::SetViewingRotation(Quat<float> const& viewOrientation)
	{
		m_viewOrientation = viewOrientation;
		m_viewDirty = true;
	}

	const Matrix4< float >& Camera::GetProjectionMatrix() const
	{
		return m_projectionMatrix;
	}

	const Matrix4< float >& Camera::GetViewMatrix() const
	{
		return m_viewMatrix;
	}

	const Vec3D& Camera::GetWorldPosition() const
	{
		return m_viewingPositionWorld;
	}

	const Plane<double>& Camera::GetCullingPlane(int plane) const
	{
		return m_cullingPlanes[plane];
	}

	void Camera::SetViewport(int x, int y, int w, int h)
	{
		m_viewport[0] = x;
		m_viewport[1] = y;
		m_viewport[2] = w;
		m_viewport[3] = h;
		m_inverseViewport[0] = w / 1.f;
		m_inverseViewport[1] = h / 1.f;
		m_aspectRatio = w / (float)h;
		m_projectionDirty = true;
		m_viewingDiagonal = sqrt(float(w * w + h * h));

	}

	void Camera::GetViewport(int& x, int& y, int& w, int& h)
	{
		x = m_viewport[0];
		y = m_viewport[1];
		w = m_viewport[2];
		h = m_viewport[3];
	}

	void Camera::GetInverseViewport(float& viewingWidth, float& viewingHeight) const
	{
		viewingWidth = m_inverseViewport[0];
		viewingHeight = m_inverseViewport[1];
	}

	int Camera::GetViewingWidth() const
	{
		return m_viewport[2];
	}

	int Camera::GetViewingHeight() const
	{
		return m_viewport[3];
	}

	float Camera::GetViewingDiagonal() const
	{
		return m_viewingDiagonal;
	}

	void Camera::SetFieldOfView(Anglef angle)
	{
		m_fov = angle;
		m_tanFOV = tan(angle.getRadians() * 0.5f);
		m_projectionDirty = true;
	}

	void Camera::SetClippingRanges(float nearPlane, float farPlane)
	{
		m_nearPlane = nearPlane;
		m_farPlane = farPlane;
		m_projectionDirty = true;
	}

	Anglef Camera::GetFieldOfView() const
	{
		return m_fov;
	}

	float Camera::GetTangentFOV() const
	{
		return m_tanFOV;
	}

	float Camera::GetAspect() const
	{
		return m_aspectRatio;
	}

	bool Camera::ViewDirty() const
	{
		return m_viewDirty;
	}

	bool Camera::ProjectionDirty() const
	{
		return m_projectionDirty;
	}

	void Camera::SetAperture(float a)
	{
		m_aperture = a;
		UpdateDepthOfField();
	}

	void Camera::SetFocalLength(float f)
	{
		m_focalLength = f;
		UpdateDepthOfField();
	}

	void Camera::SetFocusDistance(float d)
	{
		m_planeInFocus = d;
		UpdateDepthOfField();
	}

	float Camera::GetAperture() const
	{
		return m_aperture;
	}

	float Camera::GetFocalLength() const
	{
		return m_focalLength;
	}

	float Camera::GetFocusDistance() const
	{
		return m_planeInFocus;
	}

	void Camera::GetDepthOfFieldParams(float& cocScale, float& cocBias) const
	{
		cocScale = m_cocScale ;
		cocBias = m_cocBias;
	}

	void Camera::Update()
	{
		if (m_projectionDirty)
		{
			MakePerspectiveMatrix();
			UpdateDepthOfField();
		}

		if (m_viewDirty || m_projectionDirty)
		{
			m_viewOrientation.getMatrix(m_viewMatrix);

			UpdateClippingPlanes();

			m_viewMatrix[12] = static_cast<float>(m_viewingPositionWorld[0]);
			m_viewMatrix[13] = static_cast<float>(m_viewingPositionWorld[1]);
			m_viewMatrix[14] = static_cast<float>(m_viewingPositionWorld[2]);

			m_viewMatrix.invert();
		}

		m_viewDirty = false;
		m_projectionDirty = false;
	}

	void Camera::UpdateDepthOfField()
	{
		m_cocScale = (m_aperture * m_focalLength * m_planeInFocus * (m_farPlane - m_nearPlane)) / ((m_planeInFocus - m_focalLength) * m_nearPlane * m_farPlane);
		m_cocBias = (m_aperture * m_focalLength * (m_nearPlane - m_planeInFocus)) / ((m_planeInFocus * m_focalLength) * m_nearPlane);
	}

	void Camera::UpdateClippingPlanes()
	{
		Vec3D p = m_viewingPositionWorld;

		//transpose of the normal matrix should do it
		Vec3D r = Vec3D(m_viewMatrix[0], m_viewMatrix[1], m_viewMatrix[2]);
		Vec3D u = Vec3D(m_viewMatrix[4], m_viewMatrix[5], m_viewMatrix[6]);
		Vec3D f = Vec3D(m_viewMatrix[8], m_viewMatrix[9], m_viewMatrix[10]);

		//m_basis[0] = r;
		//m_basis[1] = u;
		//m_basis[2] = -f;

		double m_hn = m_nearPlane * m_tanFOV;
		double m_wn = m_nearPlane * m_tanFOV * m_aspectRatio;
		double m_hf = m_farPlane * m_tanFOV;
		double m_wf = m_farPlane * m_tanFOV * m_aspectRatio;

		m_frustumPoints[NTL] = p + u * m_hn + r * -m_wn - f * double(m_nearPlane);
		m_frustumPoints[NTR] = p + u * m_hn + r *  m_wn - f * double(m_nearPlane);

		m_frustumPoints[NBL] = p + u * -m_hn + r * -m_wn - f * double(m_nearPlane);
		m_frustumPoints[NBR] = p + u * -m_hn + r *  m_wn - f * double(m_nearPlane);

		m_frustumPoints[FTL] = p + u * m_hf + r * -m_wf - f * double(m_farPlane);
		m_frustumPoints[FTR] = p + u * m_hf + r *  m_wf - f * double(m_farPlane);

		m_frustumPoints[FBL] = p + u * -m_hf + r * -m_wf - f * double(m_farPlane);
		m_frustumPoints[FBR] = p + u * -m_hf + r *  m_wf - f * double(m_farPlane);

		m_cullingPlanes[NEARP].set(m_frustumPoints[NTL], m_frustumPoints[NTR], m_frustumPoints[NBR]);
		m_cullingPlanes[FARP].set(m_frustumPoints[FBR], m_frustumPoints[FTR], m_frustumPoints[FTL]);

		m_cullingPlanes[TOP].set(m_frustumPoints[FTR], m_frustumPoints[NTR], m_frustumPoints[NTL]);
		m_cullingPlanes[BOTTOM].set(m_frustumPoints[FBL], m_frustumPoints[NBL], m_frustumPoints[NBR]);

		m_cullingPlanes[LEFT].set(m_frustumPoints[NTL], m_frustumPoints[NBL], m_frustumPoints[FBL]);
		m_cullingPlanes[RIGHT].set(m_frustumPoints[FBR], m_frustumPoints[NBR], m_frustumPoints[NTR]);
	}

	void Camera::MakePerspectiveMatrix()
	{
		float f = 1.f / m_tanFOV;
		m_projectionMatrix.setZero();

		m_projectionMatrix[0] = (float)f / (float)m_aspectRatio;
		m_projectionMatrix[5] = (float)f;
		m_projectionMatrix[10] = (m_farPlane + m_nearPlane) / (m_nearPlane - m_farPlane);
		m_projectionMatrix[11] = -1;
		m_projectionMatrix[14] = (2 * m_farPlane * m_nearPlane) / (m_nearPlane - m_farPlane);
		m_projectionDirty = false;
	}

}
