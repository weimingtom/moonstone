#include "Lua_System.hpp"
#include <iostream>
#include "Manager.hpp"

Lua_System::Lua_System()
{
  L = Lua_Manager::get_instance()->get_lua_state();
}

Lua_System::~Lua_System()
{
  lua_close(L);
}

void Lua_System::set_file(std::string file)
{
  printf("Initing system with file: %s \n", filename.c_str());
  filename = file;
  int s = luaL_dofile(L, filename.c_str());
  Lua_Manager::get_instance()->report_errors(s);

  script_ref = luaL_ref(L, LUA_REGISTRYINDEX);
  lua_rawgeti(L, LUA_REGISTRYINDEX, script_ref);
  
  //call the lua init function
  lua_getfield(L, -1, "init");
  lua_pushvalue(L, -2); //push self value
  s = lua_pcall(L, 1, 1, 0);
  Lua_Manager::get_instance()->report_errors(s);

  //get the list of component types
  lua_pushnil(L);
  while(lua_next(L, -2)) {
    const char * v = lua_tostring(L, -1);
    std::string component_name(v);
    components.push_back(component_name);
    lua_pop(L, 1);
  }
  lua_pop(L, 2);
}

void Lua_System::reload()
{
  luaL_unref(L, LUA_REGISTRYINDEX, script_ref);
  components.clear();
  set_file(filename);
}

lua_State * Lua_System::get_lua_state()
{
  return L;
}
void Lua_System::init()
{
}

void Lua_System::create_ref_vector()
{
  std::vector<int> entities = manager->get_entities(components);
  for(auto entity : entities) {
    entity_ref_vector[entity] = create_entity_ref(entity);
  }
}

int Lua_System::create_entity_ref(int entity)
{
  lua_newtable(L);
  for(std::string component_name : components) {
    Component * component = manager->get_component(entity, component_name);
    int ref = component->get_lua_ref(L);
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    lua_setfield(L, -2, component_name.c_str());
  }
  int table_ref = luaL_ref(L, LUA_REGISTRYINDEX);
}

void Lua_System::replace_entity_ref(int entity)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, entity_ref_vector[entity]);

  for(std::string component_name : components) {
    Component * component = manager->get_component(entity, component_name);
    lua_getfield(L, -1, component_name.c_str());
    component->update_from_lua(L);
    lua_pop(L, 1);
  }
}

void Lua_System::update(float dt)
{

  create_ref_vector();
  for(int entity : manager->get_entities(components)) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, script_ref);

    lua_getfield(L, -1, "update");
    lua_pushvalue(L, -2); //push self
    lua_pushnumber(L, dt);
    //push the entity
    lua_rawgeti(L, LUA_REGISTRYINDEX, entity_ref_vector[entity]);

    int s = lua_pcall(L, 3, 0, 0);
    Lua_Manager::get_instance()->report_errors(s);

    //Re-populate components

    replace_entity_ref(entity);
    lua_pop(L,1);
  }

}
std::string Lua_System::get_name()
{
  return script_name;
}
void Lua_System::set_name(std::string name)
{
  script_name = name;
}
