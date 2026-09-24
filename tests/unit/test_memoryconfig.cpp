#include <cassert>
#include <stdexcept>

#include "domain/config/MemoryConfig.hpp"

void test_valid_configuration() {
	const MemoryConfig config(4096, 256 * 1024);

	assert(config.getPageSize() == 4096);
	assert(config.getPhysicalMemorySize() == 256 * 1024);
	assert(config.getFrameCount() == 64);
	assert(config.getOffsetBits() == 12);
	assert(config.getDirectoryIndexBits() == 10);
	assert(config.getPageTableIndexBits() == 10);
}

void test_different_page_size() {
	const MemoryConfig config(8192, 256 * 1024);

	assert(config.getFrameCount() == 32);
	assert(config.getOffsetBits() == 13);
	assert(config.getDirectoryIndexBits() == 10);
	assert(config.getPageTableIndexBits() == 9);
}

void test_invalid_configurations() {
	bool thrown = false;
	try {
		MemoryConfig config(3000, 256 * 1024);
	} catch (const std::invalid_argument&) {
		thrown = true;
	}
	assert(thrown);

	thrown = false;
	try {
		MemoryConfig config(4096, 128 * 1024);
	} catch (const std::invalid_argument&) {
		thrown = true;
	}
	assert(thrown);

	thrown = false;
	try {
		MemoryConfig config(4096, 256 * 1024 + 1);
	} catch (const std::invalid_argument&) {
		thrown = true;
	}
	assert(thrown);
}

int main() {
	test_valid_configuration();
	test_different_page_size();
	test_invalid_configurations();
	return 0;
}