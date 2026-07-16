#ifndef CAR_PERIPHERALS_H
#define CAR_PERIPHERALS_H

#include <Arduino.h>

// ---------------------------------------------------------
// НАЛАШТУВАННЯ (викликати один раз у setup)
// ---------------------------------------------------------
void Car_Peripherals_Setup();

// ---------------------------------------------------------
// КЕРУВАННЯ КОЛЬОРОВИМИ СВІТЛОДІОДАМИ (WS2812)
// ---------------------------------------------------------
// Увімкнути один світлодіод (led_number: 0-11, r, g, b: 0-255)
void LED_SetColor(int led_number, int r, int g, int b);

// Увімкнути ВСІ світлодіоди одним кольором
void LED_SetAllColors(int r, int g, int b);

// Вимкнути один конкретний світлодіод
void LED_TurnOff(int led_number);

// Вимкнути всі світлодіоди відразу
void LED_TurnOffAll();


// ---------------------------------------------------------
// КЕРУВАННЯ ЕКРАНОМ-"ОЧИМА" (LED Matrix)
// ---------------------------------------------------------
// УВАГА: Щоб працювали анімації, цю функцію треба додати у ваш loop()!
void Matrix_Update();

// --- СТАТИЧНІ ЗОБРАЖЕННЯ (вимикають анімацію) ---
void Matrix_Clear();                   // Очистити екран
void Matrix_ShowArrowLeft();           // Показати стрілку вліво
void Matrix_ShowArrowRight();          // Показати стрілку вправо
void Matrix_ShowStaticEmotion(int id); // Показати статичну емоцію (id: від 0 до 20)

// --- АНІМАЦІЇ ---
void Matrix_AnimRotate();              // Очі, що обертаються
void Matrix_AnimBlink();               // Кліпання очима
void Matrix_AnimSmile();               // Посмішка
void Matrix_AnimCry();                 // Плач
void Matrix_AnimWheelLeft();           // Колесо (ліве)
void Matrix_AnimWheelRight();          // Колесо (праве)

#endif