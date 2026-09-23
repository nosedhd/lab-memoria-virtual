#include "domain/paging/VirtualAddress.hpp"
#include <stdexcept>
#include <cmath>

VirtualAddress::VirtualAddress(uint32_t raw_address, const MemoryConfig& config)
	: raw_address_(raw_address),
	  directory_index_(0),
	  page_table_index_(0),
	  offset_(0),
	  vpn_(0) {
	if (!isValidVirtualAddress(raw_address, config)) {
		throw std::invalid_argument("La configuracion de bits no es valida");
	}

	directory_index_ = computeDirectoryIndex(raw_address, config);
	page_table_index_ = computePageTableIndex(raw_address, config);
	offset_ = computeOffset(raw_address, config);
	vpn_ = computeVpn(raw_address, config);
}

uint32_t VirtualAddress::getRaw() const {
	return raw_address_;
}

uint32_t VirtualAddress::getDirectoryIndex() const {
	return directory_index_;
}

uint32_t VirtualAddress::getPageTableIndex() const {
	return page_table_index_;
}

uint32_t VirtualAddress::getOffset() const {
	return offset_;
}

uint32_t VirtualAddress::getVpn() const {
	return vpn_;
}

bool VirtualAddress::isValidVirtualAddress(uint32_t raw_address, const MemoryConfig& config) {
	(void)raw_address;
	const uint32_t offset_bits = config.getOffsetBits();
	const uint32_t directory_bits = config.getDirectoryIndexBits();
	const uint32_t page_table_bits = config.getPageTableIndexBits();

	return offset_bits + directory_bits + page_table_bits <=
		   MemoryConfig::VIRTUAL_ADDRESS_BITS;
}

uint32_t VirtualAddress::computeDirectoryIndex(uint32_t raw_address, const MemoryConfig& config) {
	const uint32_t shift = config.getOffsetBits() + config.getPageTableIndexBits();
	return raw_address >> shift;
}

uint32_t VirtualAddress::computePageTableIndex(uint32_t raw_address, const MemoryConfig& config) {
	const uint32_t vpn = computeVpn(raw_address, config);
	const uint32_t directory_index = computeDirectoryIndex(raw_address, config);
	const uint32_t entries_per_page_table = static_cast<uint32_t>(std::pow(2, config.getPageTableIndexBits()));
	return vpn - directory_index * entries_per_page_table;
}

uint32_t VirtualAddress::computeOffset(uint32_t raw_address, const MemoryConfig& config) {
	return raw_address % config.getPageSize();
}

uint32_t VirtualAddress::computeVpn(uint32_t raw_address, const MemoryConfig& config) {
	return raw_address >> config.getOffsetBits();
}