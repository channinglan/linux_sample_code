#ifndef _LCD_ICON_H_
#define _LCD_ICON_H_

#include "LCDWidget.h"
#include <string>

/** \class LCDIcon LCDIcon.h "api/LCDIcon.h"
 *  \brief A widget to display a predefined icon.
 *  \ingroup widgets
 * This widget is used to display a predefined icon somewhere on screen.
 */
class LCDIcon : public LCDWidget
{
 private:
  std::string _type;
 public:

  /**
   * \brief Default Constructor.
   *
   * This constructor can be used without parameter in most cases.
   * But the widget will have to be added to a parent (screen or frame).
   * @param parent A pointer to parent of this screen. It should be a LCDClient object.
   * @param id A string with the identifier for the screen. If not provided, a unique one will be generated automatically.
   */
  LCDIcon(LCDElement *parent = 0, const std::string &id = "");
  /**
   * \brief Constructor with widget values specified.
   *
   * This construct can be used to specify the values for the widget while building it.
   * @param type A string containing the name of the icon.
   * @param x Integer containing 1-based value for column number.
   * @param y Integer containing 1-based value for row number.
   * @param parent A pointer to parent of this screen. It should be a LCDClient object.
   * @param id A string with the identifier for the screen. If not provided, a unique one will be generated automatically.
   */
  LCDIcon(const std::string &type, int x=1, int y=1, LCDElement *parent = 0, const std::string &id = "");

  /**
   * \brief Set the values for the widget.
   *
   * Set or change the useful values for this widget.
   * @param type A string containing the name of the icon.
   * @param x Integer containing 1-based value for column number.
   * @param y Integer containing 1-based value for row number.
   */
  void set(const std::string &type, int x=1, int y=1);

  /**
   * \brief Set the displayed icon.
   *
   * Set or change the icon displayed on the LCD by this widget.
   * @param type A string containing the icon name.
   */
  void setIcon(const std::string &type);

  /**
   * \brief Get the displayed icon.
   *
   * Get the icon displayed on the LCD by this widget.
   * @return A string containing the icon name.
   */
  std::string getIcon();

  virtual void valueCallback(std::string value) {};

  virtual void notifyChanged();
};

#endif
