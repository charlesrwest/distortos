/**
 * \file
 * \brief SpiMasterLowLevelDmaBased class header for SPIv1 in STM32
 *
 * \author Copyright (C) 2018-2019 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SOURCE_CHIP_STM32_PERIPHERALS_SPIV1_INCLUDE_DISTORTOS_CHIP_SPIMASTERLOWLEVELDMABASED_HPP_
#define SOURCE_CHIP_STM32_PERIPHERALS_SPIV1_INCLUDE_DISTORTOS_CHIP_SPIMASTERLOWLEVELDMABASED_HPP_

#include "distortos/chip/DmaChannel.hpp"
#include "distortos/chip/DmaChannelFunctorCommon.hpp"

#include "distortos/devices/communication/SpiMasterLowLevel.hpp"

namespace distortos
{

namespace chip
{

class SpiPeripheral;

/**
 * SpiMasterLowLevelDmaBased class is a low-level SPI master driver for SPIv1 in STM32.
 *
 * This driver uses DMA for data transfers.
 *
 * \ingroup devices
 */

class SpiMasterLowLevelDmaBased : public devices::SpiMasterLowLevel
{
public:

	/**
	 * \brief SpiMasterLowLevelDmaBased's constructor
	 *
	 * \param [in] spiPeripheral is a reference to raw SPI peripheral
	 * \param [in] rxDmaChannel is a reference to DMA channel used for reception
	 * \param [in] rxDmaRequest is the request identifier for DMA channel used for reception
	 * \param [in] txDmaChannel is a reference to DMA channel used for transmission
	 * \param [in] txDmaRequest is the request identifier for DMA channel used for transmission
	 */

	constexpr SpiMasterLowLevelDmaBased(const SpiPeripheral& spiPeripheral, DmaChannel& rxDmaChannel,
			const uint8_t rxDmaRequest, DmaChannel& txDmaChannel, const uint8_t txDmaRequest) :
					spiPeripheral_{spiPeripheral},
					rxDmaChannel_{rxDmaChannel},
					txDmaChannel_{txDmaChannel},
					rxDmaChannelUniqueHandle_{},
					txDmaChannelUniqueHandle_{},
					rxDmaChannelFunctor_{*this},
					txDmaChannelFunctor_{*this},
					spiMasterBase_{},
					size_{},
					rxDummyData_{},
					txDummyData_{},
					rxDmaRequest_{rxDmaRequest},
					txDmaRequest_{txDmaRequest},
					started_{},
					wordLength_{8}
	{

	}

	/**
	 * \brief SpiMasterLowLevelDmaBased's destructor
	 *
	 * Does nothing if driver is already stopped. If it's not, performs forced stop of operation.
	 */

	~SpiMasterLowLevelDmaBased() override;

	/**
	 * \brief Configures parameters of low-level SPI master driver.
	 *
	 * \param [in] mode is the desired SPI mode
	 * \param [in] clockFrequency is the desired clock frequency, Hz
	 * \param [in] wordLength selects word length, bits, [4; 16] or [minSpiWordLength; maxSpiWordLength]
	 * \param [in] lsbFirst selects whether MSB (false) or LSB (true) is transmitted first
	 * \param [in] dummyData is the dummy data that will be sent if write buffer of transfer is nullptr
	 *
	 * \return pair with return code (0 on success, error code otherwise) and real clock frequency; error codes:
	 * - EBADF - the driver is not started;
	 * - EBUSY - transfer is in progress;
	 * - error codes returned by configureSpi();
	 */

	std::pair<int, uint32_t> configure(devices::SpiMode mode, uint32_t clockFrequency, uint8_t wordLength,
			bool lsbFirst, uint32_t dummyData) override;

	/**
	 * \brief Starts low-level SPI master driver.
	 *
	 * \return 0 on success, error code otherwise:
	 * - EBADF - the driver is not stopped;
	 * - error codes returned by DmaChannelUniqueHandle::reserve();
	 */

	int start() override;

	/**
	 * \brief Starts asynchronous transfer.
	 *
	 * This function returns immediately. When the transfer is physically finished (either expected number of bytes were
	 * written and read or an error was detected), SpiMasterBase::transferCompleteEvent() will be executed.
	 *
	 * \param [in] spiMasterBase is a reference to SpiMasterBase object that will be notified about completed transfer
	 * \param [in] writeBuffer is the buffer with data that will be written, nullptr to send dummy data
	 * \param [out] readBuffer is the buffer with data that will be read, nullptr to ignore received data
	 * \param [in] size is the size of transfer (size of \a writeBuffer and/or \a readBuffer), bytes, must be even if
	 * number of data bits is in range (8; 16], divisible by 3 if number of data bits is in range (16; 24] or divisible
	 * by 4 if number of data bits is in range (24; 32]
	 *
	 * \return 0 on success, error code otherwise:
	 * - EBADF - the driver is not started;
	 * - EBUSY - transfer is in progress;
	 * - EINVAL - \a size is invalid;
	 * - error codes returned by DmaChannelUniqueHandle::configureTransfer();
	 */

	int startTransfer(devices::SpiMasterBase& spiMasterBase, const void* writeBuffer, void* readBuffer,
			size_t size) override;

	/**
	 * \brief Stops low-level SPI master driver.
	 *
	 * \return 0 on success, error code otherwise:
	 * - EBADF - the driver is not started;
	 * - EBUSY - transfer is in progress;
	 */

	int stop() override;

private:

	/// RxDmaChannelFunctor class is a DmaChannelFunctor for DMA channel used for reception
	class RxDmaChannelFunctor : public DmaChannelFunctor
	{
	public:

		/**
		 * \brief RxDmaChannelFunctor's constructor
		 *
		 * \param [in] owner is a reference to owner SpiMasterLowLevelDmaBased object
		 */

		constexpr explicit RxDmaChannelFunctor(SpiMasterLowLevelDmaBased& owner) :
				owner_{owner}
		{

		}

		/**
		 * \brief "Transfer complete" event
		 *
		 * Called by low-level DMA channel driver when the transfer is physically finished.
		 */

		void transferCompleteEvent() override;

		/**
		 * \brief "Transfer error" event
		 *
		 * Called by low-level DMA channel driver when transfer error is detected.
		 *
		 * \param [in] transactionsLeft is the number of transactions left
		 */

		void transferErrorEvent(size_t transactionsLeft) override;

	private:

		/// reference to owner SpiMasterLowLevelDmaBased object
		SpiMasterLowLevelDmaBased& owner_;
	};

	/// TxDmaChannelFunctor class is a DmaChannelFunctorCommon for DMA channel used for transmission
	class TxDmaChannelFunctor : public DmaChannelFunctorCommon
	{
	public:

		/**
		 * \brief TxDmaChannelFunctor's constructor
		 *
		 * \param [in] owner is a reference to owner SpiMasterLowLevelDmaBased object
		 */

		constexpr explicit TxDmaChannelFunctor(SpiMasterLowLevelDmaBased& owner) :
				owner_{owner}
		{

		}

		/**
		 * \brief "Transfer error" event
		 *
		 * Called by low-level DMA channel driver when transfer error is detected.
		 *
		 * \param [in] transactionsLeft is the number of transactions left
		 */

		void transferErrorEvent(size_t transactionsLeft) override;

	private:

		/// reference to owner SpiMasterLowLevelDmaBased object
		SpiMasterLowLevelDmaBased& owner_;
	};

	/**
	 * \brief "Transfer complete" and "transfer error" event handler
	 *
	 * \param [in] transactionsLeft is the number of transactions left
	 */

	void eventHandler(size_t transactionsLeft);

	/**
	 * \return true if driver is started, false otherwise
	 */

	bool isStarted() const
	{
		return started_;
	}

	/**
	 * \return true if transfer is in progress, false otherwise
	 */

	bool isTransferInProgress() const
	{
		return size_ != 0;
	}

	/// reference to raw SPI peripheral
	const SpiPeripheral& spiPeripheral_;

	/// reference to DMA channel used for reception
	DmaChannel& rxDmaChannel_;

	/// reference to DMA channel used for transmission
	DmaChannel& txDmaChannel_;

	/// unique handle of DMA channel used for reception
	DmaChannel::UniqueHandle rxDmaChannelUniqueHandle_;

	/// unique handle of DMA channel used for transmission
	DmaChannel::UniqueHandle txDmaChannelUniqueHandle_;

	/// functor for DMA channel used for reception
	RxDmaChannelFunctor rxDmaChannelFunctor_;

	/// functor for DMA channel used for transmission
	TxDmaChannelFunctor txDmaChannelFunctor_;

	/// pointer to SpiMasterBase object associated with this one
	devices::SpiMasterBase* volatile spiMasterBase_;

	/// size of transfer, bytes
	volatile size_t size_;

	/// object used as reception DMA target if read buffer of transfer is nullptr
	uint16_t rxDummyData_;

	/// dummy data that will be sent if write buffer of transfer is nullptr
	uint16_t txDummyData_;

	/// request identifier for DMA channel used for reception
	uint8_t rxDmaRequest_;

	/// request identifier for DMA channel used for transmission
	uint8_t txDmaRequest_;

	/// true if driver is started, false otherwise
	bool started_;

	/// selected word length, bits, [4; 16] or [minSpiWordLength; maxSpiWordLength]
	uint8_t wordLength_;
};

}	// namespace chip

}	// namespace distortos

#endif	// SOURCE_CHIP_STM32_PERIPHERALS_SPIV1_INCLUDE_DISTORTOS_CHIP_SPIMASTERLOWLEVELDMABASED_HPP_
