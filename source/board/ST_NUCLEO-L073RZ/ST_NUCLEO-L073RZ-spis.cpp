/**
 * \file
 * \brief Definitions of low-level SPI master drivers for SPIv1 in ST,NUCLEO-L073RZ (ST,STM32L073RZ chip)
 *
 * \author Copyright (C) 2016-2018 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * \warning
 * Automatically generated file - do not edit!
 */

#include "distortos/chip/spis.hpp"

#include "distortos/chip/ChipSpiMasterLowLevel.hpp"

namespace distortos
{

namespace chip
{

#ifdef CONFIG_CHIP_STM32_SPIV1_SPI1_ENABLE

/*---------------------------------------------------------------------------------------------------------------------+
| SPI1
+---------------------------------------------------------------------------------------------------------------------*/

ChipSpiMasterLowLevel spi1 {ChipSpiMasterLowLevel::spi1Parameters};

/**
 * \brief SPI1 interrupt handler
 */

extern "C" void SPI1_IRQHandler()
{
	spi1.interruptHandler();
}

#endif	// def CONFIG_CHIP_STM32_SPIV1_SPI1_ENABLE

#ifdef CONFIG_CHIP_STM32_SPIV1_SPI2_ENABLE

/*---------------------------------------------------------------------------------------------------------------------+
| SPI2
+---------------------------------------------------------------------------------------------------------------------*/

ChipSpiMasterLowLevel spi2 {ChipSpiMasterLowLevel::spi2Parameters};

/**
 * \brief SPI2 interrupt handler
 */

extern "C" void SPI2_IRQHandler()
{
	spi2.interruptHandler();
}

#endif	// def CONFIG_CHIP_STM32_SPIV1_SPI2_ENABLE

}	// namespace chip

}	// namespace distortos
