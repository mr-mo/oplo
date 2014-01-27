#include "EulerCameraController.h"
#include "Camera.h"

namespace oplo
{
	EulerCameraController::EulerCameraController() :
		m_heading(0), m_pitch(0), m_roll(0),
		m_position(0), m_controlledCamera(0)
	{}

	EulerCameraController::EulerCameraController(Vec3D const& pos) : m_position(pos),
		m_heading(0), m_pitch(0), m_roll(0),
		m_controlledCamera(0)
	{}

	void EulerCameraController::setCamera(Camera* p)
	{
		m_controlledCamera = p;
	}

	const Camera* EulerCameraController::getCamera() const
	{
		return m_controlledCamera;
	}

	void EulerCameraController::moveForward(float amount)
	{
		updateRotation();
		Vec3F viewAxis = ViewVector(m_orientation) * amount;
		m_position += viewAxis.cast<double>();
	}

	void EulerCameraController::updateCamera()
	{
		updateRotation();

		m_controlledCamera->SetViewingPosition(m_position);
		m_controlledCamera->SetViewingRotation(m_orientation);
		m_controlledCamera->Update();
	}

	void EulerCameraController::updateRotation()
	{
		if (m_rotationDirty)
		{
			Quat<float> hdg, pitch, roll;
			hdg.fromAxisAngle(m_heading, Vec3F(0, 1, 0));
			pitch.fromAxisAngle(m_pitch, Vec3F(1, 0, 0));
			roll.fromAxisAngle(m_roll, Vec3F(0, 0, 1));
			m_orientation = hdg * pitch * roll;
			m_orientation.normalize();
		}

		m_rotationDirty = false;
	}

}