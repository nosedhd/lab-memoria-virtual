#pragma once

#include "application/SimulationReport.hpp"

class IReportExporter {
public:
    virtual ~IReportExporter() = default;
    virtual void exportReport(const SimulationReport& report) = 0;
};
