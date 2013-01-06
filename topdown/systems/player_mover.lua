local lua_system = {}

function lua_system:init()

  return {"Rigid_Body", "player_movement", "Input"}
end

function clamp(input, low, high)
  if input > high then 
    return high
  elseif input < low then
    return low
  end
  return input
end

function lua_system:update(dt, entity)
  print(key.DOWN)
  dx = (entity.Input.keys[key.L] and 1 or 0) - (entity.Input.keys[key.J] and 1 or 0)
  dy = (entity.Input.keys[key.I] and 1 or 0) - (entity.Input.keys[key.K] and 1 or 0)
  local force_mult = entity.player_movement.force_mult;
  local force_max = entity.player_movement.force_max;
  entity.Rigid_Body.forces = entity.Rigid_Body.forces + Vec3f(dx*dt*force_mult, 0, dy*dt*force_mult);
  entity.Rigid_Body.forces.x = clamp(entity.Rigid_Body.forces.x, -force_max, force_max)
  entity.Rigid_Body.forces.z = clamp(entity.Rigid_Body.forces.z, -force_max, force_max)
  
end

return lua_system
