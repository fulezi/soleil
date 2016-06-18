


#ifndef SOLEIL_GAMEINSTANCE__HPP
#define SOLEIL_GAMEINSTANCE__HPP

# include <osgViewer/Viewer>

namespace Soleil
{
  class GameInstance : public osgViewer::Viewer
  {
  public:
    enum class GameState { Menu, Playing, Dead };

  public:
    GameInstance() {}
    virtual ~GameInstance(void) {}

    // States:
  public:
    bool playing(void) const noexcept
    {
      return state == GameState::Playing;
    }

    bool inMenu(void) const noexcept
    {
      return state == GameState::Menu;
    }
    
    // Modifiers:
  public:
    void play(void) {
      _root->setValue(0, true);
      _root->setValue(1, false);

      state = GameState::Playing;
    }
    
    void gameOver(void) {
      state = GameState::Dead;

      _root->setValue(2, true); // TODO insert child by value of use a search
    }

    void setSceneData(osg::ref_ptr<osg::Switch> root) {
      osgViewer::Viewer::setSceneData(root);

      _root = root;
    }

  protected:
    GameState		state = GameState::Menu;
    osg::ref_ptr<osg::Switch> _root;
    
  };

};

#endif
