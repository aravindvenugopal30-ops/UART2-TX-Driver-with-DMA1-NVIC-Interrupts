# STM32F446RE Bare-Metal UART2 TX Driver with DMA1 & NVIC Interrupts

A bare-metal C firmware project for the **STM32F446RE** (ARM Cortex-M4) microcontroller demonstrating asynchronous serial transmission over **USART2** using **DMA1 Stream 6** and **NVIC interrupts**, written completely from scratch without STM32 HAL or CMSIS libraries.

## Features
* **Pure Register-Level C:** Direct peripheral configuration via custom memory-mapped hardware register definitions.
* **DMA-Driven UART TX:** Offloads CPU overhead by transferring data from SRAM directly to `USART2_DR` using DMA1 in Memory-to-Peripheral mode.
* **Interrupt-Driven Synchronization:** Uses the DMA1 Stream 6 Transfer Complete Interrupt (`TCIF6`) mapped to `NVIC_ISER0` (IRQ 17) to notify the main loop upon completion.
* **Polling UART Fallback:** Includes a lightweight register-level polling string transmitter (`uart2_write_string`) for post-transfer status logging.

## Hardware & Peripheral Configuration

| Parameter | Configuration |
| :--- | :--- |
| **MCU** | STM32F446RE (ARM Cortex-M4) |
| **UART Peripheral** | `USART2` (`PA2` TX, Alternate Function `AF7`) |
| **Baud Rate** | `115200 bps` (`16 MHz` HSI system clock) |
| **DMA Controller** | `DMA1`, **Stream 6**, **Channel 4** |
| **Transfer Mode** | Memory-to-Peripheral (`DIR = 01`), Memory Increment (`MINC = 1`) |
| **Interrupt Vector** | `DMA1_Stream6_IRQHandler` (IRQ 17 via `NVIC_ISER0`) |

## Architecture & Flow

1. **UART2 Initialization (`uar2_tx_init`):**
   * Enables `GPIOA` (`AHB1ENR`) and `USART2` (`APB1ENR`) peripheral clocks.
   * Configures `PA2` as Alternate Function (`AF7`) for `USART2_TX`.
   * Sets baud rate divisor (`USART2_BRR`) and enables Transmitter (`TE`), DMA Transmit mode (`DMAT`), and USART (`UE`).
2. **DMA1 Stream 6 Setup (`dma1_stream6_init`):**
   * Enables `DMA1` clock via `AHB1ENR`.
   * Disables Stream 6, clears existing flags in `DMA1_HIFCR`, and sets Source Address (`M0AR`), Destination Address (`PAR = &USART2_DR`), and Transfer Count (`NDTR`).
   * Selects Channel 4, Memory-to-Peripheral direction, Memory Increment, and Transfer Complete Interrupt Enable (`TCIE`), then enables the stream (`EN`).
3. **NVIC & ISR (`DMA1_Stream6_IRQHandler`):**
   * Enables IRQ 17 in `NVIC_ISER0` and activates global interrupts (`cpsie i`).
   * When the DMA transfer completes, the ISR clears the transfer complete flag (`CTCIF6` in `DMA1_HIFCR`) and sets a `volatile` software flag (`flag_C = 1`).
4. **Main Loop Execution:**
   * Polls `flag_C` in the `while(1)` loop, ensures UART transmission completion, logs a confirmation string over serial, and resets `flag_C`.

## Build & Test
* **Toolchain:** STM32CubeIDE / ARM GCC
* **Serial Monitor:** Termite / PuTTY (`115200` baud, `8N1`, No Flow Control)
