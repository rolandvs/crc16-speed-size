/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// Benchmark function
typedef struct {
    const char *name;
    uint16_t (*func)(const uint8_t*, size_t);
    uint16_t result;
    uint32_t time_ms;
    uint32_t bytes_per_sec;
} BenchmarkResult;


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */


// Full 256-entry lookup table (512 bytes)
static const uint16_t crc16_table_full[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

// 16-entry nibble table (32 bytes)
static const uint16_t crc16_table_nibble[16] = {
    0x0000, 0x1021, 0x2042, 0x3063,
    0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b,
    0xc18c, 0xd1ad, 0xe1ce, 0xf1ef
};



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


// Full 256-entry table
uint16_t crc16_full_table(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF;
    
    for (size_t i = 0; i < length; i++) {
        uint8_t index = (crc >> 8) ^ data[i];
        crc = (crc << 8) ^ crc16_table_full[index];
    }
    return crc;
}

// Nibble-based table (32 bytes)
uint16_t crc16_nibble_table(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF;
    
    for (size_t i = 0; i < length; i++) {
        // Process high nibble
        crc = (crc << 4) ^ crc16_table_nibble[((crc >> 12) ^ (data[i] >> 4)) & 0x0F];
        // Process low nibble
        crc = (crc << 4) ^ crc16_table_nibble[((crc >> 12) ^ (data[i] & 0x0F)) & 0x0F];
    }
    return crc;
}

// Algorithmic (no table)
uint16_t crc16_algorithmic(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF;
    
    for (size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;
        
        for (int bit = 0; bit < 8; bit++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ CRC16_POLY;
            } else {
                crc = crc << 1;
            }
        }
    }
    return crc;
}

/* Byte-at-a-time — the reference implementation */
uint16_t crc16_hardware(const uint8_t *data, size_t length)
{
    hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;

    /* HAL_CRC_Calculate resets DR to InitValue (0xFFFF) on entry */
    return (uint16_t)HAL_CRC_Calculate(&hcrc, (uint32_t *)(uintptr_t)data, (uint32_t)length);
}


/* Word-at-a-time — same result, fewer bus transactions */
uint16_t crc16_hardware_fast(const uint8_t *data, size_t length)
{
    uint32_t chunk[16];
    size_t   i      = 0;
    size_t   nwords = length / 4U;

    /* Length 0 seeds DR with InitValue and gives us a value to accumulate onto */
    hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
    uint32_t crc = HAL_CRC_Calculate(&hcrc, (uint32_t *)(uintptr_t)data, 0U);

    hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_WORDS;
    while (nwords != 0U) {
        size_t n = (nwords > 16U) ? 16U : nwords;

        for (size_t k = 0; k < n; k++) {
            uint32_t w;
            memcpy(&w, data + i + (4U * k), 4U);  /* alignment-safe */
            chunk[k] = __REV(w);                   /* little-endian -> MSB-first */
        }

        crc = HAL_CRC_Accumulate(&hcrc, chunk, (uint32_t)n);
        i      += 4U * n;
        nwords -= n;
    }

    if (i < length) {                              /* 1-3 trailing bytes */
        hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
        crc = HAL_CRC_Accumulate(&hcrc,
                                 (uint32_t *)(uintptr_t)(data + i),
                                 (uint32_t)(length - i));
    }

    return (uint16_t)crc;
}

// benchmarking function
void benchmark(BenchmarkResult *result, const uint8_t *data, size_t length, int iterations) {
    uint32_t start, end;
    uint16_t crc = 0;
    
    // Warm-up
    crc = result->func(data, length);
    
    // Actual benchmark
    start = HAL_GetTick();
    for (int i = 0; i < iterations; i++) {
        crc = result->func(data, length);
    }
    end = HAL_GetTick();
    
    result->result = crc;
    result->time_ms = end - start;
    
    // Calculate throughput
    if (result->time_ms > 0) {
        uint64_t total_bytes = (uint64_t)length * iterations;
        result->bytes_per_sec = (uint32_t)((total_bytes * 1000) / result->time_ms);
    } else {
        result->bytes_per_sec = 0;
    }
}


void uart_putc(uint8_t c) {
    HAL_UART_Transmit(&huart2, &c, 1, HAL_MAX_DELAY);
}

void uart_puts(const char *str) {
    while (*str) {
        if (*str == '\n') uart_putc('\r');
        uart_putc(*str++);
    }
}

void print_result(BenchmarkResult *result) {
    char buf[100];
    sprintf(buf, "%-25s CRC: 0x%04X  Time: %5lu ms  Speed: %7lu bytes/s\n",
            result->name, result->result, result->time_ms, result->bytes_per_sec);
    uart_puts(buf);
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */


  // Test data setup
  #define TEST_SIZE 1000
  #define ITERATIONS 1000
    
  static uint8_t test_data[TEST_SIZE];
  for (size_t i = 0; i < TEST_SIZE; i++) {
      test_data[i] = (uint8_t)(i * 13);
  }
    
  // Results array
  BenchmarkResult results[5];
    
  // Initialize result structures
  results[0].name = "Full table (512 bytes)";
  results[0].func = crc16_full_table;
  
  results[1].name = "Nibble table (32 bytes)";
  results[1].func = crc16_nibble_table;
  
  results[2].name = "Algorithmic (0 bytes)";
  results[2].func = crc16_algorithmic;
  
  results[3].name = "Hardware CRC (byte)";
  results[3].func = crc16_hardware;
  
  results[4].name = "Hardware CRC (word)";
  results[4].func = crc16_hardware_fast;
  
  // Run benchmarks
  for (int i = 0; i < 5; i++) {
      benchmark(&results[i], test_data, TEST_SIZE, ITERATIONS);
  }
  
  uart_puts("\n=== CRC-16 Benchmark Results (STM32F091RC @ 48MHz) ===\n\n");
  for (int i = 0; i < 5; i++) {
       print_result(&results[i]);
  }
    
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
