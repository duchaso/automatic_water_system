void processButtonState()
{
  switch (settingsMode)
  {
    case SettingsMode::SET_DRAINAGE_DELAY:
      switch (buttonState)
      {
        case ButtonState::SHORT:
        {
          settingsMode = SettingsMode::SET_RTC_TRIGGER_TIME;
          return;
        }
        case ButtonState::LONG:
        {
          settingsMode = SettingsMode::RUN_PUMP;
          relay.setPump(Relay::ON);
          return;
        }
        case ButtonState::NOTHING:
        {
          drainageDelay = map(potentiometerValue, 0, POT_MAX_VAL, 5, 60);
          return;
        }
      }
    
    case SettingsMode::RUN_PUMP:
      switch (buttonState)
      {
        case ButtonState::LONG:
        {
          settingsMode = SettingsMode::SET_DRAINAGE_DELAY;
          relay.setPump(Relay::OFF);
          return;
        }
        default:
          return;
      }

    case SettingsMode::SET_RTC_TRIGGER_TIME:
      switch (buttonState)
      {
        case ButtonState::SHORT:
        {
          settingsMode = SettingsMode::SET_DRAINAGE_DELAY;
          return;
        }
        case ButtonState::LONG:
        {
          if (!rtc.isrunning()) return;
          settingsMode = SettingsMode::ADJUST_RTC_MODULE_H;
          return;
        }
        case ButtonState::NOTHING:
        {
          long val = map(potentiometerValue, 0, POT_MAX_VAL, 0, 24);
          rtcTriggerTime = DateTime(2025, 2, 24, val);
          return;
        }
      }

    case SettingsMode::ADJUST_RTC_MODULE_H:
    {
      if (!rtc.isrunning()) return;
      switch (buttonState)
      {
        case ButtonState::SHORT:
        {
          settingsMode = SettingsMode::ADJUST_RTC_MODULE_M;
          return;
        }
        case ButtonState::NOTHING:
        {
          long val = map(potentiometerValue, 0, POT_MAX_VAL, 0, 24);
          newTime = DateTime(2025, 2, 24, val);
          return;
        }
        default:
          return;
      }
    }

    case SettingsMode::ADJUST_RTC_MODULE_M:
    {
      if (!rtc.isrunning()) return;
      switch (buttonState)
      {
        case ButtonState::SHORT:
        {
          settingsMode = SettingsMode::SET_RTC_TRIGGER_TIME;
          rtc.adjust(newTime);
          return;
        }
        case ButtonState::NOTHING:
        {
          long val = map(potentiometerValue, 0, POT_MAX_VAL, 0, 60);
          newTime = DateTime(2025, 2, 24, newTime.hour(), val);
          return;
        }
        default:
          return;
      }
    }

    case SettingsMode::OFF:
      switch (buttonState) {
        case ButtonState::LONG:
        {
          settingsMode = SettingsMode::SET_DRAINAGE_DELAY;
          step = 0;
          return;
        }
        default:
          return;
      }
  }
  
}

ButtonState getButtonState()
{
  if (button.released()) {
    if (button.previousDuration() > 3000) {
      // long press
      return ButtonState::LONG;
    } else {
      // short press
      return ButtonState::SHORT;
    }
  }
  return ButtonState::NOTHING;
}