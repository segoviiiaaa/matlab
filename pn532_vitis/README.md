# PN532 Example for Zybo Z7 in Vitis 2023.1

Este ejemplo muestra la forma más sencilla de inicializar el módulo NFC PN532 mediante I2C y leer el UID de una tarjeta NFC. El proyecto debe crearse en Vitis 2023.1 con la BSP generada para el procesador Zynq de la Zybo Z7.

## Hardware
- Zybo Z7 (Zynq-7000)
- Módulo PN532 conectado por I2C. Pines sugeridos:
  - SDA → PS I2C0 SDA
  - SCL → PS I2C0 SCL
  - IRQ → GPIO opcional
  - GND y 3.3V

## Compilación
1. Crear un nuevo "Application Project" en Vitis utilizando la plataforma de la Zybo Z7.
2. Copiar los archivos `pn532.c`, `pn532.h` y `main.c` al directorio `src` del proyecto.
3. Compilar y programar la FPGA.

Al ejecutar, se mostrará el UID de la tarjeta detectada por consola.
