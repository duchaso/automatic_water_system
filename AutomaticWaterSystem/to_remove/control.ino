void processButtonState()
{
  switch (settingsMode)
  {
    case SettingsMode::SET_DRAINAGE_DELAY:
      switch (buttonState)
      {
        #if ENABLE_RTC
        case ButtonState::SHORT:
        {
          settingsMode = SettingsMode::SET_RTC_TRIGGER_TIME;
          return;
        }
        #endif
        case ButtonState::LONG:
        {
          settingsMode = SettingsMode::RUN_PUMP;
          relay.setPump(Relay::ON);
          return;
        }
        default:
          return;
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

    #if ENABLE_RTC
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
        default:
          return;
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
        default:
          return;
      }
    }
    #endif

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