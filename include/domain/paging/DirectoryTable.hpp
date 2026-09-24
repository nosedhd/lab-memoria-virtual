#pragma once

#include "domain/paging/DirectoryTableEntry.hpp"
#include "domain/paging/PageTable.hpp"

#include <cstdint>
#include <memory>
#include <vector>

class DirectoryTable {
public:
	explicit DirectoryTable(uint32_t entry_count = 1024);

	DirectoryTableEntry& getEntry(uint32_t index);
	const DirectoryTableEntry& getEntry(uint32_t index) const;

	bool hasPageTable(uint32_t dir_index) const;
	PageTable& getOrCreatePageTable(
		uint32_t dir_index, uint32_t entries_per_table);

private:
	void validateIndex(uint32_t index) const;

	std::vector<DirectoryTableEntry> entries_;
	std::vector<std::unique_ptr<PageTable>> page_tables_;
};
