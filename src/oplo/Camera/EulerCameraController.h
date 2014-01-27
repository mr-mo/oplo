#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include "Math/Quat.h"
#include "Math/Angle.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"

namespace oplo
{

	class Camera;
	class EulerCameraController
	{
	public:
		EulerCameraController();

		EulerCameraController(Vec3D const& pos);

		void setCamera(Camera* p);

		const Camera* getCamera() const;

		inline void setHeadingAbsolute(Anglef h) 
		{ 
			m_heading = h; 
			m_rotationDirty = true; 
		}

		inline void setPitchAbsolute(Anglef p) 
		{ 
			m_pitch = p; 
			m_rotationDirty = true; 
		}

		inline void setRollAbsolute(Anglef r) 
		{ 
			m_roll = r;  
			m_rotationDirty = true; 
		}

		inline void rotateHeadingDelta(float h) 
		{ 
			m_heading.setDegrees(m_heading.getDegrees() + h);
			m_rotationDirty = true; 
		}

		inline void rotatePitchDelta(float p) 
		{ 
			m_pitch.setDegrees(m_pitch.getDegrees() + p);
			m_rotationDirty = true; 
		}

		inline void rotateRollDelta(float r) 
		{ 
			m_roll.setDegrees(m_roll.getDegrees() + r);
			m_rotationDirty = true; 
		}

		inline void incrementPosition(Vec3D const& p) 
		{ 
			m_position += p; 
		}

		inline void setPosition(Vec3D const& p) 
		{ 
			m_position = p; 
		}

		void moveForward(float amount);

		void updateCamera();

	private:

		void updateRotation();

		Camera* m_controlledCamera;

		bool m_rotationDirty;

		Anglef m_heading, m_pitch, m_roll;

		Vec3D m_position;
		Quat<float> m_orientation;
	};

}

#endif
