
#ifndef SOLEIL_SOUND__HPP
#define SOLEIL_SOUND__HPP

#include <stdlib.h>
#include <AL/al.h>
#include <AL/alc.h>
// #include <AL/alu.h>
#include <AL/alut.h>

#include "FirstPersonManipulator.hpp"

namespace Soleil
{
  static void throw_AlError_onfailure()
  {
    ALenum error= alGetError();

    if (error != AL_NO_ERROR)
      {
	throw alGetString(error);
      }
  }

  
  static void throw_ALCError(ALCdevice *device)
  {
    ALCenum error = alcGetError(device);
    
    throw alGetString(error);      
  }

  static void throw_ALUTError_onfailure()
  {
    ALenum error = alutGetError();
    if (error != ALUT_ERROR_NO_ERROR)
      {
	throw alutGetErrorString(error);
      }
  }

  // TODO might be wise to find wrapper arround AL* calls in oder to throw on error intead of adding the throw_* each times.

  class Device
  //: public osgGA::TrackballManipulator
  //, public osg::NodeCallback
  {
  public:
    Device()
    //: osgGA::TrackballManipulator()
    //, osg::NodeCallback()
    {
      _device = alcOpenDevice(NULL);
      if (!_device)
	throw_ALCError(_device);

      // TODO use inner class for the context in order to benefit of RAII in case of exception
      _context = alcCreateContext(_device, NULL);

      if (_context == nullptr)
	throw_ALCError(_device);
    
      if (!alcMakeContextCurrent(_context))
	throw_ALCError(_device);

      ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

      alListener3f(AL_POSITION, 0, 0, 0.0f);
      throw_AlError_onfailure();
      alListener3f(AL_VELOCITY, 0, 0, 0);
      throw_AlError_onfailure();
      alListenerfv(AL_ORIENTATION, listenerOri);
      throw_AlError_onfailure();

      //this->setUpdateCallback(this);

    }
  
    virtual ~Device(void)
    {
      alcMakeContextCurrent(NULL);
    
      alcDestroyContext(_context);
      alcCloseDevice(_device);
    }

    ALCdevice	*get(void) noexcept
    {
      return _device;
    }
  
  private:
    ALCdevice	*_device;
    ALCcontext	*_context;
  };


  class Sound
  // : public osg::MatrixTransform
  // , public osg::NodeCallback
  {
  public:
    Sound()
    // : osg::MatrixTransform()
    // ,  osg::NodeCallback()
    {
      // TODO use inner class for the buffer in order to benefit of RAII in case of exception

      alGenSources((ALuint)1, &_source);
      throw_AlError_onfailure();

      alSourcef(_source, AL_PITCH, 1);
      throw_AlError_onfailure();
      alSourcef(_source, AL_GAIN, 1);
      throw_AlError_onfailure();
      alSource3f(_source, AL_POSITION, 0, 0, 10);
      throw_AlError_onfailure();
      alSource3f(_source, AL_VELOCITY, 0, 0, 0);
      throw_AlError_onfailure();
      alSourcei(_source, AL_LOOPING, AL_TRUE);


      alGenBuffers((ALuint)1, &_buffer);
      throw_AlError_onfailure();

      ALsizei size, freq;
      ALenum format;
      ALvoid *data;
      ALboolean loop = AL_FALSE;

      alutLoadWAVFile((ALbyte *)"test.wav", &format, &data, &size, &freq, &loop);
      throw_ALUTError_onfailure();

      alBufferData(_buffer, format, data, size, freq);
      throw_AlError_onfailure();

      alSourcei(_source, AL_BUFFER, _buffer);
      throw_AlError_onfailure();

      //this->setUpdateCallback(this);
    }
  
    virtual ~Sound(void)
    {
      stop();
      alDeleteSources(1, &_source);
      alDeleteBuffers(1, &_buffer);
    }

    void play(void)
    {
      alSourcePlay(_source);
      throw_AlError_onfailure();
    }

    void stop(void)
    {
      alSourceStop(_source);
      //throw_AlError_onfailure();
    }

    
    void setPosition(const osg::Vec3d  &v)
    {
      alSource3f(_source, AL_POSITION, v.x(), v.y(), v.z());
      throw_AlError_onfailure();
    }

    // virtual void 	operator()(osg::Node *node, osg::NodeVisitor *nv)
    // {
    //   const osg::Vec3d  &v = //this->getMatrix().getTrans();
    
    //   alSource3f(_source, AL_POSITION, v.x(), v.y(), v.z());
    //   std::cout <<  v.x()<< ","  << v.y()<< ","  << v.z() << "\n";

    //   traverse(node, nv);
    // }
  
  private:
    ALuint	_source;
    ALuint	_buffer;
  };


  class ListenerCallBack
    : public osg::NodeCallback
  {
  public:
    //ListenerCallBack(osgGA::TrackballManipulator &player, Device &device)
    ListenerCallBack(FirstPersonManipulator &player, Device &device)
      : osg::NodeCallback()
      , _player(player)
      , _device(device)
    {
    }
      
    virtual ~ListenerCallBack(void)
    {
    }

    virtual void 	operator()(osg::Node *node, osg::NodeVisitor *nv) override
    {
      const osg::Vec3d  &v = _player.getMatrix().getTrans();
      osg::Vec3d eye;
      osg::Vec3d center;
      osg::Vec3d up;
      _player.getMatrix().getLookAt(eye, center, up);

      ALfloat listenerOri[] = { (float)center.x(), (float)center.y(), (float)center.z()
				, (float)up.x(), (float)up.y(), (float)up.z() };

      alListenerfv(AL_ORIENTATION, listenerOri);

    
      alListener3f(AL_POSITION, v.x(), v.y(), v.z());
      //std::cout << "Listener: " <<  v.x()<< ","  << v.y()<< ","  << v.z() << "\n";
      //osgGA::TrackballManipulator::operator()(node, nv);

    
      traverse(node, nv);
    }

  private:
    FirstPersonManipulator	&_player;
    Device			&_device;
  };


  class UpdateSoundCallBack
    : public osg::NodeCallback
  {
  public:
    UpdateSoundCallBack(osg::PositionAttitudeTransform &node)//, Sound &sound)
      : osg::NodeCallback()
      , _node(node)
	//, _sound(sound)
    {
      _sound.play();
    }

    virtual ~UpdateSoundCallBack(void)
    {
      _sound.stop();
    }

    void stop()
    {
      _sound.stop();
    }

    virtual void 	operator()(osg::Node *node, osg::NodeVisitor *nv) override
    {
      osg::Switch *s = node->asSwitch();
      
      const osg::Vec3d  &v = _node.getPosition();//_node.getMatrix().getTrans();

      _sound.setPosition(v);
      //alSource3f(_source, AL_POSITION, v.x(), v.y(), v.z());
    
      traverse(node, nv);
    }


  private:
    osg::PositionAttitudeTransform	&_node;
    Sound				_sound;
  };

};

#endif
