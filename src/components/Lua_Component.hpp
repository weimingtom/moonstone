#pragma once

#include <string>
#include <vector>

#include "Component.hpp"

struct Lua_Component : public Component
{
  Lua_Component();
  void set_file(std::string filename);
  virtual std::string name();
  virtual int get_lua_ref(lua_State * L);
private:
  std::string component_name;
  int component_ref;
  lua_State * L;
};
