#ifndef _LCD_SYSTEM_SENSOR_H_
#define _LCD_SYSTEM_SENSOR_H_

#include "LCDSensor.h"
#include <string>

/** \class LCDSystemSensor LCDSystemSensor.h "api/LCDSystemSensor.h"
 *  \brief A sensor that executes a shell command.
 *  \ingroup sensors
 *  This sensor takes a shell command (that can call external programs) and returns first line of its output.
 */
class LCDSystemSensor : public LCDSensor
{
 private:
    std::string _command;

 public:
  virtual void waitForChange();
  virtual std::string getCurrentValue();

  /**
   * \brief Default constructor.
   *
   * Used to build such a sensor.
   * @param command A string containing the shell command to execute.
   */
  LCDSystemSensor(const std::string &command);
};

#endif
