#ifndef USER_CONTROLS_H
#define USER_CONTROLS_H

#include <globalvars.h>
#include <htmlgen/element.h>

class UserControlsGenerator : public CElement
{
public:
  UserControlsGenerator(void);

  void render(void);

};

#endif // USER_CONTROLS_H
