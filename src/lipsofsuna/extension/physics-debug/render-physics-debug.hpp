#ifndef __RENDER_INTERNAL_RENDER_PHYSICS_DEBUG_HPP__
#define __RENDER_INTERNAL_RENDER_PHYSICS_DEBUG_HPP__

/**
The code in the file is based upon the code here:
http://www.ogre3d.org/tikiwiki/BulletDebugDrawer&structure=Cookbook
and is in the public domain.
 */

#include <btBulletCollisionCommon.h>
#include <OgreFrameListener.h>
#include <OgreSceneManager.h>
#include <OgreManualObject.h>

class OgreDebugDrawer: public btIDebugDraw, public Ogre::FrameListener{
public:
	OgreDebugDrawer( Ogre::SceneManager *scm );
	~OgreDebugDrawer ();
	virtual void     drawLine (const btVector3 &from, const btVector3 &to, const btVector3 &color);
	virtual void     drawTriangle (const btVector3 &v0, const btVector3 &v1, const btVector3 &v2, const btVector3 &color, btScalar);
	virtual void     drawContactPoint (const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color);
	virtual void     reportErrorWarning (const char *warningString);
	virtual void     draw3dText (const btVector3 &location, const char *textString);
	virtual void     setDebugMode (int debugMode);
	virtual int     getDebugMode () const;
protected:
	bool frameStarted(const Ogre::FrameEvent& evt);
	bool frameEnded(const Ogre::FrameEvent& evt);
private:
	struct ContactPoint{
		Ogre::Vector3 from;
		Ogre::Vector3 to;
		Ogre::ColourValue   color;
		size_t        dieTime;
	};
	DebugDrawModes               mDebugModes;
	Ogre::ManualObject          *mLines;
	Ogre::ManualObject          *mTriangles;
	std::vector< ContactPoint > *mContactPoints;
	std::vector< ContactPoint >  mContactPoints1;
	std::vector< ContactPoint >  mContactPoints2;

	Ogre::SceneNode* mNode;
};

#endif /*__RENDER_INTERNAL_RENDER_PHYSICS_DEBUG_HPP__*/
