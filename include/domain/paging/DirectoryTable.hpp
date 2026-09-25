#pragma once

#include "domain/paging/DirectoryTableEntry.hpp"
#include "domain/paging/PageTable.hpp"

#include <cstdint>
#include <memory>
#include <vector>

class DirectoryTable {
public:
	explicit DirectoryTable(uint32_t entry_count);

	DirectoryTableEntry& getEntry(uint32_t index);
	const DirectoryTableEntry& getEntry(uint32_t index) const;

	bool hasPageTable(uint32_t directory_index) const;
	PageTable& getOrCreatePageTable(  
		uint32_t directory_index, uint32_t entries_per_table);

private:
	void validateIndex(uint32_t directory_index) const;

	std::vector<DirectoryTableEntry> entries_;
	std::vector<std::unique_ptr<PageTable>> page_tables_;
};

