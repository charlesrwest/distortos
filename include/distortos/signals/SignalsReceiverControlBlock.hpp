/**
 * \file
 * \brief SignalsReceiverControlBlock class header
 *
 * \author Copyright (C) 2015 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * \date 2015-03-14
 */

#ifndef INCLUDE_DISTORTOS_SIGNALS_SIGNALSRECEIVERCONTROLBLOCK_HPP_
#define INCLUDE_DISTORTOS_SIGNALS_SIGNALSRECEIVERCONTROLBLOCK_HPP_

#include <cstdint>

namespace distortos
{

class SignalSet;

namespace scheduler
{

class ThreadControlBlock;

}	// namespace scheduler

namespace synchronization
{

/// SignalsReceiverControlBlock class is a structure required by threads for "receiving" of signals
class SignalsReceiverControlBlock
{
public:

	/**
	 * \brief SignalsReceiverControlBlock's constructor
	 */

	constexpr SignalsReceiverControlBlock() :
			threadControlBlock_{}
	{

	}

	/**
	 * \brief Accepts (clears) one of signals that are pending.
	 *
	 * This should be called when the signal is "accepted".
	 *
	 * \param [in] signalNumber is the signal that will be accepted, [0; 31]
	 *
	 * \return 0 on success, error code otherwise:
	 * - EINVAL - \a signalNumber value is invalid;
	 */

	int acceptPendingSignal(uint8_t signalNumber) const;

	/**
	 * \param [in] threadControlBlock is a reference to associated ThreadControlBlock
	 */

	void setThreadControlBlock(scheduler::ThreadControlBlock& threadControlBlock)
	{
		threadControlBlock_ = &threadControlBlock;
	}

	/**
	 * \param [in] signalSet is a pointer to set of signals that will be "waited for", nullptr when wait was terminated
	 */

	void setWaitingSignalSet(const SignalSet* signalSet) const;

private:

	/// pointer to associated ThreadControlBlock
	scheduler::ThreadControlBlock* threadControlBlock_;
};

}	// namespace synchronization

}	// namespace distortos

#endif	// INCLUDE_DISTORTOS_SIGNALS_SIGNALSRECEIVERCONTROLBLOCK_HPP_
