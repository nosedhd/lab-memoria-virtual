#include "domain/paging/VirtualAddress.hpp"

#include <stdexcept>

VirtualAddress::VirtualAddress(uint32_t raw_address, const MemoryConfig& config)
	: raw_address_(raw_address),
	  directory_index_(0),
	  table_index_(0),
	  offset_(0),
	  vpn_(0) {
	if (!isValidVirtualAddress(raw_address, config)) {
		throw std::invalid_argument("La direccion virtual no es valida");
	}

	directory_index_ = computeDirectoryIndex(raw_address, config);
	table_index_ = computeTableIndex(raw_address, config);
	offset_ = computeOffset(raw_address, config);
	vpn_ = computeVpn(raw_address, config);
}

uint32_t VirtualAddress::getRaw() const {
	return raw_address_;
}

uint32_t VirtualAddress::getDirectoryIndex() const {
	return directory_index_;
}

uint32_t VirtualAddress::getTableIndex() const {
	return table_index_;
}

uint32_t VirtualAddress::getOffset() const {
	return offset_;
}

uint32_t VirtualAddress::getVpn() const {
	return vpn_;
}

bool VirtualAddress::isValidVirtualAddress(
	uint32_t raw_address, const MemoryConfig& config) {
	(void)raw_address;
	const uint32_t offset_bits = config.getOffsetBits();
	const uint32_t directory_bits = config.getDirectoryIndexBits();
	const uint32_t table_bits = config.getPageIndexBits();

	return offset_bits + directory_bits + table_bits <=
		   MemoryConfig::VIRTUAL_ADDRESS_BITS;
}

uint32_t VirtualAddress::computeDirectoryIndex(
	uint32_t raw_address, const MemoryConfig& config) {
	const uint32_t shift = config.getOffsetBits() + config.getPageIndexBits();
	const uint32_t mask = (1U << config.getDirectoryIndexBits()) - 1U;
	return (raw_address >> shift) & mask;
}

uint32_t VirtualAddress::computeTableIndex(
	uint32_t raw_address, const MemoryConfig& config) {
	const uint32_t offset_bits = config.getOffsetBits();
	const uint32_t mask = (1U << config.getPageIndexBits()) - 1U;
	return (raw_address >> offset_bits) & mask;
}

uint32_t VirtualAddress::computeOffset(
	uint32_t raw_address, const MemoryConfig& config) {
	const uint32_t mask = (1U << config.getOffsetBits()) - 1U;
	return raw_address & mask;
}

uint32_t VirtualAddress::computeVpn(
	uint32_t raw_address, const MemoryConfig& config) {
	return raw_address >> config.getOffsetBits();
}
