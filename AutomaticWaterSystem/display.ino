// void display()
// {
//   static elapsedMillis displayDelay;
//   static elapsedMillis blankDelay;
//   static bool blank = false;

//   if (displayDelay > 10)
//   {
//     displayDelay = 0;
//     if (step > 0)
//     {
//       if (timeLeft == 0) {
//         if (rtc.isrunning())
//         {
//           unsigned int t = now.hour() * 100 + now.minute();
//           sevseg.setNumber(t, 2);
//         } else {
//           sevseg.setChars("TOFF");
//         }
//       } else {
//         sevseg.setNumber(timeLeft);
//       }
//     } else {
//       switch (settingsMode) {
//         case SettingsMode::RUN_PUMP:
//         {
//           sevseg.setChars("on");
//           return;
//         }
//         case SettingsMode::SET_DRAINAGE_DELAY:
//         {
//           sevseg.setNumber(drainageDelay);
//           return;
//         }
//         case SettingsMode::SET_RTC_TRIGGER_TIME:
//         {
//           if (rtc.isrunning())
//           {
//             sevseg.setNumber(rtcTriggerTime.hour() * 100 + rtcTriggerTime.minute(), 2);
//           } else {
//             sevseg.setChars("----");
//           }
//           return;
//         }
//         case SettingsMode::ADJUST_RTC_MODULE_H:
//         case SettingsMode::ADJUST_RTC_MODULE_M:
//         {
//           if (!rtc.isrunning())
//           {
//             sevseg.setChars("----");
//             return;
//           }

//           if (blankDelay > 200)
//           {
//             blankDelay = 0;
//             blank = !blank;
//             blank ?
//               sevseg.blank() :
//               sevseg.setNumber(newTime.hour() * 100 + newTime.minute(), 2);
//           }
//           return;
//         }
//         case SettingsMode::OFF:
//         {
//           if (blankDelay > 500)
//           {
//             blankDelay = 0;
//             blank = !blank;
//             blank ?
//               sevseg.setChars("off") :
//               sevseg.setNumber((int)errorCode);
//           }
//           return;  
//         }

//       }
//     }
//   }
// }