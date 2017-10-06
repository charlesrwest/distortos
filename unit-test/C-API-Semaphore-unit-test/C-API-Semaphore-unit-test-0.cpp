/**
 * \file
 * \brief Semaphore C-API test cases
 *
 * \author Copyright (C) 2017 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "distortos/fromCApi.hpp"
#include "distortos/Semaphore.hpp"
#include "distortos/C-API/Semaphore.h"

using trompeloeil::_;

/*---------------------------------------------------------------------------------------------------------------------+
| global test cases
+---------------------------------------------------------------------------------------------------------------------*/

TEST_CASE("Testing DISTORTOS_SEMAPHORE_INITIALIZER()", "[initializer]")
{
	constexpr unsigned int lowRandomValue {0x0f86a4ba};
	constexpr unsigned int highRandomValue {0x34e0865d};

	{
		const distortos_Semaphore semaphore =
				DISTORTOS_SEMAPHORE_INITIALIZER(semaphore, lowRandomValue, highRandomValue);
		REQUIRE(semaphore.value == lowRandomValue);
		REQUIRE(semaphore.maxValue == highRandomValue);
	}
	{
		const distortos_Semaphore semaphore =
				DISTORTOS_SEMAPHORE_INITIALIZER(semaphore, highRandomValue, lowRandomValue);
		REQUIRE(semaphore.value == lowRandomValue);
		REQUIRE(semaphore.maxValue == lowRandomValue);
	}
}

TEST_CASE("Testing DISTORTOS_SEMAPHORE_CONSTRUCT_1()", "[construct]")
{
	constexpr unsigned int lowRandomValue {0x0c9bd95c};
	constexpr unsigned int highRandomValue {0x3500b8fa};

	{
		DISTORTOS_SEMAPHORE_CONSTRUCT_1(semaphore, lowRandomValue, highRandomValue);
		REQUIRE(semaphore.value == lowRandomValue);
		REQUIRE(semaphore.maxValue == highRandomValue);
	}
	{
		DISTORTOS_SEMAPHORE_CONSTRUCT_1(semaphore, highRandomValue, lowRandomValue);
		REQUIRE(semaphore.value == lowRandomValue);
		REQUIRE(semaphore.maxValue == lowRandomValue);
	}
}

TEST_CASE("Testing DISTORTOS_SEMAPHORE_CONSTRUCT()", "[construct]")
{
	constexpr unsigned int randomValue {0x26564b7f};

	{
		DISTORTOS_SEMAPHORE_CONSTRUCT(semaphore, randomValue);
		REQUIRE(semaphore.value == randomValue);
		REQUIRE(semaphore.maxValue == UINT_MAX);
	}
}

TEST_CASE("Testing distortos_Semaphore_construct_1()", "[construct]")
{
	constexpr unsigned int lowRandomValue {0x799c4209};
	constexpr unsigned int highRandomValue {0xc5ffe5ab};

	distortos::FromCApiSemaphoreMock fromCApiMock;
	distortos::Semaphore semaphoreMock;
	std::aligned_storage<sizeof(distortos::Semaphore), alignof(distortos::Semaphore)>::type storage;

	REQUIRE(distortos_Semaphore_construct_1(nullptr, lowRandomValue, highRandomValue) == EINVAL);

	distortos::Semaphore::getProxyInstance() = &semaphoreMock;
	REQUIRE_CALL(semaphoreMock, construct(lowRandomValue, highRandomValue));
	REQUIRE(distortos_Semaphore_construct_1(reinterpret_cast<distortos_Semaphore*>(&storage), lowRandomValue,
			highRandomValue) == 0);
	distortos::Semaphore::getProxyInstance() = {};

	reinterpret_cast<distortos::Semaphore*>(&storage)->~Semaphore();
}

TEST_CASE("Testing distortos_Semaphore_construct()", "[construct]")
{
	constexpr unsigned int randomValue {0x9ba07851};

	distortos::FromCApiSemaphoreMock fromCApiMock;
	distortos::Semaphore semaphoreMock;
	std::aligned_storage<sizeof(distortos::Semaphore), alignof(distortos::Semaphore)>::type storage;

	REQUIRE(distortos_Semaphore_construct(nullptr, randomValue) == EINVAL);

	distortos::Semaphore::getProxyInstance() = &semaphoreMock;
	REQUIRE_CALL(semaphoreMock, construct(randomValue, UINT_MAX));
	REQUIRE(distortos_Semaphore_construct(reinterpret_cast<distortos_Semaphore*>(&storage), randomValue) == 0);
	distortos::Semaphore::getProxyInstance() = {};

	reinterpret_cast<distortos::Semaphore*>(&storage)->~Semaphore();
}

TEST_CASE("Testing distortos_Semaphore_destruct()", "[destruct]")
{
	distortos::FromCApiSemaphoreMock fromCApiMock;
	trompeloeil::deathwatched<distortos::Semaphore> semaphoreMock;
	distortos_Semaphore semaphore;

	REQUIRE(distortos_Semaphore_destruct(nullptr) == EINVAL);

	{
		REQUIRE_CALL(fromCApiMock, getSemaphore(_)).LR_WITH(&_1 == &semaphore).LR_RETURN(std::ref(semaphoreMock));
		REQUIRE_DESTRUCTION(semaphoreMock);
		REQUIRE(distortos_Semaphore_destruct(&semaphore) == 0);
	}
}

TEST_CASE("Testing distortos_Semaphore_getValue()", "[getValue]")
{
	constexpr unsigned int randomValue {0x36e9ebac};

	distortos::FromCApiSemaphoreMock fromCApiMock;
	distortos::Semaphore semaphoreMock;
	const distortos_Semaphore semaphore {};
	unsigned int value;

	REQUIRE(distortos_Semaphore_getValue(nullptr, nullptr) == EINVAL);
	REQUIRE(distortos_Semaphore_getValue(nullptr, &value) == EINVAL);
	REQUIRE(distortos_Semaphore_getValue(&semaphore, nullptr) == EINVAL);

	REQUIRE_CALL(fromCApiMock, getConstSemaphore(_)).LR_WITH(&_1 == &semaphore).LR_RETURN(std::ref(semaphoreMock));
	REQUIRE_CALL(semaphoreMock, getValue()).RETURN(randomValue);
	REQUIRE(distortos_Semaphore_getValue(&semaphore, &value) == 0);
	REQUIRE(value == randomValue);
}

TEST_CASE("Testing distortos_Semaphore_post()", "[post]")
{
	distortos::FromCApiSemaphoreMock fromCApiMock;
	distortos::Semaphore semaphoreMock;
	distortos_Semaphore semaphore;

	REQUIRE(distortos_Semaphore_post(nullptr) == EINVAL);

	REQUIRE_CALL(fromCApiMock, getSemaphore(_)).LR_WITH(&_1 == &semaphore).LR_RETURN(std::ref(semaphoreMock));
	REQUIRE_CALL(semaphoreMock, post()).RETURN(EOVERFLOW);
	REQUIRE(distortos_Semaphore_post(&semaphore) == EOVERFLOW);
}

TEST_CASE("Testing distortos_Semaphore_tryWait()", "[tryWait]")
{
	distortos::FromCApiSemaphoreMock fromCApiMock;
	distortos::Semaphore semaphoreMock;
	distortos_Semaphore semaphore;

	REQUIRE(distortos_Semaphore_tryWait(nullptr) == EINVAL);

	REQUIRE_CALL(fromCApiMock, getSemaphore(_)).LR_WITH(&_1 == &semaphore).LR_RETURN(std::ref(semaphoreMock));
	REQUIRE_CALL(semaphoreMock, tryWait()).RETURN(EAGAIN);
	REQUIRE(distortos_Semaphore_tryWait(&semaphore) == EAGAIN);
}

TEST_CASE("Testing distortos_Semaphore_tryWaitFor()", "[tryWaitFor]")
{
	constexpr int64_t randomDuration {0x3f1f8c067f94aa71};

	distortos::FromCApiSemaphoreMock fromCApiMock;
	distortos::Semaphore semaphoreMock;
	distortos_Semaphore semaphore;

	REQUIRE(distortos_Semaphore_tryWaitFor(nullptr, randomDuration) == EINVAL);

	REQUIRE_CALL(fromCApiMock, getSemaphore(_)).LR_WITH(&_1 == &semaphore).LR_RETURN(std::ref(semaphoreMock));
	const auto duration = distortos::TickClock::duration{randomDuration};
	REQUIRE_CALL(semaphoreMock, tryWaitFor(duration)).RETURN(ETIMEDOUT);
	REQUIRE(distortos_Semaphore_tryWaitFor(&semaphore, randomDuration) == ETIMEDOUT);
}

TEST_CASE("Testing distortos_Semaphore_tryWaitUntil()", "[tryWaitUntil]")
{
	constexpr int64_t randomTimePoint {0x6b8cbece059cfa31};

	distortos::FromCApiSemaphoreMock fromCApiMock;
	distortos::Semaphore semaphoreMock;
	distortos_Semaphore semaphore;

	REQUIRE(distortos_Semaphore_tryWaitUntil(nullptr, randomTimePoint) == EINVAL);

	REQUIRE_CALL(fromCApiMock, getSemaphore(_)).LR_WITH(&_1 == &semaphore).LR_RETURN(std::ref(semaphoreMock));
	const auto timePoint = distortos::TickClock::time_point{distortos::TickClock::duration{randomTimePoint}};
	REQUIRE_CALL(semaphoreMock, tryWaitUntil(timePoint)).RETURN(ETIMEDOUT);
	REQUIRE(distortos_Semaphore_tryWaitUntil(&semaphore, randomTimePoint) == ETIMEDOUT);
}

TEST_CASE("Testing distortos_Semaphore_wait()", "[wait]")
{
	distortos::FromCApiSemaphoreMock fromCApiMock;
	distortos::Semaphore semaphoreMock;
	distortos_Semaphore semaphore;

	REQUIRE(distortos_Semaphore_wait(nullptr) == EINVAL);

	REQUIRE_CALL(fromCApiMock, getSemaphore(_)).LR_WITH(&_1 == &semaphore).LR_RETURN(std::ref(semaphoreMock));
	REQUIRE_CALL(semaphoreMock, wait()).RETURN(EINTR);
	REQUIRE(distortos_Semaphore_wait(&semaphore) == EINTR);
}
