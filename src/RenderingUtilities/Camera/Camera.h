#ifndef CAMERA_H
#define CAMERA_H

#include "RenderingUtilities/Math/Vector.h"
#include "RenderingUtilities/Math/Plane.h"
#include "RenderingUtilities/Math/Matrix.h"
#include "RenderingUtilities/Math/Angle.h"
#include "RenderingUtilities/Math/Quat.h"

namespace oplo
{

	class Camera
	{
	public:

		enum
		{
			TOP = 0,
			BOTTOM,
			LEFT,
			RIGHT,
			NEARP,
			FARP,
			NUM_PLANES
		};

		enum
		{
			NTL = 0,
			NTR,
			NBL,
			NBR,
			FTL,
			FTR,
			FBL,
			FBR,
			NUM_CORNERS
		};

		enum ClippingResult
		{
			OUTSIDE = 0,
			INTERSECTS,
			INSIDE
		};

		Camera();

		Camera(const Camera& rhs);

		Camera& operator=(const Camera& rhs);

		ClippingResult ClipSphere(Vec3D const& worldPosition, double worldRadius) const;

		void SetViewingPosition(Vec3D const& position);

		void SetViewingRotation(Quat<float> const& viewOrientation);

		const Matrix4< float >& GetProjectionMatrix() const;

		const Matrix4< float >& GetViewMatrix() const;

		const Vec3D& GetWorldPosition() const;

		void SetViewport(int x, int y, int w, int h);

		void GetViewport(int& x, int& y, int& w, int& h);

		void GetInverseViewport(float& viewingWidth, float& viewingHeight) const;

		int GetViewingWidth() const;

		int GetViewingHeight() const;

		float GetViewingDiagonal() const;

		void SetFieldOfView(Anglef angle);

		void SetClippingRanges(float nearPlane, float farPlane);

		Anglef GetFieldOfView() const;

		float GetTangentFOV() const;

		float GetAspect() const;

		bool ViewDirty() const;

		bool ProjectionDirty() const;

		void Update();

	private:

		void UpdateClippingPlanes();

		void MakePerspectiveMatrix();

	private:

		Vec3D m_viewingPositionWorld;
		Quat<float> m_viewOrientation;

		Plane<double> m_cullingPlanes[6];

		bool m_projectionDirty;
		bool m_viewDirty;

		Matrix4<float> m_viewMatrix;
		Matrix4<float> m_projectionMatrix;

		float m_tanFOV;
		float m_aspectRatio;
		float m_nearPlane;
		float m_farPlane;
		float m_viewingDiagonal;

		int m_viewport[4];
		float m_inverseViewport[2];

		Angle<float> m_fov;
		Vec3D m_frustumPoints[8];
	};

}

#endif