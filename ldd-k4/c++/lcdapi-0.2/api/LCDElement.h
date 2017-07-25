#ifndef _LCD_ELEMENT_H_
#define _LCD_ELEMENT_H_

/**
 * \defgroup main Main components
 *  These are the main components of the LCDApi.
 *   Some of them are used internally and should
 *   not be useful for API users.
 *
 *  The most important one is LCDClient as each program
 *   using LCDApi should have one instance of this class
 *   to connect to LCDproc server (LCDd).
 *
 *  The screen class is also important because all the element
 *   diplayed on the LCD are in fact in an LCDScreen. You can have
 *   more than one. LCDproc will then rotate them (according to value
 *   set with LCDScreen::setDuration().
 */

#include "LCDMutex.h"

#include <pthread.h>
#include <string>
#include <list>
#include <map>
#include <set>

/** \class LCDElement LCDElement.h "api/LCDElement.h"
 *  \brief Main class for all elements of the API.
 *  \ingroup main
 *  All the classes in this API have LCDElement as their base class.
 *   It contains important methods. But it should not be used directly.
 */

class LCDElement
{
 protected:
  static std::set<std::string> _elementsList;
  bool _iAmDead;
  static LCDMutex _elementMutex;
  static unsigned int _elementCounter;
  std::string _id;
  std::string _elementDel;
  std::string _elementAddCmd, _elementAddParam;
  LCDElement *_parent;
  class Command
  {
   public:
    std::string _cmd;
    std::string _params;
    Command(std::string cmd, std::string params) : _cmd(cmd), _params(params) {}
  };
  typedef std::list<Command> CommandList;
  typedef std::map<std::string, LCDElement *> ElementMap;
  ElementMap _childrenList;
  CommandList _commandBuffer;
  void setParent(LCDElement *parent);
  void addToList(LCDElement *elt);
  void removeFromList(LCDElement *elt);
  void notifyCreated();
  void notifyDestroyed();
  void notifyAdded();

 public:
  LCDElement(const std::string &id, const std::string &addCommand, const std::string &addParam = "", LCDElement *parent=0);

  /**
   * \brief Destructor of the element.
   *
   * This destructor sends deletion commands to LCDproc server
   *  so the element is removed.
  */

  ~LCDElement();

  /**
   * \brief Add a child to the component.
   *
   * This method is used to add a child to a component.
   * @param child The element to be added in the children list.
  */

  void add(LCDElement *child);
  virtual void sendCommand(const std::string &cmd, const std::string &parameters = "");

  /**
   * \brief Returns Id of the element.
   *
   * With this method one can have read access to the element identifier.
   * @return A string.
  */
  const std::string &getId();

  /**
   * \brief Test if a widget with given Id exists.
   *
   * This static method is used to test the existence of a widget.
   * @param id The identifier of the widget to test.
  */
  static bool exists(std::string id);

};

#endif
