#ifndef CAMERA_H
#define CAMERA_H

#include "Math/Vector.h"
#include "Math/Plane.h"
#include "Math/Matrix.h"
#include "Math/Angle.h"
#include "Math/Quat.h"

namespace oplo
{

	//This has too many responsibilities
	//Break it apart
	//Make stuff like aperture, etc. components?
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

		const Plane<double>& GetCullingPlane(int plane) const;

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

		//Todo: make me separable
		void SetAperture(float aperture);

		void SetFocalLength(float f);

		void SetFocusDistance(float d);

		float GetAperture() const;

		float GetFocalLength() const;

		float GetFocusDistance() const;

		void GetDepthOfFieldParams(float& cocScale, float& cocBias) const;

		void Update();

	private:

		void UpdateDepthOfField();

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

		float m_aperture;
		float m_focalLength;
		float m_planeInFocus;
		float m_cocScale;
		float m_cocBias;

		Angle<float> m_fov;
		Vec3D m_frustumPoints[8];
	};

}

#endif
