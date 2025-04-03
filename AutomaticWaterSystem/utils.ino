void healthCheck()
{
  if (water_level_sensor.high_in_water() and !water_level_sensor.low_in_water())
  {
    errorCode = ErrorCode::H_IN_WATER_L_NOT_IN_WATER;
    raiseAbort();
    return;
  }

  if (relay.pumpFillingTankTime() > PUMP_ABORT_TIME * SEC_IN_MIN * 1000) 
  {
    errorCode = ErrorCode::PUMP_TIMEOUT;
    raiseAbort();
    return;
  }
}

void raiseAbort()
{
  relay.setPump(Relay::OFF);
  relay.set3WayValve(Relay::ThreeWayValveMode::DRAINAGE);
  relay.setSolenoid(Relay::OFF);

  settingsMode = SettingsMode::OFF;
  step = -1;
}