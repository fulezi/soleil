#include <OSGUtility/MathExtension.h>
#include "GameManipulator.h"
#include <OSGUtility/OSGUtility.h>

#include <stdlib.h>
#include <osgUtil.h>
#include <fstream>

// For text
#include <osg.h>
#include <osgText.h>
#include <osgText/Font>
#include <osgText/Text>

#include "Visitors.h"
#include "NodeMaskValues.h"

using namespace osg;
using namespace osgGA;

class DeleteSelectedNodesVisitor : public osg::NodeVisitor
{
public:

    DeleteSelectedNodesVisitor():
        osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)        
    {
    }
    
    virtual void apply(osg::Node& node)
    {
        osg::Group* scribe = dynamic_cast<osg::Group*>( &node );
        if( scribe && scribe->getName() == "bookmark" )
        {
            _selectedNodes.push_back( scribe );
        }
        else
        {
            traverse( node );
        }
    }
    
    void pruneSelectedNodes()
    {
        for(SelectedNodes::iterator itr = _selectedNodes.begin();
            itr != _selectedNodes.end();
            ++itr)
        {
            osg::Node* node = itr->get();
            osg::Node::ParentList parents = node->getParents();
            for(osg::Node::ParentList::iterator pitr = parents.begin();
                pitr != parents.end();
                ++pitr)
            {
                osg::Group* parent = *pitr;
                parent->removeChild(node);
            }
        }
    }
    
    typedef std::vector< osg::ref_ptr<osg::Group> > SelectedNodes;
    SelectedNodes _selectedNodes;
    
};

osg::Group* create3DText(const osg::Vec3& pos, const osg::Quat& rot, const std::string textToDisplay )
{
    osg::Group* rootNode = 0;
    rootNode = new osg::Group;
    rootNode->setName( "bookmark" );

    osgText::Font* font = osgText::readFontFile("fonts/arial.ttf");

    osg::Geode* geode  = new osg::Geode;
    rootNode->addChild(geode);

////////////////////////////////////////////////////////////////////////////////////////////////////////
//    
// Examples of how to set up different text layout
//

    osg::Vec4 layoutColor(1.0f,1.0f,0.0f,1.0f);
    float layoutCharacterSize = 10.0f;    
    
    {
        osgText::Text* text = new osgText::Text;
        text->setFont(font);
        text->setColor(layoutColor);
        text->setCharacterSize(layoutCharacterSize);
        text->setPosition( pos );
        text->setRotation( rot );

        // the default layout is left to right, typically used in languages
        // originating from europe such as English, French, German, Spanish etc..
        text->setLayout( osgText::Text::LEFT_TO_RIGHT );

        text->setText( textToDisplay );
        geode->addDrawable(text);
    }
    return rootNode;    
}


namespace Kst 
{	
// ============================================================================
/*!
 * \class GameManipulator GameManipulator.h <kstGameManipulator/GameManipulator.h>
 * \ingroup Stealth
 *
 * \brief Camera manipulator that mimic 'standard' computer-game keyboard/mouse control.
 *
 * This manipulator is perfect for flying around in a terrain. It does not work
 * well if you have a small object as your scene. If you want to view a small model
 * then I suggest you use the track-ball manipulator instead.
 *
 * This manipulator is used by the stealth system as the control system on the server
 * side. 
 * 
 * \par Steering system:
 *      Default movement mode is free-flight (no clamping to ground). <br>
 *      Movement speed increases dynamically with your altitude above ground.
 *      Traveling far distances is thus faster at higher altitudes. <br>
 *      There is no collision checking (except for clamping you to ground).
 *      The clamp to ground mode does not allow you to walk into a location
 *      where there is no ground. An altitude of more than 10000 meters is
 *      currently considered an area without ground. <br>
 *      Clamp to ground, does not care what is ground, you get clamped to
 *      any geometry below you.
 *      
 * \par Input modes:
 *      - Normal mode     : Default. Mouse does not affect view direction.
 *      - Mouse-View mode : Mouse pointer is hidden. Mouse movement controls view direction.
 *
 * <br> <b> Normal mode: </b>
 * <pre>
 *        W       = Move forward.
 *      A S D     = (A)=Rotate left, (S)=Move backward, (D)=Rotate right. <br>
 *        E       = Warp to mouse collision location. <br>
 *        G       = Toggle between clamp to ground mode and flying mode. <br>
 *    ALT+B+any   = Bookmark current location and rotation. (any = any-key)
 *        B+any   = Warp to bookmarked location. <br>
 *      SPACE     = Warp to home location. <br>
 *    Right-Mouse = Enter 'Mouse-View' mode (hidden mouse). <br>
 *      UP-KEY    = Increase movement speed.
 *    DOWN-KEY    = Decrease movement speed.
 * </pre>
 * <b> Mouse-View mode: </b>
 * <pre>
 *        W         = Move forward.
 *      A S D       = (A) =<b> Strafe left </b>, (S) = Move backward, (D) =<b> Strafe right </b>. <br>
 *    mouse-move    = <b> Look in any direction. </b> <br>
 *        E         = Warp to mouse collision location. <br>
 *        G         = Toggle between clamp to ground mode and flying mode. <br>
 *    ALT+B+any     = Bookmark current location and rotation.
 *        B+any     = Warp to bookmarked location. <br>
 *      SPACE       = Warp to home location. <br>
 *    Right-Mouse   = <b> Leave </b> 'Mouse-View' mode (<b> visible </b> mouse ).
 *  Left-Mouse-Hold = <b> Fine tune, all movement is slowed down </b>. <br>
 *  F + mouse-move  = <b> Change field of view (FOV) </b>.
 *  C + mouse-move  = <b> Change clip-range </b>. <br>
 *      UP-KEY      = Increase movement speed.
 *    DOWN-KEY      = Decrease movement speed.
 * </pre>
 *  
 */




// ----------------------------------------------------------------------------
/*!
 * \par Actions:
 *      - Constructs the %GameManipulator object.
 *      - Sets all attributes to default values.
 * \par Interfacing:
 *      - Reads the bookmark-file if one is present.
 */
GameManipulator::GameManipulator( osg::ArgumentParser* arguments )
: CameraManipulatorBase()
, _ga_t1			( NULL )
, _ga_t0			( NULL )
, _node				( NULL )
, _eye				( osg::Vec3() )
, _angleY			( 0.0 )
, _angleX			( 0.0 )
, _rotation			( osg::Quat() )
, _homeAngleY		( 0.0 )
, _homeAngleX		( 0.0 )
, _velocity			( 75.0f )
, _speedUp			( false )
, _speedDown		( false )
, _moveForward		( false )
, _moveBackward		( false )
, _moveRight		( false )
, _moveLeft			( false )
, _forwardDirection	( osg::Vec3( 0.0, 0.0, 1.0 ) )
, _rightDirection	( osg::Vec3( 1.0, 0.0, 0.0 ) )
, _upDirection      ( osg::Vec3( 0.0, 0.0, 1.0 ) )
, _zeroAltitude		( 0.0 )
, _instantMove		( false )
, _instantMoveLoc	( osg::Vec3() )
, _warpTrigger		( false )
, _collisionPoint	( osg::Vec3() )
, _collisionNormal	( osg::Vec3( 0.0, 0.0, 1.0 ) )
, _altKey			( false )
, _bookmarkKey		( false )
, _bookmarkValue	( 0 )
, _bookmarkTrigger  ( false )
, _bookmarkFilename ( "" )
, _mouseView		( false )
, _fovKey           ( false )
, _clipRangeKey     ( false )
//, _clampToGround    ( false )
, _triPod           ( NULL )
, _numBookmarks     ( 0 )
, _currentBookmark  ( -1 )
, _lastBookmarkSeen ( false )
, _currentText      ( NULL )
{
    if( arguments )
    {
	    arguments->getApplicationUsage()->addCommandLineOption( "-bookmarkFile <Path+Filename>"
                                                              , "Path for bookmark file."
                                                              );	
  	    while( arguments->read( "-bookmarkFile", _bookmarkFilename ));
    }

	// Read bookmark file if it exists
    if( _bookmarkFilename.length() )
    {
	    std::ifstream is( _bookmarkFilename.c_str(), std::ios_base::binary );
        char zBuf[10240];
	    if( is.good() )
	    {
            is.read( zBuf, 10240 );
            char* c = zBuf;
            while( 1 )
            {
                std::string text = c;
                int i = text.find( "TextStart:\n", 0 );
                if( i == -1 )
                {
                    break;
                }
                Bookmark_t* b = new Bookmark_t;
                int key;
                sscanf( c, "Bookmark: %d %d %f %f %f %f %f", &(b->_id), &key, &(b->_pos._v[0]), &(b->_pos._v[1]), &(b->_pos._v[2]), &(b->_rx), &(b->_ry) );
                c += i + 11;
                text = c;
                i = text.find( "\nTextEnd\n", 0 );
                if( i == -1 )
                {
                    break;
                }
                c[i] = '\0';
                b->_text = strdup(c);
                c += i + 9;
                _bookmarks[key] = b;
                _numBookmarks++;
            }
	    }
    }
}

// ----------------------------------------------------------------------------
GameManipulator::~GameManipulator()
{
    if( !!_node )
    {
        _node.release();
        _node = NULL;
    }
    if( !!_ga_t1 )
    {
        _ga_t1.release();
        _ga_t1 = NULL;
    }
    if( !!_ga_t0 )
    {
        _ga_t0.release();
        _ga_t0 = NULL;
    }
    if( !!_triPod )
    {
        delete _triPod;
    }
}

// ----------------------------------------------------------------------------
/*!
 * \param node : New node that we shall focus on.
 * \par Actions:
 *      - Calculates initial velocity.
 *      - Calculates new home position.
 */
void GameManipulator::setNode( osg::Node* node )
{
    _node = node;
	computeInitVelocity();
    if( getAutoComputeHomePosition() ) 
	{
		computeHomePosition();
	}
}


// ----------------------------------------------------------------------------
/*!
* \return The focus node.
*/
const osg::Node* GameManipulator::getNode() const
{
    return _node.get();
}


// ----------------------------------------------------------------------------
/*!
* \return The focus node.
*/
osg::Node* GameManipulator::getNode()
{
    return _node.get();
}

// ----------------------------------------------------------------------------
/*!
 * \par Actions:
 *      - Uses the focus node's bounding sphere to calculate the initial velocity.
 *      - If the new velocity is too small then we clamp it to a minimum value.
 *      - If the current velocity is greater than the new init velocity, then we
 *        clamp the current velocity to be maximum the new velocity.
 */
void GameManipulator::computeInitVelocity()
{
    if( _node.get() )
    {
        const osg::BoundingSphere& boundingSphere = _node->getBound();
		double radius = boundingSphere._radius;

		double newVelocity = radius / 2.0;
		if( newVelocity < 5.0 )
		{
			newVelocity = 5.0;
		}
		if( newVelocity < _velocity )
		{
			_velocity = newVelocity;
		}
	}
}

// ----------------------------------------------------------------------------
/*!
 * \par Actions:
 *      - Calculates the 'home' position. 
 *
 * Home-position is the default location and rotation for the camera.
 * 
 * For large objects (terrains): Position is in the sky looking down. <br>
 * For smaller objects (models): Position is at the side looking toward center.
 */
void GameManipulator::computeHomePosition()
{
	// Default home position
	_homeAngleX = 0.0;
	_homeAngleY = 0.0;
	_homeEye = osg::Vec3( 0.0, 0.0, 200.0 );

    if( _node.get() )
    {
		// Read node's bounding sphere
        const osg::BoundingSphere& boundingSphere = _node->getBound();
        osg::Vec3d ep = boundingSphere._center;
		double radius = boundingSphere._radius;

		if( radius > 500.0 )
		{
			// Large object, we want to see this from above
			_homeEye	= ep + osg::Vec3( 0.0, 0.0, radius/1.5 );
			_homeAngleY	= -M_PI/2.0;
		}
		else
		{
			// Small object, we want to see this from the side
			_homeEye	  = osg::Vec3( 0.0, -radius*2.0, 0.0 );
			_homeAngleY   = 0.0;
			_homeAngleX   = 0.0;
		}

		// Set the zero altitude constant
		_zeroAltitude = ep[2];
	}
}

// ----------------------------------------------------------------------------
/*!
 * \param ea : Keyboard and mouse events.
 * \param us : Interface to the GUI system.
 * \par Actions:
 *      - Set camera position and rotation to 'home' position.
 */
void GameManipulator::home( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
    ea;
	// Use 'instant' move to get home
	_instantMove    = true;
	_instantMoveLoc = _homeEye;
	_angleX = _homeAngleX;
	_angleY = _homeAngleY;

    us.requestRedraw();
    us.requestContinuousUpdate( false );

    flushMouseEventStack();
}

// ----------------------------------------------------------------------------
/*!
 * \param ea : Keyboard and mouse events.
 * \param us : Interface to the GUI system.
 * \par Actions:
 *      - Gets the OSG view.
 *      - Creates the tri-pod for ground clamping.
 *      - Remembers camera FOV, aspect, near and far clip.
 *      - Turns on static near & far clip mode.
 *      - Empties the event-stack.
 */
void GameManipulator::init( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
    ea;
    osgViewer::View* view = dynamic_cast<osgViewer::View*>(us.asView());
    if( view )
    {
        if( !_triPod )
        {
            _triPod = new Pod( view->getSceneData(), 3 );
        }
    }
    flushMouseEventStack();
}

// ----------------------------------------------------------------------------
/*!
 * \par Actions:
 *      - Hides the mouse-pointer.
 *
 * This is called at right mouse button press-down event. The mouse does however
 * not disappear until the user have released the button. I have not found a way
 * to ensure that the mouse cursor disappears immediately
 */
void GameManipulator::hideMouse( osgViewer::View& view )
{
	// switch off the cursor
    osgViewer::Viewer::Windows windows;
    view.getViewerBase()->getWindows(windows);
    for(osgViewer::Viewer::Windows::iterator itr = windows.begin();
	    itr != windows.end();
	    ++itr)
    {
	    (*itr)->useCursor( false );
    }
}

// ----------------------------------------------------------------------------
/*!
 * \par Actions:
 *      - Makes the mouse-pointer visible.
 */
void GameManipulator::showMouse( osgViewer::View& view )
{
	// switch on the cursor
	osgViewer::Viewer::Windows windows;
    view.getViewerBase()->getWindows(windows);
    for(osgViewer::Viewer::Windows::iterator itr = windows.begin();
	    itr != windows.end();
	    ++itr)
    {
	    (*itr)->useCursor( true );
    }
}

// ----------------------------------------------------------------------------
/*!
* \param view : Reference to the view.
 * \return Pointer to the viewport.
 * \par Actions:
 *      - Finds viewport from view's camera or slave camera #0.
 */
const osg::Viewport* GameManipulator::getViewport( osgViewer::View& view )
{
    const osg::Viewport* ret = 0;
    const osg::Camera* cam = view.getCamera();
    if( cam )
    {
        ret = cam->getViewport();
    }
    if( !ret && view.getNumSlaves() > 0 )
    {
        osg::View::Slave& slave = view.getSlave(0);
        if( !!(slave._camera) )
        {
            ret = slave._camera->getViewport();
        }
    }
    return ret;
}

// ----------------------------------------------------------------------------
/*!
* \param view : Reference to the view.
 * \return Width of viewport.
 */
unsigned int GameManipulator::getViewportWidth( osgViewer::View& view )
{
    const osg::Viewport* vp = getViewport( view );
    if( vp )
    {
        return vp->width();
    }
    return 0.0f;
}

// ----------------------------------------------------------------------------
/*!
* \param view : Reference to the view.
 * \return Height of viewport.
 */
unsigned int GameManipulator::getViewportHeight( osgViewer::View& view )
{
    const osg::Viewport* vp = getViewport( view );
    if( vp )
    {
        return vp->height();
    }
    return 0.0f;
}

// ----------------------------------------------------------------------------
/*!
 * \param ea : Keyboard and mouse events.
 * \param us : Interface to the GUI system.
 * \par Actions:
 *      - Set/clear class input state variables according to user input.
 *      - Enforces mouse position to be at a static location if we are in mouse-view mode.
 */
bool GameManipulator::handle(const GUIEventAdapter& ea,GUIActionAdapter& us)
{
    osgViewer::View* viewp = dynamic_cast<osgViewer::View*>(us.asView());
    if( !viewp )
    {
        return false;
    }
    osgViewer::View& view = *viewp;

    switch( ea.getEventType() )
    {
        case( GUIEventAdapter::FRAME ):
            addMouseEvent( ea );
            if( calcMovement( view ) )
			{
				us.requestRedraw();
			}
            return false;

        case( GUIEventAdapter::RESIZE ):
            init( ea, us );
            us.requestRedraw();
            return true;
        default:
            break;
    }

    if( ea.getHandled() )
	{
		return false;
	}

	// Right mouse button will warp the mouse so that it stays in the same location
	// This is a need to have mouse-view 
	if( _mouseView )
	{
	    unsigned int x = getViewportWidth( view )  / 2;
	    unsigned int y = getViewportHeight( view ) / 2;
        const osgViewer::GraphicsWindow* gw = dynamic_cast<const osgViewer::GraphicsWindow*>(view.getCamera()->getGraphicsContext());
        if (!gw)
        {
            if( view.getNumSlaves() > 0 )
            {
                gw = dynamic_cast<const osgViewer::GraphicsWindow*>(view.getSlave(0)._camera->getGraphicsContext());
            }
        }
        if (gw)
        {
            const_cast<osgViewer::GraphicsWindow*>(gw)->requestWarpPointer( float(x), float(y) );
        }
	}


    switch( ea.getEventType() )
    {
        case( GUIEventAdapter::PUSH ):
        {
			if( ea.getButton() & GUIEventAdapter::RIGHT_MOUSE_BUTTON )
			{
				_mouseView = !_mouseView;
				if( _mouseView )
				{
					hideMouse( view );
				}
				else
				{
					showMouse( view );
				}
				flushMouseEventStack();
	            unsigned int x = getViewportWidth( view )  / 2;
	            unsigned int y = getViewportHeight( view ) / 2;
                const osgViewer::GraphicsWindow* gw = dynamic_cast<const osgViewer::GraphicsWindow*>(view.getCamera()->getGraphicsContext());
                if (!gw)
                {
                    if( view.getNumSlaves() > 0 )
                    {
                        gw = dynamic_cast<const osgViewer::GraphicsWindow*>(view.getSlave(0)._camera->getGraphicsContext());
                    }
                }
                if (gw)
                {
                    const_cast<osgViewer::GraphicsWindow*>(gw)->requestWarpPointer( float(x), float(y) );
                }
			}
			else
			{
				addMouseEvent( ea );
				us.requestContinuousUpdate( true );
				if( calcMovement( view ) ) 
				{
					us.requestRedraw();
				}
			}
            return true;
        }

        case( GUIEventAdapter::RELEASE ):
        {
            addMouseEvent( ea );
            us.requestContinuousUpdate( true );
            if( calcMovement( view ) ) 
			{
				us.requestRedraw();
			}
            return true;
        }

        case( GUIEventAdapter::DRAG ):
        {
            addMouseEvent( ea );
            us.requestContinuousUpdate( true );
            if( calcMovement( view ) ) 
			{
				us.requestRedraw();
			}
            return true;
        }

        case( GUIEventAdapter::MOVE ):
        {
            addMouseEvent( ea );
            us.requestContinuousUpdate( true );
            if( calcMovement( view ) )
			{
				us.requestRedraw();
			}
            return true;
        }


        case( GUIEventAdapter::KEYDOWN ):
        {
			_bookmarkValue = ea.getKey();
			_bookmarkTrigger = _bookmarkKey;
			switch( ea.getKey() )
			{
				case 'r':
                    {
                        ReloadShaderNodeVisitor rsnv;
                        view.getSceneData()->accept( rsnv );
                    }
					return true;

				case 'e':
					_warpTrigger = true;
					return true;

				case 'w':
					_moveForward = true;
					return true;

				case 's':
					_moveBackward = true;
					return true;

				case 'd':
					_moveRight = true;
					return true;

				case 'a':
					_moveLeft = true;
					return true;

				case 'b':
					_bookmarkKey = true;
					return true;

                case 'f':
                    _fovKey = true;
                    return true;

                case 'c':
                    _clipRangeKey = true;
                    return true;

                case 'g':
                    _clampToGround = !_clampToGround;
                    return true;

                case 'l':
                    setDebugLightState( !getDebugLightState() );
                    return true;

				case( GUIEventAdapter::KEY_Space ):
				{
					flushMouseEventStack();
					home( ea, us );
					return true;
				}

				case( GUIEventAdapter::KEY_Up ):
				{
					_speedUp = true;
					return true;
				}

				case( GUIEventAdapter::KEY_Down ):
				{
					_speedDown = true;
					return true;
				}

                case( GUIEventAdapter::KEY_Page_Down ):
				{
					PageDown();
					return true;
				}

                case( GUIEventAdapter::KEY_Page_Up ):
				{
					PageUp();
					return true;
				}

				case( GUIEventAdapter::KEY_Alt_L ):
				case( GUIEventAdapter::KEY_Alt_R ):
				{
					_altKey = true;
					return true;
				}
			}
            return false;
        }

        case( GUIEventAdapter::KEYUP ):
        {
			switch( ea.getKey() )
			{
				case 'w':
					_moveForward = false;
					return true;

				case 's':
					_moveBackward = false;
					return true;

				case 'd':
					_moveRight = false;
					return true;

				case 'a':
					_moveLeft = false;
					return true;

				case 'b':
					_bookmarkKey = false;
					return true;

                case 'f':
                    _fovKey = false;
                    return true;

                case 'c':
                    _clipRangeKey = false;
                    return true;

				case( GUIEventAdapter::KEY_Up ):
				{
					_speedUp = false;
					return true;
				}

				case( GUIEventAdapter::KEY_Down ):
				{
					_speedDown = false;
					return true;
				}

				case( GUIEventAdapter::KEY_Alt_L ):
				case( GUIEventAdapter::KEY_Alt_R ):
				{
					_altKey = false;
					return true;
				}
			}
            return false;
        }
    }
	return false;
}

// ----------------------------------------------------------------------------
/*!
 * \retval usage : Application usage description class.
 * \par Actions:
 *      - Specifies the user interface to this manipulator.
 */
void GameManipulator::getUsage(osg::ApplicationUsage& usage) const
{
    usage.addKeyboardMouseBinding( "Game: f",			 "Change FOV (needs mouse-view mode)" );
    usage.addKeyboardMouseBinding( "Game: c",			 "Change clip-range (needs mouse-view mode)" );
    usage.addKeyboardMouseBinding( "Game: g",			 "Toggle clamp to ground mode" );
    usage.addKeyboardMouseBinding( "Game: w",			 "Move forward" );
    usage.addKeyboardMouseBinding( "Game: s",			 "Move backward" );
    usage.addKeyboardMouseBinding( "Game: a",			 "Strafe left (mouse-view), or rotate left" );
    usage.addKeyboardMouseBinding( "Game: d",			 "Strafe right (mouse-view), or rotate right" );
    usage.addKeyboardMouseBinding( "Game: e",			 "'Instant' move to mouse location" );
    usage.addKeyboardMouseBinding( "Game: l",			 "Toggle debug-light" );
    usage.addKeyboardMouseBinding( "Game: Right button", "Toggle mouse-view" );
    usage.addKeyboardMouseBinding( "Game: Left button",  "Hold for fine-tune" );
    usage.addKeyboardMouseBinding( "Game: Space",		 "Home position" );
    usage.addKeyboardMouseBinding( "Game: UP",			 "Increase movement speed" );
    usage.addKeyboardMouseBinding( "Game: DOWN",		 "Decrease movement speed" );
}


// ----------------------------------------------------------------------------
/*!
 * \par Actions:
 *      - Clears the mouse event stack.
 */
void GameManipulator::flushMouseEventStack()
{
    _ga_t1 = NULL;
    _ga_t0 = NULL;
}


// ----------------------------------------------------------------------------
/*!
 * \param ea : Keyboard and mouse events.
 * \par Actions:
 *      - Stores the input event in our event-stack.
 */
void GameManipulator::addMouseEvent( const GUIEventAdapter& ea )
{
    _ga_t1 = _ga_t0;
    _ga_t0 = &ea;
}

// ----------------------------------------------------------------------------
/*!
 * \param matrix : New camera location and rotation.
 * \par Actions:
 *      - Extract and remember position from matrix.
 *      - Extract and remember Y rotation from matrix.
 *      - Extract and remember X rotation from matrix.
 *
 * This class does not care about rotation around the Z axis. Switching to this
 * camera-manipulator from another manipulator will thus cause the camera to
 * pop as it removes any roll value.
 */
void GameManipulator::setByMatrix( const osg::Matrixd& matrix )
{
	// Set translation
	_eye = matrix.getTrans();

	// Calculate needed rotation around Z (up) vector
	osg::Vec3 forward( 0.0, 0.0, -1.0 );    // Camera default forward vector is down (-Z)
	forward = matrix.getRotate() * forward; // Rotate the forward vector
	forward[2] = 0.0;                       // We only want to see rotation values around Z axis
	if( forward.normalize() == 0.0 )
	{
		_angleX = 0.0;                      // No rotation needed around Z
	}
	else
	{
		_angleX = -acos( forward[1] );     // Find angle
		if( forward[0] < 0.0 )
			_angleX = M_PI-_angleX+M_PI;   // Compensate if more than 180
	}

	// Calculate needed rotation around X (right) vector
	_rotation = osg::Quat( _angleX                        // Calculate new rotation around Z
		                 , osg::Vec3( 0.0
						            , 0.0
									, 1.0 
									) 
						 );                               
	_rotation = matrix.getRotate() * _rotation.inverse(); // Subtract this from given rotation
	forward = _rotation * osg::Vec3( 0.0, 0.0, -1.0 );    // Rotate forward vector with remainder
	forward[0] = 0.0;                                     // We only want rotation around X
	if( forward.normalize() == 0.0 )
	{
		_angleY = 0.0;                                    // No rotation needed around X 
	}
	else
	{
		_angleY = acos( forward[1] );                     // Find angle
		if( forward[2] < 0.0 )
			_angleY = M_PI-_angleY+M_PI;                  // Compensate if more than 180
	}

	// Ensure our rotation matrix is up to date.
	computeRotation();
}

// ----------------------------------------------------------------------------
/*!
 * \return Matrix containing camera position and rotation.
 */
osg::Matrixd GameManipulator::getMatrix() const
{
    return osg::Matrixd::rotate( _rotation ) * osg::Matrixd::translate( _eye );
}

// ----------------------------------------------------------------------------
/*!
 * \return Matrix containing inverse camera position and rotation.
 */
osg::Matrixd GameManipulator::getInverseMatrix() const
{
    return osg::Matrixd::translate( -_eye ) * osg::Matrixd::rotate( _rotation.inverse() );
}

// ----------------------------------------------------------------------------
/*!
 * \par Actions:
 *      - Clamps the rotation variables to maximum 2*PI.
 *
 * This is done in order to avoid that the rotation variable grows to high values
 * where floating point resolution will prohibit smooth rotation.
 */
void GameManipulator::clampRotation()
{
	_angleX = fmod( _angleX, M_PI*2.0 );
	_angleY = fmod( _angleY, M_PI*2.0 );
}

// ----------------------------------------------------------------------------
/*!
 * \param eye    : New position for the camera.
 * \param angleY : New camera rotation around the Y axis.
 * \param angleX : New camera rotation around the X axis.
 * \par Actions:
 *      - Stores the input values.
 *      - Calls computeRotation to deal with the new rotation values.
 */
void GameManipulator::computePosition( const osg::Vec3d& eye, const double angleY, const double angleX )
{
    _eye    = eye;
	_angleX = angleX;
	_angleY = angleY;

	computeRotation();
}

// ----------------------------------------------------------------------------
/*!
 * \par Actions:
 *      - Clamps the rotation.
 *      - Calculates the new forward direction vector.
 *      - Calculates the new right side direction vector.
 *      - Calculates the new rotation quaternion from the X and Y rotation angles.
 */
void GameManipulator::computeRotation()
{
	clampRotation();

	// Calculate new move-directions
    if( _clampToGround )
    {
        _rotation = osg::Quat( _angleX, osg::Vec3( 0.0, 0.0,  1.0 ) );
    }
    else
    {
	    _rotation  = osg::Quat( _angleY, osg::Vec3( 1.0, 0.0,  0.0 ) );
        _rotation *= osg::Quat( _angleX, osg::Vec3( 0.0, 0.0,  1.0 ) );
    }
	_forwardDirection = _rotation * osg::Vec3( 0.0, 1.0, 0.0 );
	_rightDirection   = _rotation * osg::Vec3( 1.0, 0.0, 0.0 );
    _upDirection      = _rotation * osg::Vec3( 0.0, 0.0, 1.0 );

	// Calculate new camera view-rotation
	_rotation  = osg::Quat( _angleY+M_PI/2.0, osg::Vec3( 1.0, 0.0,  0.0 ) );
    _rotation *= osg::Quat( _angleX         , osg::Vec3( 0.0, 0.0,  1.0 ) );
}


// ----------------------------------------------------------------------------
/*!
 * \par Actions:
 *      - Calculates the new position for the camera.
 *      - Ensures that all input states are taken into consideration.
 *
 * This is highly dependent on the various input states. Read through the code
 * comments to know exactly what happens.
 */
bool GameManipulator::calcMovement( osgViewer::View& view )
{
    // return if less then two events have been added.
    if( _ga_t0.get() == NULL || _ga_t1.get() == NULL ) 
	{
		return false;
	}

    // newEye will hold the new position calculation. We store it in a temporary
    // variable first in case the tri-pod (ground-clamp object) should refuse
    // the new location.
    osg::Vec3d newEye = _eye;

    // Get delta time between this and previous user input.
    double dt = _ga_t0->getTime() - _ga_t1->getTime();

    // OSG code paranoia check
    if( dt<0.0f )
    {
        notify( INFO ) << "warning dt = " << dt << std::endl;
        dt = 0.0f;
    }

    // Intensity will be reduced if we are in a left-click hold state.
	double intensity = 1.0;

    // Movement speed increases the further away from ground level that we are.
    double za = _zeroAltitude;
    double extraVelocity = (fabs(newEye[2] - za) / 100.0 * 2.0) + 1.0;
    double newza;
//	double extraVelocity = (fabs(newEye[2] - za) / 100.0 * 2.0) + 1.0;

    if( _triPod && _triPod->clampToGround( newEye, newza ))
    {
        za = newEye._v[2] - newza;
	    extraVelocity = (fabs(za) / 100.0 * 2.0) + 1.0;
    }

    // Helpers to hold old and new mouse button mask.
	unsigned int newButtonMask = _ga_t0->getButtonMask();

    // Reduce intensity if user are holding down the left mouse button.
	if( newButtonMask & GUIEventAdapter::LEFT_MOUSE_BUTTON )
	{
		intensity *= 0.35;
	}

    // Resolve user speed-up request.
	if( _speedUp )
	{
		_velocity += intensity * 1.0;
	}

    // Resolve user speed-down request.
	if( _speedDown )
	{
		_velocity -= intensity * 1.0;
		if( _velocity < 0.1 )
		{
			_velocity = 0.1;
		}
	}

    // Things are different in mouse-view mode and normal mode
	if( _mouseView )
    {
        //
        // Bug-fix: For mouse-view auto-scroll bug.
        //          Function _ga_t0->getY() returns a float that shows where the mouse 
        //          is on screen. This value is always an integer, you never get 
        //          a 138.5 value even though it returns a float.
        //          If view-port is odd sized, then the _ga_t0->getY() value will be 
        //          height/2 rounded up.
        //          To avoid auto-scrolling in mouse-view mode:
        //              We have to add one to the viewport-height and then remove the
        //              least significant bit. h = (getViewportHeight()+1)&~1
        //
        float dy = (_ga_t0->getY()/((getViewportHeight( view )+1)&~1UL))*2.0 - 1.0;
        if( _ga_t0->getMouseYOrientation()==osgGA::GUIEventAdapter::Y_INCREASING_DOWNWARDS )
        {
            dy = -dy;
        }
        // Resolve mouse-view mode.
        if( !_fovKey )
        {
            if( !_clipRangeKey )
            {
                // Mouse-view, calculate rotation angles from mouse movement.
		        _angleY += dy * 1.0 * intensity;
                _angleX -= ((_ga_t0->getX()/((getViewportWidth( view )+1)&~1UL))*2.0 - 1.0) * 0.5 * intensity;
            }
            else
            {
                // Far-clip-range modification mode.
                // Calculate far-clip-range from mouse movement.
                double fovy, aspectRatio, zNear, zFar;
                view.getCamera()->getProjectionMatrixAsPerspective( fovy, aspectRatio, zNear, zFar );
                zFar += dy * 5000.0 * intensity;
                if( zFar < zNear + 1.0 ) 
                {
                    zFar = zNear + 1.0;
                }
                else if( zFar > 100000.0f ) 
                {
                    zFar = 100000.0f;
                }
                view.getCamera()->setProjectionMatrixAsPerspective( fovy, aspectRatio, zNear, zFar );
            }
        }
        else
        {
            // Field of view (FOV) modification mode.
            // Calculate FOV from mouse movement.
            double fovy, aspectRatio, zNear, zFar;
            view.getCamera()->getProjectionMatrixAsPerspective( fovy, aspectRatio, zNear, zFar );
            fovy -= dy * 5.0 * intensity;
            if( fovy < 0.1f ) 
            {
                fovy = 0.1f;
            }
            else if( fovy > 179.0f ) 
            {
                fovy = 179.0f;
            }
            view.getCamera()->setProjectionMatrixAsPerspective( fovy, aspectRatio, zNear, zFar );
            printf( "Field of view: %3.2f degrees, you see %3.2f degrees of the world.\n", fovy, fovy*2.0 );
        }

        // Rotation values are updated, resolve new rotation.
	    computeRotation();

        // Handle user strafe right request.
		if( _moveRight )
		{
			newEye += _rightDirection * _velocity * dt * intensity * extraVelocity * 0.5;
		}

        // Handle user strafe left request.
		if( _moveLeft )
		{
			newEye += _rightDirection * -_velocity * dt * intensity * extraVelocity * 0.5;
		}
	}
	else
	{
        // Resolve non-mouse-view mode (normal mode).
        // Handle user rotate right request.
		if( _moveRight )
		{
			_angleX -= 1.0 * dt * intensity;
		}

        // Handle user rotate left request.
        if( _moveLeft )
		{
			_angleX += 1.0 * dt * intensity;
		}

        // Rotation values are updated, resolve new rotation.
        computeRotation();
	}

    // Handle user move forward request.
	if( _moveForward )
	{
        newEye += _forwardDirection * _velocity * dt * intensity * extraVelocity;
	}

    // Handle user move backward request.
    if( _moveBackward )
	{
        newEye -= _forwardDirection * _velocity * dt * intensity * extraVelocity;
	}

    // Handle user move warp to location request.
    if( _warpTrigger )
	{
        // Figure out where in to world the mouse points at.
        if( mouseRayIntersect( view, _ga_t0->getX(), _ga_t0->getY() ) )
        {
            // Move to the location mouse points at.
            // Movement is not a pop, it is a n frame slide ('instant'-move).
            _instantMoveLoc = _collisionPoint - (_forwardDirection * 50.0);
            _instantMove = true;
        }
	}

    // Handle any on-going 'instant'-move slide request.
    bool wasInstantMove = _instantMove;
	if( _instantMove )
	{
		osg::Vec3 delta = _instantMoveLoc - newEye;
		newEye = newEye + ( delta / 5.0 );
        if( _clampToGround )
        {
            delta._v[2] = 0.0;
        }
        // Turn off 'instant'-move when we are close enough.
		_instantMove = delta.length() > 2.0;
	}

    // Handle user bookmark request.
	if( _bookmarkTrigger )
	{
		if( _altKey )
		{
            // We are setting a bookmark!
			_altKey = false;
			_bookmarkKey = false;
			bookmarkSet();
		}
		else
		{
            // We are moving to a set bookmark.
			bookmarkGet();
		}
	}

    // Handle clamp-to-ground mode.
    // We ignore clamp-to-grund if we are in a 'instant'-move slide mode.
    if( !wasInstantMove && _clampToGround )
    {
        // Calculate new location based on ground-clamp.
        bool allLegsOnGround = _triPod->clampToGround( newEye, newEye._v[2] );
        if( allLegsOnGround )
        {
            // All of the tri-pod legs are colliding with ground.
            // We will thus accept this movement.
            // If any leg is no longer colliding with ground, then we do not
            // accept the new movement. We thus force the tripod to stay on
            // the terrain.
            _eye = newEye;
        }
    }
    else
    {
        // We are in flying mode, accept new position.
        _eye = newEye;
    }

    // Ensure some of the user input modes are 'one-shot' modes.
	_warpTrigger = false;
	_bookmarkTrigger = false;

	return true;
}

// ----------------------------------------------------------------------------
/*!
 * \param x : Mouse's x position on screen.
 * \param y : Mouse's y position on screen.
 * \return Boolean stating if we collided or not.
 * \par Actions:
 *  - Cast a ray from mouse screen-position and into the world.
 *  - Update class member _collisionPoint with the ray's collision point (if colliding).
 *  - Update class member _collisionNormal with the ray's collision point normal (if colliding).
 * \note If there are multiple collisions then we do not locate the closest collision.
 *       I have not yet seen that this has been needed.
 * 
 */
bool GameManipulator::mouseRayIntersect( osgViewer::View& view, const double x, const double y )
{
    osg::Vec3 camPos;
    osg::Camera* camera;
    if( view.getCamera()->getViewport() )
    {
        camera = view.getCamera();
    }
    else
    {
        camera = view.getSlave(0)._camera.get();
    }

    const osg::Matrixd& mat = camera->getViewMatrix();
    const osg::Quat&    rot = mat.getRotate().inverse();
    camPos.set(-(rot * mat.getTrans()));
    AlphaIntersectionVisitor aiv( camPos, x, y, 0.1 );
    camera->accept( aiv );
    if( aiv.processCollisions() )
    {
        _collisionPoint  = aiv.getCollisionPoint();
        _collisionNormal = aiv.getCollisionPointNormal();
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
/*!
 * \par Actions:
 *  - Store current location and rotation.
 *  - Data is stored in a dictionary with current input key combination as index-key.
 *  - All bookmarks are then stored on disk.
 */
void GameManipulator::bookmarkSet()
{
	if( _bookmarkValue && _bookmarkValue != 'b' && _bookmarkValue != 'B' )
	{
		Bookmark_t* b = new Bookmark_t;
		b->_pos = _eye;
		b->_rx = _angleX;
		b->_ry = _angleY;
        b->_id = _numBookmarks++;
        b->_text = "";

		_bookmarks[_bookmarkValue] = b;
		_bookmarkValue = 0;

        std::ofstream os( _bookmarkFilename.c_str(), std::ios_base::binary  );
		std::map<int,Bookmark_t*>::const_iterator i;
        char zBuf[1024];
		for( i = _bookmarks.begin(); i != _bookmarks.end(); ++i )
		{
			Bookmark_t* b = i->second;
			int key = i->first;
            sprintf( zBuf, "Bookmark: %d %d %f %f %f %f %f\nTextStart:\n%s\nTextEnd\n", b->_id, key, b->_pos._v[0], b->_pos._v[1], b->_pos._v[2], b->_rx, b->_ry,b->_text.c_str() );
            os.write( zBuf, strlen( zBuf ) );
		}
	}
}

// ----------------------------------------------------------------------------
/*!
 * \par Actions:
 *  - Determines if current input key combination has a bookmark stored for it.
 *  - Warps camera to the new location and rotation using 'instant'-move slide.
 */
void GameManipulator::bookmarkGet()
{
	std::map<int,Bookmark_t*>::const_iterator i;
	i = _bookmarks.find( _bookmarkValue );
	if( i != _bookmarks.end() )
	{
		Bookmark_t* b = i->second;
        GotToBookmark( *b );
    }
}

// ----------------------------------------------------------------------------
/*!
 * \par Actions:
 *      - Jumps to previous bookmark.
 *      - We do not wrap to the last bookmark in the list until we have seen
 *        the last bookmark at least one time.
 */
void GameManipulator::PageUp()
{
    if( _currentBookmark <= 0 )
    {
        if( !_lastBookmarkSeen )
        {
            return;
        }
    }

    _currentBookmark--;
    if( _currentBookmark < 0 )
    {
        _currentBookmark = _numBookmarks - 1;
    }
	std::map<int,Bookmark_t*>::const_iterator i;
    for( i = _bookmarks.begin();
         i != _bookmarks.end();
         i++ )
	{
		Bookmark_t* b = i->second;
        if( b->_id == _currentBookmark )
        {
            GotToBookmark( *b );
            return;
        }
    }
}


// ----------------------------------------------------------------------------
/*!
 * \par Actions:
 *  - Jumps to next bookmark.
 */
void GameManipulator::PageDown()
{
    _currentBookmark++;
    if( _currentBookmark+1 >= _numBookmarks )
    {
        _lastBookmarkSeen = true;
    }
    if( _currentBookmark == _numBookmarks )
    {
        _currentBookmark = 0;
    }
	std::map<int,Bookmark_t*>::const_iterator i;
    for( i = _bookmarks.begin();
         i != _bookmarks.end();
         i++ )
	{
		Bookmark_t* b = i->second;
        if( b->_id == _currentBookmark )
        {
            GotToBookmark( *b );
            return;
        }
    }
}


// ----------------------------------------------------------------------------
/*!
 * \param b : Reference to wanted bookmark.
 * \par Actions:
 *  - Jumps to given bookmark.
 */
void GameManipulator::GotToBookmark( const Bookmark_t& b )
{
	_instantMove = true;
	_instantMoveLoc = b._pos;
	_angleX = b._rx;
	_angleY = b._ry;

    removeCurrentText();
    if( b._text.length() )
    {
        computeRotation();
        osg::Group* group = dynamic_cast<osg::Group*>(_node.get());
        if( group )
        {
            _currentText = create3DText( _instantMoveLoc
                                             + _forwardDirection * 150.0f
                                             - _rightDirection   *  50.0f
                                             + _upDirection      *  30.0f
                                       , _rotation
                                       , b._text 
                                       );
            group->addChild( _currentText );
        }
    }
}

/*!
 * \par Actions:
 *      - Removes the text from the scene.
 */
void GameManipulator::removeCurrentText()
{
    if( _currentText )
    {
        DeleteSelectedNodesVisitor dsnv;
        _node->accept( dsnv );
        dsnv.pruneSelectedNodes();
        _currentText = NULL;
    }
}

void GameManipulator::setClampToGroundDist( double dist ) 
{ 
    _clampToGroundDist = dist;
    if(!!_triPod) 
        _triPod->setClampToGroundDist( dist ); 
}

}
